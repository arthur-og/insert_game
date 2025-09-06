#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 6
#define MAXIMUM_PLAYS (N * N) / 2

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

void inicia_tab(Tabuleiro *t, size_t n) {
  // Passo 1: Criar um "saco" com o total de peças
  size_t total_pecas = n * n;
  Casa saco_de_pecas[total_pecas];

  // vetor temporário armazenando as peças que vamos utilizar
  Casa pecas[] = {VERTICAL, HORIZONTAL, DIAGONAL_PRINCIPAL,
                  DIAGONAL_SECUNDARIA};
  size_t pecas_por_tipo = total_pecas / 4;

  // Passo 2: Encher o saco temporário com 9 peças de cada
  size_t contador = 0;
  for (int i = 0; i < 4; i++) { // Para cada um dos 4 tipos de peça
    for (int j = 0; j < pecas_por_tipo; j++) { // Adiciona 9 de cada
      saco_de_pecas[contador] = pecas[i];
      contador++;
    }
  }

  // Passo 3: Embaralharando o saco temporário
  for (size_t i = total_pecas - 1; i > 0; i--) {
    // Escolhe um índice aleatório de 0 a i
    size_t j = rand() % (i + 1);

    // Troca o elemento de i com o elemento do índice aleatório j
    Casa temp = saco_de_pecas[i];
    saco_de_pecas[i] = saco_de_pecas[j];
    saco_de_pecas[j] = temp;
  }

  // Passo 4: Distribuir as peças embaralhadas no tabuleiro
  contador = 0;
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      t->grid[i][j] = saco_de_pecas[contador];
      contador++;
    }
  }
}

void print_tab(const Tabuleiro *t, size_t n) {
  size_t width = 3.9 * n;
  printf("   ");
  for (size_t j = 0; j < n; ++j)
    printf(" %zu  ", j);
  printf("\n  ┏");
  for (size_t j = 0; j < width; ++j)
    if ((j + 1) % 4 == 0)
      printf("┳");
    else
      printf("━");
  printf("┓\n");

  for (size_t i = 0; i < n; ++i) {
    printf("%zu ┃", i);
    for (size_t j = 0; j < n; ++j) {
      printf(" %c ", (char)t->grid[i][j]);
      if (j < n - 1)
        printf("┃");
    }
    printf("┃\n");
    if (i < n - 1) {
      printf("  ┣");
      for (size_t j = 0; j < width; ++j)
        if ((j + 1) % 4 == 0)
          printf("╋");
        else
          printf("━");
      printf("┫\n");
    }
  }

  printf("  ┗");
  for (size_t j = 0; j < width; ++j)
    if ((j + 1) % 4 == 0)
      printf("┻");
    else
      printf("━");
  printf("┛\n");
}

bool insercao_valida(Tabuleiro *t, size_t row, size_t col, Casa simb,
                     size_t n) {
  if (row >= n || col >= n)
    return false;
  return true;
}

void inserir(Tabuleiro *t, size_t row, size_t col, Casa peca, size_t n) {
  if (insercao_valida(t, row, col, peca, n))
    t->grid[row][col] = peca;
  else
    printf("Faça uma jogada válida!\n");
}

int main(void) {
  srand((unsigned)time(NULL));
  Tabuleiro tab;
  inicia_tab(&tab, N);
  print_tab(&tab, N);
  int linha, coluna;
  Casa peca;

  for (size_t i = 0; i <= MAXIMUM_PLAYS; i++) {
    if (i % 2 == 0) { // indica que é a vez do jogador A
      printf("Vez do jogador A:\n");
      printf("*****************\n");

      printf("Linha: ");
      scanf("%d", &linha);

      printf("Coluna: ");
      scanf("%d", &coluna);
      peca = BRANCO;

      inserir(&tab, linha, coluna, BRANCO, N);
      print_tab(&tab, N);
    }
    if (i % 2 != 0) {
      printf("Vez do jogador B:\n");
      printf("*****************\n");

      printf("Linha: ");
      scanf("%d", &linha);

      printf("Coluna: ");
      scanf("%d", &coluna);
      peca = PRETO;

      inserir(&tab, linha, coluna, PRETO, N);
      print_tab(&tab, N);
    }
  }
  return 0;
}
