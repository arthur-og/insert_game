#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 6
#define MAXIMUM_PLAYS (N * N) / 2

typedef enum
{
  BRANCO = 'O',
  PRETO = 'X',
  VERTICAL = '|',
  HORIZONTAL = '-',
  DIAGONAL_PRINCIPAL = '\\',
  DIAGONAL_SECUNDARIA = '/'
} Casa;

typedef struct
{
  Casa grid[N][N];
} Tabuleiro;

typedef struct
{
  unsigned int linha;
  unsigned int coluna;
} Coordenada;

void inicia_tab(Tabuleiro *t, unsigned int n)
{
  // Passo 1: Criar um "saco" com o total de peças
  unsigned int total_pecas = n * n;
  Casa saco_de_pecas[total_pecas];

  // vetor temporário armazenando as peças que vamos utilizar
  Casa pecas[] = {VERTICAL, HORIZONTAL, DIAGONAL_PRINCIPAL,
                  DIAGONAL_SECUNDARIA};
  unsigned int pecas_por_tipo = total_pecas / 4;

  // Passo 2: Encher o saco temporário com 9 peças de cada
  unsigned int contador = 0;
  for (unsigned int i = 0; i < 4; i++)
  { // Para cada um dos 4 tipos de peça
    for (unsigned int j = 0; j < pecas_por_tipo; j++)
    { // Adiciona 9 de cada
      saco_de_pecas[contador] = pecas[i];
      contador++;
    }
  }

  // Passo 3: Embaralharando o saco temporário
  for (unsigned int i = total_pecas - 1; i > 0; i--)
  {
    // Escolhe um índice aleatório de 0 a i
    unsigned int j = rand() % (i + 1);

    // Troca o elemento de i com o elemento do índice aleatório j
    Casa temp = saco_de_pecas[i];
    saco_de_pecas[i] = saco_de_pecas[j];
    saco_de_pecas[j] = temp;
  }

  // Passo 4: Distribuir as peças embaralhadas no tabuleiro
  contador = 0;
  for (unsigned int i = 0; i < n; i++)
  {
    for (unsigned int j = 0; j < n; j++)
    {
      t->grid[i][j] = saco_de_pecas[contador];
      contador++;
    }
  }
}

void print_tab(const Tabuleiro *t, unsigned int n)
{
  unsigned int width = 3.9 * n;
  printf("   ");
  for (unsigned int j = 0; j < n; ++j)
    printf(" %u  ", j);
  printf("\n  ┏");
  for (unsigned int j = 0; j < width; ++j)
    if ((j + 1) % 4 == 0)
      printf("┳");
    else
      printf("━");
  printf("┓\n");

  for (unsigned int i = 0; i < n; ++i)
  {
    printf("%u ┃", i);
    for (unsigned int j = 0; j < n; ++j)
    {
      printf(" %c ", (char)t->grid[i][j]);
      if (j < n - 1)
        printf("┃");
    }
    printf("┃\n");
    if (i < n - 1)
    {
      printf("  ┣");
      for (unsigned int j = 0; j < width; ++j)
        if ((j + 1) % 4 == 0)
          printf("╋");
        else
          printf("━");
      printf("┫\n");
    }
  }

  printf("  ┗");
  for (unsigned int j = 0; j < width; ++j)
    if ((j + 1) % 4 == 0)
      printf("┻");
    else
      printf("━");
  printf("┛\n");
}

bool insercao_valida(Tabuleiro *t, unsigned int linha, unsigned int coluna, unsigned int n, Coordenada coordenada_anterior, int contador_de_jogadas)
{
  if (linha >= n || coluna >= n)
  {
    return false;
  }

  Casa casa_alvo_atual = t->grid[linha][coluna];

  if (casa_alvo_atual == BRANCO || casa_alvo_atual == PRETO)
    return false;

  if(contador_de_jogadas > 0)
  {
  unsigned int linha_anterior = coordenada_anterior.linha;
  unsigned int coluna_anterior = coordenada_anterior.coluna;

  Casa tipo_casa_anterior = t->grid[linha_anterior][coluna_anterior];
  if(casa_alvo_atual != tipo_casa_anterior) return false;
  }
  return true;
}

void inserir(Tabuleiro *t, unsigned int linha, unsigned int coluna, Casa peca)
{
  t->grid[linha][coluna] = peca;
}

char *nomeJogador(Casa jogador_atual)
{
  if (jogador_atual == PRETO)
    return "da rosca preta";
  else
    return "da rosca branca";
}

int main(void)
{
  srand((unsigned)time(NULL));
  Tabuleiro tab;
  inicia_tab(&tab, N);
  Casa jogador_atual = PRETO;
  bool vitoria = false;
  unsigned int linha, coluna;
  Coordenada coordenada_anterior = { .linha = -1, .coluna = -1};
  int contador_de_jogadas = 0;
  

  // loop principal
  while (!vitoria)
  {
    print_tab(&tab, N);
    printf("\nVez do jogador %s\n", nomeJogador(jogador_atual));

    // rodada
    while (true)
    {
      printf("Linha: ");
      scanf("%u", &linha);

      printf("Coluna: ");
      scanf("%u", &coluna);
      printf("\n");

      if (insercao_valida(&tab, linha, coluna, N, coordenada_anterior, contador_de_jogadas))
      {
        coordenada_anterior.linha = linha;
        coordenada_anterior.coluna = coluna;
        contador_de_jogadas++;
        break;
      }

      else
      {
        printf("\nJogada inválida! Tente novamente.\n");
      }
    }

    inserir(&tab, linha, coluna, jogador_atual);

    if (jogador_atual == PRETO)
      jogador_atual = BRANCO;
    else
      jogador_atual = PRETO;
  }
  /*Verificando se houveram jogadas especiais*/

  return 0;
}
