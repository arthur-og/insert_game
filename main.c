#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 6
#define TOTAL_JOGADAS 30 // Limite de 30 turnos
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

/* Coordenada agora usa int para permitir valores sentinela (-1) */
typedef struct {
  int linha;
  int coluna;
} Coordenada;

/* protótipos */
int contarRegiao(Tabuleiro *t, int linha, int coluna, Casa jogador,
                 bool visitado[N][N]);
int contarMaiorArea(Tabuleiro *t, Casa jogador);
void inicia_tab(Tabuleiro *t, ui n);
void print_tab(const Tabuleiro *t, ui n);
int validaJogada(Tabuleiro *t, int linha, int coluna, ui n,
                 Coordenada coordenada_anterior, int contador_de_jogadas);
void inserir(Tabuleiro *t, int linha, int coluna, Casa peca);
void fazInsert(Tabuleiro *t, int linha, int coluna, Casa jogador_atual);
int verificaVitoria(Tabuleiro *t, Casa jogador_que_jogou,
                    int contador_de_jogadas);
char *nomeJogador(Casa jogador_atual);

void salva_estado(Tabuleiro *t, Casa jogador_atual, int contador_de_jogadas,
                  Coordenada *coordenada_atual, Casa tipo_casa_anterior);
void salva_partida();
void carrega_partida(int *contador_de_jogadas, Tabuleiro *tab,
                     Coordenada *coordenada_anterior, Casa *tipo_casa_anterior,
                     Casa *jogador_atual);

void salva_estado(Tabuleiro *t, Casa jogador_atual, int contador_de_jogadas,
                  Coordenada *coordenada_atual, Casa tipo_casa_anterior) {
  FILE *fp = fopen("temp", "w");
  if (fp == NULL) {
    perror("Erro ao abrir arquivo para salvar estado");
    return;
  }

  // escreve o tabuleiro (sem espaços, com quebras de linha)
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      fprintf(fp, "%c", (char)t->grid[i][j]);
    }
    fputc('\n', fp);
  }

  // escreve metadados: jogador_atual, contador_de_jogadas, tipo_casa_anterior,
  // linha, coluna Ex: X 12 - 2 3
  fprintf(fp, "%c %d %c %d %d\n", (char)jogador_atual, contador_de_jogadas,
          (char)tipo_casa_anterior, coordenada_atual->linha,
          coordenada_atual->coluna);

  if (fclose(fp) != 0) {
    perror("Erro ao fechar arquivo após salvar estado");
  } else {
    printf("Partida salva em 'temp'.\n");
  }
}

void carrega_partida(int *contador_de_jogadas, Tabuleiro *t,
                     Coordenada *coordenada_anterior, Casa *tipo_casa_anterior,
                     Casa *jogador_atual) {
  FILE *fp = fopen("temp", "r");
  if (fp == NULL) {
    printf("Arquivo de save 'temp' não encontrado. Iniciando jogo novo.\n");
    return;
  }

  // Ler o grid: ler caractere por caractere em uma variável temporária
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      char c;
      if (fscanf(fp, " %c", &c) != 1) {
        printf("Arquivo de save corrompido ao ler o tabuleiro.\n");
        fclose(fp);
        return;
      }
      t->grid[i][j] = (Casa)c;
    }
    // após cada linha, pode haver newline; fscanf com " %c" já pula whitespace
  }

  // Ler metadados finais (se existirem)
  char jogador_char = 0, tipo_char = 0;
  int cont = 0, linha_ant = -1, col_ant = -1;
  int read = fscanf(fp, " %c %d %c %d %d", &jogador_char, &cont, &tipo_char,
                    &linha_ant, &col_ant);
  if (read == 5) {
    *jogador_atual = (Casa)jogador_char;
    *contador_de_jogadas = cont;
    *tipo_casa_anterior = (Casa)tipo_char;
    coordenada_anterior->linha = linha_ant;
    coordenada_anterior->coluna = col_ant;
    printf("Partida carregada: jogador atual '%c', jogadas feitas: %d\n",
           jogador_char, cont);
    printf("\nCasa da rodada: %c\n", *tipo_casa_anterior);
    printf("\nUltima jogada foi na linha %d e na coluna %d\n",
           coordenada_anterior->linha, coordenada_anterior->coluna);

  } else {
    // Se não houver metadados, inicializa valores seguros
    *contador_de_jogadas = 0;
    coordenada_anterior->linha = -1;
    coordenada_anterior->coluna = -1;
    *tipo_casa_anterior = HORIZONTAL;
    *jogador_atual = PRETO;
    printf("Save sem metadados. Valores padrão aplicados.\n");
  }

  if (fclose(fp) != 0) {
    perror("Erro ao fechar arquivo após carregar estado");
  }
}

void inicia_tab(Tabuleiro *t, ui n) {
  // Criando um "saco" com o total de peças
  ui total_pecas = n * n;
  Casa saco_de_pecas[total_pecas];

  // vetor temporário armazenando as peças que vamos utilizar
  Casa pecas[] = {VERTICAL, HORIZONTAL, DIAGONAL_PRINCIPAL,
                  DIAGONAL_SECUNDARIA};
  ui pecas_por_tipo = total_pecas / 4; // padrão

  // Encher o saco temporário
  ui contador = 0;
  for (ui i = 0; i < 4; i++) {
    for (ui j = 0; j < pecas_por_tipo; j++) {
      saco_de_pecas[contador] = pecas[i];
      contador++;
    }
  }

  // Embaralhar
  for (ui i = total_pecas - 1; i > 0; i--) {
    ui j = rand() % (i + 1);
    Casa temp = saco_de_pecas[i];
    saco_de_pecas[i] = saco_de_pecas[j];
    saco_de_pecas[j] = temp;
  }

  // Preencher tabuleiro
  contador = 0;
  for (ui i = 0; i < n; i++) {
    for (ui j = 0; j < n; j++) {
      t->grid[i][j] = saco_de_pecas[contador++];
    }
  }
}

void print_tab(const Tabuleiro *t, ui n) {
  ui width = 3.9 * n;
  printf("   ");
  for (ui j = 0; j < n; ++j)
    printf(" %u  ", j);
  printf("\n  ┏");
  for (ui j = 0; j < width; ++j)
    if ((j + 1) % 4 == 0)
      printf("┳");
    else
      printf("━");
  printf("┓\n");

  for (ui i = 0; i < n; ++i) {
    printf("%u ┃", i);
    for (ui j = 0; j < n; ++j) {
      printf(" %c ", (char)t->grid[i][j]);
      if (j < n - 1)
        printf("┃");
    }
    printf("┃\n");
    if (i < n - 1) {
      printf("  ┣");
      for (ui j = 0; j < width; ++j)
        if ((j + 1) % 4 == 0)
          printf("╋");
        else
          printf("━");
      printf("┫\n");
    }
  }

  printf("  ┗");
  for (ui j = 0; j < width; ++j)
    if ((j + 1) % 4 == 0)
      printf("┻");
    else
      printf("━");
  printf("┛\n");
}

void inserir(Tabuleiro *t, int linha, int coluna, Casa peca) {
  if (linha >= 0 && linha < N && coluna >= 0 && coluna < N)
    t->grid[linha][coluna] = peca;
}

char *nomeJogador(Casa jogador_atual) {
  if (jogador_atual == PRETO)
    return "da rosca preta (X)";
  else
    return "da rosca branca (O)";
}

/* validaJogada (mantive simples, apenas demonstração — você já tem
   validações adicionais no loop principal) */
int validaJogada(Tabuleiro *t, int linha, int coluna, ui n,
                 Coordenada coordenada_anterior, int contador_de_jogadas) {
  if (linha < 0 || linha >= (int)n || coluna < 0 || coluna >= (int)n)
    return 0;

  Casa casa_alvo_atual = t->grid[linha][coluna];
  if (casa_alvo_atual == BRANCO || casa_alvo_atual == PRETO)
    return 0;

  if (contador_de_jogadas == 0)
    return 1;

  return 1;
}

/* fazInsert: vira todas as peças entre a nova peça e uma peça do mesmo jogador
 */
void fazInsert(Tabuleiro *t, int linha, int coluna, Casa jogador_atual) {
  Casa oponente = (jogador_atual == PRETO) ? BRANCO : PRETO;

  // Direções (8 vizinhos): horizontais, verticais e diagonais
  int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
  int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

  // Percorre cada direção
  for (int dir = 0; dir < 8; ++dir) {
    int ant_x = linha - dx[dir]; // célula anterior (antes da jogada)
    int ant_y = coluna - dy[dir];
    int prox_x = linha + dx[dir]; // célula posterior (depois da jogada)
    int prox_y = coluna + dy[dir];

    // Verifica se ambas as posições estão dentro do tabuleiro
    if (ant_x >= 0 && ant_x < N && ant_y >= 0 && ant_y < N && prox_x >= 0 &&
        prox_x < N && prox_y >= 0 && prox_y < N) {

      // Caso 1: jogador colocou NO MEIO → padrão X O X (vira)
      if (t->grid[ant_x][ant_y] == oponente &&
          t->grid[prox_x][prox_y] == oponente) {
        t->grid[ant_x][ant_y] = jogador_atual;
        t->grid[prox_x][prox_y] = jogador_atual;
      }

      // Caso 2: jogador colocou na ponta → X O X (onde jogador é X) → não vira
      // Nada é feito
    }
  }
}

/**
 * Verifica se a partida terminou.
 * Retorna 1 se PRETO venceu, 2 se BRANCO venceu, 3 para empate, 0 se o jogo
 * continua.
 */
int verificaVitoria(Tabuleiro *t, Casa jogador_que_jogou,
                    int contador_de_jogadas) {
  // 1. Checar por 5 em linha para o jogador que acabou de jogar
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (t->grid[i][j] == jogador_que_jogou) {
        // Checa Horizontal
        if (j + 4 < N && t->grid[i][j + 1] == jogador_que_jogou &&
            t->grid[i][j + 2] == jogador_que_jogou &&
            t->grid[i][j + 3] == jogador_que_jogou &&
            t->grid[i][j + 4] == jogador_que_jogou)
          return (jogador_que_jogou == PRETO) ? 1 : 2;

        // Checa Vertical
        if (i + 4 < N && t->grid[i + 1][j] == jogador_que_jogou &&
            t->grid[i + 2][j] == jogador_que_jogou &&
            t->grid[i + 3][j] == jogador_que_jogou &&
            t->grid[i + 4][j] == jogador_que_jogou)
          return (jogador_que_jogou == PRETO) ? 1 : 2;

        // Checa Diagonal Principal (\)
        if (i + 4 < N && j + 4 < N &&
            t->grid[i + 1][j + 1] == jogador_que_jogou &&
            t->grid[i + 2][j + 2] == jogador_que_jogou &&
            t->grid[i + 3][j + 3] == jogador_que_jogou &&
            t->grid[i + 4][j + 4] == jogador_que_jogou)
          return (jogador_que_jogou == PRETO) ? 1 : 2;

        // Checa Diagonal Secundária (/)
        if (i + 4 < N && j - 4 >= 0 &&
            t->grid[i + 1][j - 1] == jogador_que_jogou &&
            t->grid[i + 2][j - 2] == jogador_que_jogou &&
            t->grid[i + 3][j - 3] == jogador_que_jogou &&
            t->grid[i + 4][j - 4] == jogador_que_jogou)
          return (jogador_que_jogou == PRETO) ? 1 : 2;
      }
    }
  }

  // 2. Checar por fim de jogo por número de jogadas
  if (contador_de_jogadas >= TOTAL_JOGADAS) {
    int area_preto = contarMaiorArea(t, PRETO);
    int area_branco = contarMaiorArea(t, BRANCO);

    printf("\nFim de jogo por limite de turnos!");
    printf("\nMaior região do jogador PRETO (X): %d", area_preto);
    printf("\nMaior região do jogador BRANCO (O): %d", area_branco);

    if (area_preto > area_branco)
      return 1; // Preto vence
    if (area_branco > area_preto)
      return 2; // Branco vence
    return 3;   // Empate
  }

  return 0; // Jogo continua
}

int contarMaiorArea(Tabuleiro *t, Casa jogador) {
  int maior_area = 0;
  bool visitado[N][N] = {false};

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (t->grid[i][j] == jogador && !visitado[i][j]) {
        int area_atual = contarRegiao(t, i, j, jogador, visitado);
        if (area_atual > maior_area) {
          maior_area = area_atual;
        }
      }
    }
  }
  return maior_area;
}

// Usa busca em profundidade (DFS) para contar peças conectadas ortogonalmente
int contarRegiao(Tabuleiro *t, int linha, int coluna, Casa jogador,
                 bool visitado[N][N]) {
  if (linha < 0 || linha >= N || coluna < 0 || coluna >= N ||
      visitado[linha][coluna] || t->grid[linha][coluna] != jogador) {
    return 0;
  }

  visitado[linha][coluna] = true;
  int contagem = 1;

  contagem += contarRegiao(t, linha + 1, coluna, jogador, visitado); // Baixo
  contagem += contarRegiao(t, linha - 1, coluna, jogador, visitado); // Cima
  contagem += contarRegiao(t, linha, coluna + 1, jogador, visitado); // Direita
  contagem += contarRegiao(t, linha, coluna - 1, jogador, visitado); // Esquerda

  return contagem;
}

int main(void) {
  printf("\x1B[2J\x1B[H");

  printf("\n\n\n\n\n\n\n\n\t\t\t\t1 - Iniciar novo jogo\n\n");
  printf("\n\t\t\t\t2 - Carregar jogo salvo (arquivo 'temp')\n\n");
  printf("\n\t\t\t\t3 - Sair\n\n");
  ui escolha = 0;
  if (scanf("%u", &escolha) != 1) {
    printf("Entrada inválida. Saindo.\n");
    return 1;
  }
  printf("\x1B[2J\x1B[H");

  srand((unsigned)time(NULL));
  Tabuleiro tab;
  Casa jogador_atual;
  int status_jogo = 0; // 0=continua, 1=preto vence, 2=branco vence, 3=empate
  int linha, coluna;
  Coordenada coordenada_anterior;
  Casa tipo_casa_anterior; // Valor inicial qualquer
  int contador_de_jogadas;

  if (escolha == 1) {
    contador_de_jogadas = 0;
    inicia_tab(&tab, N);
    coordenada_anterior.linha = -1;
    coordenada_anterior.coluna = -1;
    tipo_casa_anterior = HORIZONTAL;
    jogador_atual = PRETO;
  } else if (escolha == 2) {
    carrega_partida(&contador_de_jogadas, &tab, &coordenada_anterior,
                    &tipo_casa_anterior, &jogador_atual);
    // se arquivo não existia, carrega_partida já colocou valores padrões
  } else {
    return 0;
  }

  while (status_jogo == 0) {
    print_tab(&tab, N);
    printf("\nVez do jogador %s\n", nomeJogador(jogador_atual));
    printf("(Digite -1 em 'Linha' para salvar a partida e sair)\n");

    while (true) {
      printf("Linha: ");
      if (scanf("%d", &linha) != 1) {
        printf("Entrada inválida. Tente novamente.\n");
        // limpar buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
          ;
        continue;
      }

      if (linha == -1) {
        // salvar e sair
        salva_estado(&tab, jogador_atual, contador_de_jogadas,
                     &coordenada_anterior, tipo_casa_anterior);
        printf("Saindo do jogo (salvo).\n");
        return 0;
      }

      printf("Coluna: ");
      if (scanf("%d", &coluna) != 1) {
        printf("Entrada inválida. Tente novamente.\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
          ;
        continue;
      }

      // VALIDAÇÃO COMPLETA DA JOGADA
      bool jogada_valida = false;
      if (linha >= 0 && linha < N && coluna >= 0 && coluna < N &&
          tab.grid[linha][coluna] != PRETO &&
          tab.grid[linha][coluna] != BRANCO) {
        if (contador_de_jogadas == 0) {
          jogada_valida = true;
        } else {
          // Lógica de "FREEDOM!"
          bool existe_jogada_obrigatoria = false;
          switch (tipo_casa_anterior) {
          case HORIZONTAL:
            for (int c = 0; c < N; c++)
              if (tab.grid[coordenada_anterior.linha][c] != PRETO &&
                  tab.grid[coordenada_anterior.linha][c] != BRANCO)
                existe_jogada_obrigatoria = true;
            if (!existe_jogada_obrigatoria ||
                linha == coordenada_anterior.linha)
              jogada_valida = true;
            break;
          case VERTICAL:
            for (int l = 0; l < N; l++)
              if (tab.grid[l][coordenada_anterior.coluna] != PRETO &&
                  tab.grid[l][coordenada_anterior.coluna] != BRANCO)
                existe_jogada_obrigatoria = true;
            if (!existe_jogada_obrigatoria ||
                coluna == coordenada_anterior.coluna)
              jogada_valida = true;
            break;
          case DIAGONAL_PRINCIPAL:
            for (int i = -N; i < N; i++) {
              int l = coordenada_anterior.linha + i;
              int c = coordenada_anterior.coluna + i;
              if (l >= 0 && l < N && c >= 0 && c < N &&
                  tab.grid[l][c] != PRETO && tab.grid[l][c] != BRANCO)
                existe_jogada_obrigatoria = true;
            }
            if (!existe_jogada_obrigatoria ||
                ((int)linha - (int)coluna ==
                 (int)coordenada_anterior.linha -
                     (int)coordenada_anterior.coluna))
              jogada_valida = true;
            break;
          case DIAGONAL_SECUNDARIA:
            for (int i = -N; i < N; i++) {
              int l = coordenada_anterior.linha + i;
              int c = coordenada_anterior.coluna - i;
              if (l >= 0 && l < N && c >= 0 && c < N &&
                  tab.grid[l][c] != PRETO && tab.grid[l][c] != BRANCO)
                existe_jogada_obrigatoria = true;
            }
            if (!existe_jogada_obrigatoria ||
                (linha + coluna ==
                 coordenada_anterior.linha + coordenada_anterior.coluna))
              jogada_valida = true;
            break;
          default:
            jogada_valida = true;
            break;
          }
        }
      }

      if (jogada_valida) {
        tipo_casa_anterior =
            tab.grid[linha][coluna]; // Guarda o tipo ANTES de inserir a peça

        inserir(&tab, linha, coluna, jogador_atual);
        fazInsert(&tab, linha, coluna, jogador_atual);

        coordenada_anterior.linha = linha;
        coordenada_anterior.coluna = coluna;
        contador_de_jogadas++;

        status_jogo = verificaVitoria(&tab, jogador_atual, contador_de_jogadas);

        printf("\x1B[2J\x1B[H");

        printf("\nCasa da rodada: %c\n", tipo_casa_anterior);
        printf("\nUltima jogada foi na linha %d e na coluna %d\n",
               coordenada_anterior.linha, coordenada_anterior.coluna);
        break;
      } else {
        printf("\nJogada inválida! Tente novamente.\n");
      }
    }

    // Só troca de jogador se o jogo não acabou
    if (status_jogo == 0) {
      if (jogador_atual == PRETO)
        jogador_atual = BRANCO;
      else
        jogador_atual = PRETO;
    }

    // opcional: salvar estado a cada jogada automática
    salva_estado(&tab, jogador_atual, contador_de_jogadas, &coordenada_anterior,
                 tipo_casa_anterior);
  }

  // Exibe o tabuleiro final e o resultado
  printf("\n--- FIM DE JOGO ---\n");
  print_tab(&tab, N);
  if (status_jogo == 1)
    printf("\nO JOGADOR PRETO (X) VENCEU!\n");
  else if (status_jogo == 2)
    printf("\nO JOGADOR BRANCO (O) VENCEU!\n");
  else if (status_jogo == 3)
    printf("\nO JOGO TERMINOU EM EMPATE!\n");

  return 0;
}
