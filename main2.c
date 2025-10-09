// jogo3d.c
// Jogo estilo Othello com efeito pseudo-3D no terminal Linux
// Compile: gcc -std=c11 -O2 -o jogo3d jogo3d.c

#define _POSIX_C_SOURCE 200112L
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define N 6
#define TOTAL_JOGADAS 30
#define ui unsigned int

typedef enum {
  BRANCO = 'O',
  PRETO = 'X',
  VERTICAL = '|',
  HORIZONTAL = '-',
  DIAGONAL_PRINCIPAL = '\\',
  DIAGONAL_SECUNDARIA = '/'
} Casa;

typedef struct {
  Casa grid[N][N];
} Tabuleiro;

typedef struct {
  int linha;
  int coluna;
} Coordenada;

/* protótipos */
void inicia_tab(Tabuleiro *t, ui n);
void salva_estado(Tabuleiro *t, Casa jogador_atual, int contador_de_jogadas,
                  Coordenada *coordenada_atual, Casa tipo_casa_anterior);
void carrega_partida(int *contador_de_jogadas, Tabuleiro *t,
                     Coordenada *coordenada_anterior, Casa *tipo_casa_anterior,
                     Casa *jogador_atual);
void imprimir_3d(const Tabuleiro *t, int cursor_l, int cursor_c,
                 bool possiveis[N][N], Coordenada coordenada_anterior,
                 int contador_de_jogadas, Casa jogador_atual);
void calcula_possiveis(Tabuleiro *t, bool possiveis[N][N],
                       Coordenada coordenada_anterior, Casa tipo_casa_anterior,
                       int contador_de_jogadas);
void inserir(Tabuleiro *t, int linha, int coluna, Casa peca);
void fazInsert(Tabuleiro *t, int linha, int coluna, Casa jogador_atual);
int verificaVitoria(Tabuleiro *t, Casa jogador_que_jogou,
                    int contador_de_jogadas);
int validaJogada_simples(Tabuleiro *t, int linha, int coluna, ui n,
                         Coordenada coordenada_anterior,
                         int contador_de_jogadas, Casa tipo_casa_anterior);
int contarMaiorArea(Tabuleiro *t, Casa jogador);
int contarRegiao(Tabuleiro *t, int linha, int coluna, Casa jogador,
                 bool visitado[N][N]);

/* Terminal raw mode */
static struct termios orig_termios;
void disableRawMode(void) { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }
void enableRawMode(void) {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
    perror("tcgetattr");
    exit(1);
  }
  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON); // sem echo, sem canonical
  raw.c_iflag &= ~(IXON);          // desativa ctrl-s/ctrl-q
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    perror("tcsetattr");
    exit(1);
  }
}

/* ANSI helpers */
#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"
#define INV "\x1b[7m"
#define FG_BLACK "\x1b[30m"
#define FG_RED "\x1b[31m"
#define FG_GREEN "\x1b[32m"
#define FG_YELLOW "\x1b[33m"
#define FG_BLUE "\x1b[34m"
#define FG_MAGENTA "\x1b[35m"
#define FG_CYAN "\x1b[36m"
#define BG_RED "\x1b[41m"
#define BG_GREEN "\x1b[42m"
#define BG_YELLOW "\x1b[43m"
#define BG_BLUE "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN "\x1b[46m"
#define BG_WHITE "\x1b[47m"

/* --- implementação --- */

void inicia_tab(Tabuleiro *t, ui n) {
  ui total = n * n;
  Casa saco[total];
  Casa pecas[] = {VERTICAL, HORIZONTAL, DIAGONAL_PRINCIPAL,
                  DIAGONAL_SECUNDARIA};
  ui por = total / 4;
  int idx = 0;
  for (int i = 0; i < 4; ++i)
    for (ui j = 0; j < por; ++j)
      saco[idx++] = pecas[i];
  // completar com HORIZONTAL se sobrar (caso total%4!=0)
  while (idx < (int)total) {
    saco[idx++] = HORIZONTAL;
  }
  // embaralha
  for (int i = total - 1; i > 0; --i) {
    int j = rand() % (i + 1);
    Casa tmp = saco[i];
    saco[i] = saco[j];
    saco[j] = tmp;
  }
  idx = 0;
  for (ui i = 0; i < n; ++i)
    for (ui j = 0; j < n; ++j)
      t->grid[i][j] = saco[idx++];
}

void salva_estado(Tabuleiro *t, Casa jogador_atual, int contador_de_jogadas,
                  Coordenada *coordenada_atual, Casa tipo_casa_anterior) {
  FILE *fp = fopen("temp", "w");
  if (!fp) {
    perror("Erro abrir temp para salvar");
    return;
  }
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j)
      fprintf(fp, "%c", (char)t->grid[i][j]);
    fprintf(fp, "\n");
  }
  fprintf(fp, "%c %d %c %d %d\n", (char)jogador_atual, contador_de_jogadas,
          (char)tipo_casa_anterior, coordenada_atual->linha,
          coordenada_atual->coluna);
  fclose(fp);
}

void carrega_partida(int *contador_de_jogadas, Tabuleiro *t,
                     Coordenada *coordenada_anterior, Casa *tipo_casa_anterior,
                     Casa *jogador_atual) {
  FILE *fp = fopen("temp", "r");
  if (!fp) {
    // não existe: inicializa padrões retornando
    *contador_de_jogadas = 0;
    coordenada_anterior->linha = -1;
    coordenada_anterior->coluna = -1;
    *tipo_casa_anterior = HORIZONTAL;
    *jogador_atual = PRETO;
    return;
  }
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j) {
      int c = fgetc(fp);
      while (c == '\n')
        c = fgetc(fp);
      if (c == EOF) {
        fclose(fp);
        fprintf(stderr, "Save corrompido; iniciando novo jogo.\n");
        *contador_de_jogadas = 0;
        coordenada_anterior->linha = -1;
        coordenada_anterior->coluna = -1;
        *tipo_casa_anterior = HORIZONTAL;
        *jogador_atual = PRETO;
        return;
      }
      t->grid[i][j] = (Casa)c;
    }
  // consumir newline
  int nl = fgetc(fp);
  (void)nl;
  char jc = 0, tc = 0;
  int cont = 0, lr = -1, lc = -1;
  if (fscanf(fp, " %c %d %c %d %d", &jc, &cont, &tc, &lr, &lc) == 5) {
    *jogador_atual = (Casa)jc;
    *contador_de_jogadas = cont;
    *tipo_casa_anterior = (Casa)tc;
    coordenada_anterior->linha = lr;
    coordenada_anterior->coluna = lc;
  } else {
    *contador_de_jogadas = 0;
    coordenada_anterior->linha = -1;
    coordenada_anterior->coluna = -1;
    *tipo_casa_anterior = HORIZONTAL;
    *jogador_atual = PRETO;
  }
  fclose(fp);
}

/* validação para mostrar jogadas possíveis (reaplica sua lógica) */
int validaJogada_simples(Tabuleiro *t, int linha, int coluna, ui n,
                         Coordenada coordenada_anterior,
                         int contador_de_jogadas, Casa tipo_casa_anterior) {
  if (linha < 0 || linha >= (int)n || coluna < 0 || coluna >= (int)n)
    return 0;
  Casa alvo = t->grid[linha][coluna];
  if (alvo == PRETO || alvo == BRANCO)
    return 0;
  if (contador_de_jogadas == 0)
    return 1;
  bool existe = false;
  switch (tipo_casa_anterior) {
  case HORIZONTAL:
    if (coordenada_anterior.linha >= 0) {
      for (int c = 0; c < N; ++c)
        if (t->grid[coordenada_anterior.linha][c] != PRETO &&
            t->grid[coordenada_anterior.linha][c] != BRANCO)
          existe = true;
      if (!existe || linha == coordenada_anterior.linha)
        return 1;
    } else
      return 1;
    break;
  case VERTICAL:
    if (coordenada_anterior.coluna >= 0) {
      for (int l = 0; l < N; ++l)
        if (t->grid[l][coordenada_anterior.coluna] != PRETO &&
            t->grid[l][coordenada_anterior.coluna] != BRANCO)
          existe = true;
      if (!existe || coluna == coordenada_anterior.coluna)
        return 1;
    } else
      return 1;
    break;
  case DIAGONAL_PRINCIPAL:
    if (coordenada_anterior.linha >= 0 && coordenada_anterior.coluna >= 0) {
      for (int i = -N; i < N; ++i) {
        int l = coordenada_anterior.linha + i;
        int c = coordenada_anterior.coluna + i;
        if (l >= 0 && l < N && c >= 0 && c < N && t->grid[l][c] != PRETO &&
            t->grid[l][c] != BRANCO)
          existe = true;
      }
      if (!existe || (linha - coluna ==
                      coordenada_anterior.linha - coordenada_anterior.coluna))
        return 1;
    } else
      return 1;
    break;
  case DIAGONAL_SECUNDARIA:
    if (coordenada_anterior.linha >= 0 && coordenada_anterior.coluna >= 0) {
      for (int i = -N; i < N; ++i) {
        int l = coordenada_anterior.linha + i;
        int c = coordenada_anterior.coluna - i;
        if (l >= 0 && l < N && c >= 0 && c < N && t->grid[l][c] != PRETO &&
            t->grid[l][c] != BRANCO)
          existe = true;
      }
      if (!existe || (linha + coluna ==
                      coordenada_anterior.linha + coordenada_anterior.coluna))
        return 1;
    } else
      return 1;
    break;
  default:
    return 1;
  }
  return 0;
}

void calcula_possiveis(Tabuleiro *t, bool possiveis[N][N],
                       Coordenada coordenada_anterior, Casa tipo_casa_anterior,
                       int contador_de_jogadas) {
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j)
      possiveis[i][j] = false;
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j)
      if (validaJogada_simples(t, i, j, N, coordenada_anterior,
                               contador_de_jogadas, tipo_casa_anterior))
        possiveis[i][j] = true;
}

/* inverter peças (flip, qualquer comprimento) */
void fazInsert(Tabuleiro *t, int linha, int coluna, Casa jogador_atual) {
  Casa oponente = (jogador_atual == PRETO) ? BRANCO : PRETO;
  int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
  int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
  for (int dir = 0; dir < 8; ++dir) {
    int x = linha + dx[dir], y = coluna + dy[dir];
    int flips_x[N], flips_y[N], fcount = 0;
    while (x >= 0 && x < N && y >= 0 && y < N && t->grid[x][y] == oponente) {
      if (fcount < N) {
        flips_x[fcount] = x;
        flips_y[fcount] = y;
      }
      fcount++;
      x += dx[dir];
      y += dy[dir];
    }
    if (fcount > 0 && x >= 0 && x < N && y >= 0 && y < N &&
        t->grid[x][y] == jogador_atual) {
      for (int k = 0; k < fcount; ++k)
        t->grid[flips_x[k]][flips_y[k]] = jogador_atual;
    }
  }
}

/* inserir peça */
void inserir(Tabuleiro *t, int linha, int coluna, Casa peca) {
  if (linha >= 0 && linha < N && coluna >= 0 && coluna < N)
    t->grid[linha][coluna] = peca;
}

/* contagem de regiões para desempate */
int contarRegiao(Tabuleiro *t, int linha, int coluna, Casa jogador,
                 bool visitado[N][N]) {
  if (linha < 0 || linha >= N || coluna < 0 || coluna >= N ||
      visitado[linha][coluna] || t->grid[linha][coluna] != jogador)
    return 0;
  visitado[linha][coluna] = true;
  int cnt = 1;
  cnt += contarRegiao(t, linha + 1, coluna, jogador, visitado);
  cnt += contarRegiao(t, linha - 1, coluna, jogador, visitado);
  cnt += contarRegiao(t, linha, coluna + 1, jogador, visitado);
  cnt += contarRegiao(t, linha, coluna - 1, jogador, visitado);
  return cnt;
}
int contarMaiorArea(Tabuleiro *t, Casa jogador) {
  int maior = 0;
  bool visitado[N][N] = {false};
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j)
      if (!visitado[i][j] && t->grid[i][j] == jogador) {
        int area = contarRegiao(t, i, j, jogador, visitado);
        if (area > maior)
          maior = area;
      }
  return maior;
}

/* verifica vitória (se alguém fez 5 em linha) */
int verificaVitoria(Tabuleiro *t, Casa jogador_que_jogou,
                    int contador_de_jogadas) {
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j)
      if (t->grid[i][j] == jogador_que_jogou) {
        if (j + 4 < N && t->grid[i][j + 1] == jogador_que_jogou &&
            t->grid[i][j + 2] == jogador_que_jogou &&
            t->grid[i][j + 3] == jogador_que_jogou &&
            t->grid[i][j + 4] == jogador_que_jogou)
          return (jogador_que_jogou == PRETO) ? 1 : 2;
        if (i + 4 < N && t->grid[i + 1][j] == jogador_que_jogou &&
            t->grid[i + 2][j] == jogador_que_jogou &&
            t->grid[i + 3][j] == jogador_que_jogou &&
            t->grid[i + 4][j] == jogador_que_jogou)
          return (jogador_que_jogou == PRETO) ? 1 : 2;
        if (i + 4 < N && j + 4 < N &&
            t->grid[i + 1][j + 1] == jogador_que_jogou &&
            t->grid[i + 2][j + 2] == jogador_que_jogou &&
            t->grid[i + 3][j + 3] == jogador_que_jogou &&
            t->grid[i + 4][j + 4] == jogador_que_jogou)
          return (jogador_que_jogou == PRETO) ? 1 : 2;
        if (i + 4 < N && j - 4 >= 0 &&
            t->grid[i + 1][j - 1] == jogador_que_jogou &&
            t->grid[i + 2][j - 2] == jogador_que_jogou &&
            t->grid[i + 3][j - 3] == jogador_que_jogou &&
            t->grid[i + 4][j - 4] == jogador_que_jogou)
          return (jogador_que_jogou == PRETO) ? 1 : 2;
      }
  if (contador_de_jogadas >= TOTAL_JOGADAS) {
    int ap = contarMaiorArea(t, PRETO);
    int ab = contarMaiorArea(t, BRANCO);
    if (ap > ab)
      return 1;
    if (ab > ap)
      return 2;
    return 3;
  }
  return 0;
}

/* --- impressão 3D-ish --- */
void imprimir_3d(const Tabuleiro *t, int cursor_l, int cursor_c,
                 bool possiveis[N][N], Coordenada coordenada_anterior,
                 int contador_de_jogadas, Casa jogador_atual) {
  // limpa tela
  printf("\x1b[2J\x1b[H");

  // topo com informações
  printf(BOLD FG_CYAN "  Jogo 3D (terminal) — Jogador: ");
  if (jogador_atual == PRETO)
    printf(FG_RED "X (PRETO)");
  else
    printf(FG_YELLOW "O (BRANCO)");
  printf(RESET "   Jogadas: %d\n", contador_de_jogadas);
  printf("  Use WASD ou setas para mover, Enter/Espaço para jogar, x salvar, q "
         "salvar+sair\n\n");

  // Render pseudo-3D: para cada linha imprimimos duas "stripes" (alto e baixo)
  // Também aplicamos deslocamento horizontal por linha para criar efeito de
  // profundidade.
  for (int i = 0; i < N; ++i) {
    // deslocamento inicial (quanto mais para cima na tela, mais indent)
    int shift = (N - 1 - i) * 2;
    for (int s = 0; s < shift; ++s)
      putchar(' ');

    // primeira stripe (top shading)
    for (int j = 0; j < N; ++j) {
      bool cur = (i == cursor_l && j == cursor_c);
      bool poss = possiveis[i][j];
      Casa c = t->grid[i][j];
      if (c == PRETO) {
        if (cur)
          printf(INV BG_RED "  X " RESET);
        else if (poss)
          printf(BG_GREEN FG_BLACK "  X " RESET);
        else
          printf(BG_RED "  X " RESET);
      } else if (c == BRANCO) {
        if (cur)
          printf(INV BG_YELLOW "  O " RESET);
        else if (poss)
          printf(BG_GREEN FG_BLACK "  O " RESET);
        else
          printf(BG_YELLOW "  O " RESET);
      } else {
        // casas "tipo" — usamos cores diferentes por tipo para dar textura
        const char *bg = BG_CYAN;
        if (c == VERTICAL)
          bg = BG_BLUE;
        if (c == HORIZONTAL)
          bg = BG_WHITE;
        if (c == DIAGONAL_PRINCIPAL)
          bg = BG_MAGENTA;
        if (c == DIAGONAL_SECUNDARIA)
          bg = BG_CYAN;
        if (cur)
          printf(INV "%s  %c " RESET, bg, (char)c);
        else if (poss)
          printf(BG_GREEN "  · " RESET);
        else
          printf("%s  %c " RESET, bg, (char)c);
      }
      // spacing between tiles (creates "depth")
      printf(" ");
    }
    printf("\n");

    // segunda stripe (bottom shading) - slightly darker to simulate depth
    for (int s = 0; s < shift; ++s)
      putchar(' ');
    for (int j = 0; j < N; ++j) {
      bool cur = (i == cursor_l && j == cursor_c);
      bool poss = possiveis[i][j];
      Casa c = t->grid[i][j];
      if (c == PRETO) {
        if (cur)
          printf(INV BG_RED " /_\\ " RESET);
        else if (poss)
          printf(BG_GREEN FG_BLACK " /_\\ " RESET);
        else
          printf(BG_RED " /_\\ " RESET);
      } else if (c == BRANCO) {
        if (cur)
          printf(INV BG_YELLOW " /_\\ " RESET);
        else if (poss)
          printf(BG_GREEN FG_BLACK " /_\\ " RESET);
        else
          printf(BG_YELLOW " /_\\ " RESET);
      } else {
        const char *bg = BG_CYAN;
        if (c == VERTICAL)
          bg = BG_BLUE;
        if (c == HORIZONTAL)
          bg = BG_WHITE;
        if (c == DIAGONAL_PRINCIPAL)
          bg = BG_MAGENTA;
        if (c == DIAGONAL_SECUNDARIA)
          bg = BG_CYAN;
        if (cur)
          printf(INV "%s /_%c " RESET, bg, (char)c);
        else if (poss)
          printf(BG_GREEN " /_\\ " RESET);
        else
          printf("%s /_%c " RESET, bg, (char)c);
      }
      printf(" ");
    }
    printf("\n");
  }

  // fim: legenda e última jogada
  printf("\nLegenda: " FG_RED "X=PRETO " RESET FG_YELLOW "O=BRANCO " RESET);
  printf("· possível jogada (verde)\n");
  if (coordenada_anterior.linha >= 0)
    printf("Última jogada: linha %d coluna %d\n", coordenada_anterior.linha,
           coordenada_anterior.coluna);
}

/* --- main loop --- */
int main(void) {
  srand((unsigned)time(NULL));
  Tabuleiro tab;
  Coordenada coordenada_anterior;
  Casa tipo_casa_anterior;
  Casa jogador_atual;
  int contador_de_jogadas;
  int status_jogo = 0;

  printf("\x1b[2J\x1b[H");
  printf("1 - Iniciar novo jogo\n2 - Carregar jogo salvo (temp)\n3 - "
         "Sair\n\nEscolha: ");
  int escolha = 0;
  if (scanf("%d", &escolha) != 1)
    return 0;
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF)
    ; // limpar buffer

  if (escolha == 1) {
    inicia_tab(&tab, N);
    contador_de_jogadas = 0;
    coordenada_anterior.linha = -1;
    coordenada_anterior.coluna = -1;
    tipo_casa_anterior = HORIZONTAL;
    jogador_atual = PRETO;
  } else if (escolha == 2) {
    carrega_partida(&contador_de_jogadas, &tab, &coordenada_anterior,
                    &tipo_casa_anterior, &jogador_atual);
  } else {
    return 0;
  }

  bool possiveis[N][N];
  calcula_possiveis(&tab, possiveis, coordenada_anterior, tipo_casa_anterior,
                    contador_de_jogadas);

  int cursor_l = 0, cursor_c = 0;

  enableRawMode();

  while (status_jogo == 0) {
    imprimir_3d(&tab, cursor_l, cursor_c, possiveis, coordenada_anterior,
                contador_de_jogadas, jogador_atual);

    // ler tecla (suporta setas: ESC [ A/B/C/D)
    char buf[4] = {0};
    ssize_t n = read(STDIN_FILENO, buf, 3);
    if (n <= 0)
      continue;

    if (buf[0] == 27 && n >= 2 && buf[1] == '[') {
      // seta
      char code = buf[2];
      if (code == 'A') { // up
        if (cursor_l > 0)
          cursor_l--;
      } else if (code == 'B') { // down
        if (cursor_l < N - 1)
          cursor_l++;
      } else if (code == 'C') { // right
        if (cursor_c < N - 1)
          cursor_c++;
      } else if (code == 'D') { // left
        if (cursor_c > 0)
          cursor_c--;
      }
    } else {
      char c = buf[0];
      c = (char)tolower((unsigned char)c);
      if (c == 'w') {
        if (cursor_l > 0)
          cursor_l--;
      } else if (c == 's') {
        if (cursor_l < N - 1)
          cursor_l++;
      } else if (c == 'a') {
        if (cursor_c > 0)
          cursor_c--;
      } else if (c == 'd') {
        if (cursor_c < N - 1)
          cursor_c++;
      } else if (c == '\r' || c == '\n' || c == ' ') {
        // confirmar jogada
        if (possiveis[cursor_l][cursor_c]) {
          tipo_casa_anterior = tab.grid[cursor_l][cursor_c];
          inserir(&tab, cursor_l, cursor_c, jogador_atual);
          fazInsert(&tab, cursor_l, cursor_c, jogador_atual);
          coordenada_anterior.linha = cursor_l;
          coordenada_anterior.coluna = cursor_c;
          contador_de_jogadas++;
          status_jogo =
              verificaVitoria(&tab, jogador_atual, contador_de_jogadas);
          if (status_jogo == 0)
            jogador_atual = (jogador_atual == PRETO) ? BRANCO : PRETO;
          calcula_possiveis(&tab, possiveis, coordenada_anterior,
                            tipo_casa_anterior, contador_de_jogadas);
        } else {
          // beep como feedback
          write(STDOUT_FILENO, "\a", 1);
        }
      } else if (c == 'x') {
        disableRawMode();
        salva_estado(&tab, jogador_atual, contador_de_jogadas,
                     &coordenada_anterior, tipo_casa_anterior);
        enableRawMode();
      } else if (c == 'q') {
        disableRawMode();
        salva_estado(&tab, jogador_atual, contador_de_jogadas,
                     &coordenada_anterior, tipo_casa_anterior);
        printf("Salvo em 'temp'. Saindo.\n");
        return 0;
      } else {
        // ignoradas
      }
    }
  } // fim while

  disableRawMode();

  // final
  printf("\n--- FIM DE JOGO ---\n");
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j)
      putchar((char)tab.grid[i][j]);
    putchar('\n');
  }
  if (status_jogo == 1)
    printf("\nPRETO (X) venceu!\n");
  else if (status_jogo == 2)
    printf("\nBRANCO (O) venceu!\n");
  else
    printf("\nEmpate!\n");

  return 0;
}
