/* board_symbols.c
 *
 * Compilar:
 *   gcc -std=c11 -Wall -Wextra -pedantic -O2 -o board_symbols board_symbols.c
 *
 * Objetivos:
 * - ter símbolos para peças e para direções/linhas (| - / \)
 * - inicializar, imprimir, colocar símbolo e ler jogada simples
 * - mostrar uso correto do caractere backslash ('\\')
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define N 6

/* Usamos um enum para nomear os símbolos mas os valores são caracteres
 * (isto facilita impressão direta com %c).
 *
 * Note: a barra invertida (backslash) precisa ser escrita como '\\' em um
 * literal de caractere C.
 */
typedef enum {
  /* célula vazia */
  BRANCO = 'O',              /* peça branca */
  PRETO = 'X',               /* peça preta */
  VERTICAL = '|',            /* linha vertical */
  HORIZONTAL = '-',          /* linha horizontal */
  DIAGONAL_PRINCIPAL = '\\', /* diagonal '\': precisa de escape '\\' */
  DIAGONAL_SECUNDARIA = '/'  /* diagonal '/' */
} Casa;

/* Tabuleiro com tamanho fixo N x N */
typedef struct {
  Casa grid[N][N];
} Tabuleiro;

/* Inicializa todas as casas como EMPTY */
void inicia_tab(Tabuleiro *t, size_t n) {
  for (size_t i = 0; i < n; ++i)
    for (size_t j = 0; j < n; ++j)
      t->grid[i][j] = PRETO;
}

/* Imprime o tabuleiro com índices e os símbolos (imprime cada Casa como char)
 */
void print_tab(const Tabuleiro *t, size_t n) {
  /* cabeçalho das colunas */
  printf("   ");
  for (size_t j = 0; j < n; ++j)
    printf(" %zu  ", j);
  printf("\n  ┏");
  for (size_t j = 0; j < 3.8 * n; ++j)
    if ((j + 1) % 4 == 0) {
      printf("┳");
    } else {
      printf("━");
    }
  printf("┓\n");

  for (size_t i = 0; i < n; ++i) {
    printf("%zu ┃", i);
    for (size_t j = 0; j < n; ++j) {
      /* cast para (char) apenas para deixar explícito que imprimimos o valor
       * como caractere */
      printf(" %c ", (char)t->grid[i][j]);
      if (j < n - 1) {
        printf("┃");
      }
    }

    printf("┃\n");

    if (i < n - 1) {
      printf("  ");
      printf("┣");
      for (size_t j = 0; j < 3.8 * n; ++j)
        if ((j + 1) % 4 == 0) {
          printf("╋");
        } else {
          printf("━");
        }

      printf("┫\n");
    }
  }

  printf("  ┗");
  for (size_t j = 0; j < 3.8 * n; ++j)
    if ((j + 1) % 4 == 0) {
      printf("┻");
    } else {
      printf("━");
    }
  printf("┛\n");
}

/* Coloca um símbolo no tabuleiro; retorna true se bem sucedido */
bool coloca(Tabuleiro *t, size_t row, size_t col, Casa simb, size_t n) {
  if (row >= n || col >= n)
    return false;
  t->grid[row][col] = simb;
  return true;
}

int main(void) {
  Tabuleiro tab;
  inicia_tab(&tab, N);

  /* demonstração: preenche com linhas/diagonais e peças */

  /* imprime resultado */
  print_tab(&tab, N);

  return EXIT_SUCCESS;
}
