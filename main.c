#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 6

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
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      int r = rand() % 4;
      switch (r) {
      case 0:
        t->grid[i][j] = VERTICAL;
        break;
      case 1:
        t->grid[i][j] = HORIZONTAL;
        break;
      case 2:
        t->grid[i][j] = DIAGONAL_PRINCIPAL;
        break;
      default:
        t->grid[i][j] = DIAGONAL_SECUNDARIA;
        break;
      }
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

bool coloca(Tabuleiro *t, size_t row, size_t col, Casa simb, size_t n) {
  if (row >= n || col >= n)
    return false;
  t->grid[row][col] = simb;
  return true;
}

int main(void) {
  srand((unsigned)time(NULL));
  Tabuleiro tab;
  inicia_tab(&tab, N);
  print_tab(&tab, N);
  return EXIT_SUCCESS;
}
