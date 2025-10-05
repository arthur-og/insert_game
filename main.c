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

typedef struct {
  ui linha;
  ui coluna;
} Coordenada;

int contarRegiao(Tabuleiro *t, int linha, int coluna, Casa jogador,
                 bool visitado[N][N]);
int contarMaiorArea(Tabuleiro *t, Casa jogador);
void inicia_tab(Tabuleiro *t, ui n);
void print_tab(const Tabuleiro *t, ui n);
int validaJogada(Tabuleiro *t, ui linha, ui coluna, ui n,
                 Coordenada coordenada_anterior, int contador_de_jogadas);
void inserir(Tabuleiro *t, ui linha, ui coluna, Casa peca);
void fazInsert(Tabuleiro *t, ui linha, ui coluna, Casa jogador_atual);
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

  // (Opcional) escrever dimensão
  // fprintf(fp, "%d %d\n", N, N);

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      // supondo que seu tabuleiro guarda em t->mat[i][j]
      fprintf(fp, "%c", t->grid[i][j]);
      /*if (j < N - 1) {
        fputc(' ', fp);
      }*/
    }
    fputc('\n', fp);
  }
  fprintf(fp, "%c %d %c %d %d", jogador_atual, contador_de_jogadas,
          tipo_casa_anterior, coordenada_atual->linha,
          coordenada_atual->coluna);

  if (fclose(fp) != 0) {
    perror("Erro ao fechar arquivo após salvar estado");
  }
}

void carrega_partida(int *contador_de_jogadas, Tabuleiro *tab,
                     Coordenada *coordenada_anterior, Casa *tipo_casa_anterior,
                     Casa *jogador_atual);

void inicia_tab(Tabuleiro *t, ui n) {
  // Criando um "saco" com o total de peças
  ui total_pecas = n * n;
  Casa saco_de_pecas[total_pecas];

  // vetor temporário armazenando as peças que vamos utilizar
  Casa pecas[] = {VERTICAL, HORIZONTAL, DIAGONAL_PRINCIPAL,
                  DIAGONAL_SECUNDARIA};
  ui pecas_por_tipo = total_pecas / 4; // Deve dar 9 para cada tipo [cite: 44]

  // Encher o saco temporário com 9 peças de cada
  ui contador = 0;
  for (ui i = 0; i < 4; i++) {
    for (ui j = 0; j < pecas_por_tipo; j++) {
      saco_de_pecas[contador] = pecas[i];
      contador++;
    }
  }

  // Embaralharando o saco temporário
  for (ui i = total_pecas - 1; i > 0; i--) {
    ui j = rand() % (i + 1);
    Casa temp = saco_de_pecas[i];
    saco_de_pecas[i] = saco_de_pecas[j];
    saco_de_pecas[j] = temp;
  }

  // Distribuindo as peças embaralhadas no tabuleiro
  contador = 0;
  for (ui i = 0; i < n; i++) {
    for (ui j = 0; j < n; j++) {
      t->grid[i][j] = saco_de_pecas[contador];
      contador++;
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

void inserir(Tabuleiro *t, ui linha, ui coluna, Casa peca) {
  t->grid[linha][coluna] = peca;
}

char *nomeJogador(Casa jogador_atual) {
  if (jogador_atual == PRETO)
    return "da rosca preta (X)";
  else
    return "da rosca branca (O)";
}

int validaJogada(Tabuleiro *t, ui linha, ui coluna, ui n,
                 Coordenada coordenada_anterior, int contador_de_jogadas) {
  if (linha >= n || coluna >= n)
    return 0;

  Casa casa_alvo_atual = t->grid[linha][coluna];
  if (casa_alvo_atual == BRANCO || casa_alvo_atual == PRETO)
    return 0;

  if (contador_de_jogadas == 0)
    return 1;

  ui linha_ant = coordenada_anterior.linha;
  ui coluna_ant = coordenada_anterior.coluna;
  return 1;
}

void fazInsert(Tabuleiro *t, ui linha, ui coluna, Casa jogador_atual) {
  Casa oponente = (jogador_atual == PRETO) ? BRANCO : PRETO;

  // Vetores para checar as 8 direções (horizontal, vertical, diagonais)
  int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
  int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

  for (int i = 0; i < 8; i++) {
    // Coordenada do vizinho na direção (dx[i], dy[i])
    int vizinho_l = linha + dx[i];
    int vizinho_c = coluna + dy[i];

    // Coordenada da peça que "fecha" o sanduíche
    int final_l = linha + 2 * dx[i];
    int final_c = coluna + 2 * dy[i];

    // Verifica se as coordenadas estão dentro do tabuleiro
    if (vizinho_l >= 0 && vizinho_l < N && vizinho_c >= 0 && vizinho_c < N &&
        final_l >= 0 && final_l < N && final_c >= 0 && final_c < N) {
      // Verifica se o padrão JOGADOR - OPONENTE - JOGADOR existe
      if (t->grid[vizinho_l][vizinho_c] == oponente &&
          t->grid[final_l][final_c] == jogador_atual) {
        printf("\nHouve Insert! \n");
        t->grid[vizinho_l][vizinho_c] =
            jogador_atual; // Vira a peça do oponente
      }
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
  printf("\n\t\t\t\t2 - Carregar jogo\n\n");
  printf("\n\t\t\t\t3 - Sair\n\n");
  ui escolha = 0;
  scanf("%d", &escolha);
  printf("\x1B[2J\x1B[H");

  srand((unsigned)time(NULL));
  Tabuleiro tab;
  Casa jogador_atual;
  int status_jogo = 0; // 0=continua, 1=preto vence, 2=branco vence, 3=empate
  ui linha, coluna;
  Coordenada coordenada_anterior;
  Casa tipo_casa_anterior; // Valor inicial qualquer
  int contador_de_jogadas;

  if (escolha == 1) {
    contador_de_jogadas = 0;
    inicia_tab(&tab, N);
    coordenada_anterior.linha = -1, coordenada_anterior.coluna = -1;
    tipo_casa_anterior = HORIZONTAL;
    jogador_atual = PRETO;
  } /* else if (escolha == 2) {
     carrega_partida(&contador_de_jogadas, &tab, &coordenada_anterior,
                     &tipo_casa_anterior, &jogador_atual);
   }*/

  while (status_jogo == 0) {
    print_tab(&tab, N);
    printf("\nVez do jogador %s\n", nomeJogador(jogador_atual));

    while (true) {
      printf("Linha: ");
      scanf("%u", &linha);

      printf("Coluna: ");
      scanf("%u", &coluna);

      // VALIDAÇÃO COMPLETA DA JOGADA
      bool jogada_valida = false;
      if (linha < N && coluna < N && tab.grid[linha][coluna] != PRETO &&
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
        printf("\nUltima jogada foi na linha %d e na coluna %d",
               coordenada_anterior.linha, coordenada_anterior.coluna);
        printf("\n");
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
