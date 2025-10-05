INSERT GAME
                                  
Este documento descreve a implementação do jogo de tabuleiro DONUTS (originalmente
chamado de INSERT), desenvolvido em linguagem C como parte do projeto da disciplina
de Introdução à Programação.

1. SOBRE O JOGO

DONUTS é um jogo de estratégia abstrato para dois jogadores. O objetivo é
alinhar anéis da sua cor em um tabuleiro dinâmico, onde cada jogada influencia
diretamente as opções do oponente.

2. OBJETIVO

Existem duas maneiras de vencer:

1.  **Vitória Imediata**: Ser o primeiro jogador a criar uma linha contínua de
    5 anéis da sua cor (na horizontal, vertical ou diagonal).

2.  **Vitória por Pontos**: Se o tabuleiro for preenchido com 30 peças e ninguém
    tiver conseguido uma linha de 5, o jogo termina. O vencedor é o jogador
    que possuir o maior grupo de anéis da sua cor conectados ortogonalmente
    (lados adjacentes).

3. REGRAS PRINCIPAIS

O jogo é regido por três mecânicas fundamentais:

* **Movimento Obrigatório**: A casa onde um jogador posiciona sua peça
    determina a direção (linha, coluna ou diagonal) em que o oponente é
    obrigado a jogar na sua vez. O tipo de casa (`-`, `|`, `\`, `/`) impõe a regra.

* **Regra "FREEDOM!"**: Se na direção imposta pela jogada anterior não houver
    nenhum espaço vazio, o jogador atual ganha a liberdade ("FREEDOM!") para
    jogar em qualquer outra casa vazia do tabuleiro.

* **Mecânica "INSERT"**: Ao posicionar uma peça de forma que ela fique entre
    duas peças do oponente (formando um "sanduíche" em linha reta), a peça
    do oponente que está no meio é "virada" e passa a pertencer ao jogador
    que realizou a jogada. Uma única jogada pode resultar em múltiplos "inserts"
    em diferentes direções.

4. ESTRUTURA DO CÓDIGO

O projeto foi desenvolvido em um único arquivo, `main.c`, e utiliza estruturas
de dados simples e eficientes para gerenciar o estado do jogo.

* **Estruturas de Dados (`typedef`)**:
    * `enum Casa`: Define todos os possíveis estados de uma casa no tabuleiro,
        incluindo as peças dos jogadores (`PRETO`, `BRANCO`) e os tipos de
        direção (`HORIZONTAL`, `VERTICAL`, `DIAGONAL_PRINCIPAL`, `DIAGONAL_SECUNDARIA`).
    * `struct Tabuleiro`: Contém a matriz `grid[6][6]` que representa o
        tabuleiro do jogo.
    * `struct Coordenada`: Armazena um par de linha e coluna, útil para
        guardar a posição da última jogada.

* **Funções Obrigatórias (Conforme Especificação)**:
    * `void inicia_tab(...)`: Prepara o tabuleiro para o início do jogo,
        distribuindo aleatoriamente os 4 tipos de casa.
    * `void print_tab(...)`: Exibe o tabuleiro de forma gráfica e clara no
        terminal.
    * `int validaJogada(...)`: Embora a lógica principal de validação tenha
        sido movida para o `main` para melhor controle de estado, esta função
        foi mantida para cumprir os requisitos do projeto.
    * `void fazInsert(...)`: Verifica e executa a mecânica de "Insert" após
        uma peça ser colocada no tabuleiro.
    * `int verificaVitoria(...)`: Centraliza a verificação das condições de
        fim de jogo (5 em linha ou esgotamento de turnos).

* **Funções Auxiliares**:
    * `void inserir(...)`: Simplesmente coloca uma peça em uma coordenada
        específica.
    * `int contarMaiorArea(...)` e `int contarRegiao(...)`: Funções que
        implementam um algoritmo de busca para encontrar o maior grupo de
        peças conectadas, usadas para a condição de vitória por pontos.

5. COMO COMPILAR E EXECUTAR

Para compilar e executar o projeto, utilize um compilador C padrão como o GCC.

1.  Abra um terminal na pasta onde o arquivo `main.c` está localizado.
2.  Execute o seguinte comando para compilar:
    ```bash
    gcc main.c -o donuts
    ```
3.  Execute o jogo com o comando:
    ```bash
    ./donuts
    ```
4.  O jogo começará, e os jogadores poderão inserir as coordenadas (linha e
    coluna) para fazer suas jogadas.
    
FIM!
