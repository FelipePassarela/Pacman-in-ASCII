/*************************************************************************************
 * @file FelipeDosSantosPassarela.c
 * @author Felipe dos Santos Passarela (felipepassarela11@gmail.com)
 *
 * @brief Este projeto eh um trabalho da disciplina Programacao 1, do curso de
 * Ciencia da Computacao, ofertado pela Universidade Federal do Espirito Santo (UFES).
 *
 * O programa eh o jogo do Pac-Man implementado em ASCII, onde o jogador controla o
 * personagem para coletar pontos e evitar os fantasmas. O objetivo eh obter a maior
 * pontuacao possivel sem ser pego pelos fantasmas.
 *
 * @date 2023-07-07
 *
*************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_DIR_SIZE 1001
#define QTD_FANTASMAS 4
#define QTD_COMIDAS 38 * 98 - 1
#define QTD_MOVIMENTOS 4
#define QTD_PORTAIS 2

#define W 0
#define A 1
#define S 2
#define D 3

typedef struct
{
    int linhas;
    int colunas;
    int lim_movs;
    char mapa[40][100];
    int trilha[40][100];
} tMapa;

typedef struct
{
    int x;
    int y;
    int pontos;
    int movs_restantes;
    int mov_atual;
    char direcao;
    int colidiu_com_parede;
    int colidiu_com_fantasma;
    int colidiu_com_comida;
    int colidiu_com_portal;
} tPacman;

typedef struct
{
    int x;
    int y;
    int existe;
    char tipo;
    char direcao;
    int colidiu_com_parede;
    int colidiu_com_portal;
} tFantasma;

typedef struct
{
    int x;
    int y;
    int foi_comida;
} tComida;

typedef struct
{
    char direcao;
    int comidas_pegas;
    int colisoes_parede;
    int colisoes_apenas_parede;
    int qtd_realizado;
} tEstatisticaMovimento;

typedef struct
{
    int x;
    int y;
} tPortal;

typedef struct
{
    tMapa mapa;
    tPacman pacman;
    tFantasma fantasmas[QTD_FANTASMAS];
    tComida comidas[QTD_COMIDAS];
    tPortal portais[2];
    tEstatisticaMovimento estatistica_movs[QTD_MOVIMENTOS];

    int qtd_inicial_comidas;
    int comidas_restantes;
    int trocaram_posicao;

    char dir[MAX_DIR_SIZE];
} tJogo;


/////////////////////////////////// FUNCOES DO MAPA //////////////////////////////////
/**
 * @brief Cria um mapa a partir de um arquivo de texto.
 *
 * @param dir Diretorio do arquivo de texto
 * @return tMapa Struct do mapa criado
 */
tMapa criaMapa(char* dir);

/**
 * @brief Atualiza o mapa com as posicoes dos personagens e comidas.
 *
 * @param jogo Struct do jogo
 * @return tMapa Struct do mapa atualizado
 */
tMapa atualizaMapa(tJogo jogo);

/**
 * @brief Cria a trilha do pacman no mapa. Por onde o pacman passa, a trilha eh
 * marcada com seu respectivo numero de movimento naquele momento.
 *
 * @param jogo Struct do jogo
 * @return tJogo Struct do jogo com a trilha criada
 */
tJogo criaTrilha(tJogo jogo);

/**
 * @brief Imprime o mapa no arquivo de saida.
 *
 * @param mapa Struct do mapa a ser impresso
 * @param saidaFile Arquivo de saida
 */
void fprintMapa(tMapa mapa, FILE* saidaFile);

/**
 * @brief Imprime o mapa na tela.
 *
 * @param mapa Struct do mapa a ser impresso
 */
void printMapa(tMapa mapa);


////////////////////////////////// FUNCOES DO PACMAN /////////////////////////////////
/**
 * @brief Cria o Pacman na posicao inicial.
 *
 * @param jogo Struct do jogo
 * @return tPacman Struct do Pacman criado
 */
tPacman criaPacman(tJogo jogo);

/**
 * @brief Move o Pacman de acordo com sua direcao.
 *
 * @param jogo Struct do jogo
 * @return tPacman Struct do Pacman movido
 */
tPacman movePacman(tJogo jogo);

/**
 * @brief Verifica a colisao do Pacman com os objetos do mapa.
 *
 * @param jogo Struct do jogo
 * @return tPacman Struct do Pacman com as colisoes verificadas
 */
tPacman verificaColisaoPacman(tJogo jogo);

/**
 * @brief Verifica se existe um Pacman na posicao indicada.
 *
 * @param x Posicao x
 * @param y Posicao y
 * @param pacman Struct do Pacman
 * @return int 1 se existe um Pacman na posicao indicada, 0 caso contrario
 */
int temPacmanNaPosicao(int x, int y, tPacman pacman);


///////////////////////////////// FUNCOES DOS FANTASMAS //////////////////////////////
/**
 * @brief Cria os fantasmas do jogo.
 *
 * @param jogo Struct do jogo sem o vetor de fantasmas
 * @return tJogo Struct do jogo com o vetor de fantasmas
 */
tJogo criaFantasmas(tJogo jogo);

/**
 * @brief Move os fantasmas de acordo com suas direcoes.
 *
 * @param jogo Struct do jogo
 * @return tJogo Struct do jogo com os fantasmas movidos
 */
tJogo moveFantasmas(tJogo jogo);

/**
 * @brief Verifica se existe um fantasma na posicao indicada.
 *
 * @param x Posicao x
 * @param y Posicao y
 * @param fantasmas Vetor de fantasmas
 * @return char Retorna o tipo do fantasma se existir, ou '\0' se nao existir.
 */
char temFantasmaNaPosicao(int x, int y, tFantasma fantasmas[QTD_FANTASMAS]);


///////////////////////////////// FUNCOES DAS COMIDAS ////////////////////////////////
/**
 * @brief Cria um vetor de comidas a partir do mapa do jogo.
 *
 * @param jogo Struct do jogo
 * @return tJogo Struct do jogo atualizada
 */
tJogo criaComidas(tJogo jogo);

/**
 * @brief Remove uma comida do vetor de comidas se a posicao do parametro
 * for igual a posicao da comida.
 *
 * @param x Posicao x do personagem
 * @param y Posicao y do personagem
 * @param jogo Struct do jogo
 * @return tJogo Struct do jogo atualizada
 */
tJogo comeComida(int x, int y, tJogo jogo);

/**
 * @brief Verifica se existe uma comida na posicao indicada.
 *
 * @param x Posicao x do personagem
 * @param y Posicao y do personagem
 * @param qtd_inicial_comidas Quantidade inicial de comidas
 * @param comidas Vetor de comidas
 * @return int 1 se existe uma comida na posicao, 0 se nao existe.
 */
int temComidaNaPosicao(int x, int y, int qtd_inicial_comidas, tComida comidas[qtd_inicial_comidas]);


///////////////////////////////// FUNCOES DOS PORTAIS ////////////////////////////////
/**
 * @brief Cria os portais do jogo.
 *
 * @param jogo Struct do jogo
 * @return tJogo Struct do jogo com os portais
 */
tJogo criaPortais(tJogo jogo);

/**
 * @brief Verifica se o Pacman esta em cima de um portal. Caso esteja, move ele para
 * a posição do outro portal.
 *
 * @param pacman Struct do Pacman
 * @param jogo Struct do jogo
 * @return tPacman Struct do Pacman atualizada para nova posicao
 */
tPacman teleportaPacman(tPacman pacman, tJogo jogo);

/**
 * @brief Verifica se existe um portal na posicao indicada.
 *
 * @param x Posicao x
 * @param y Posicao y
 * @param portais Vetor de portais
 * @return int 1 se existe um portal na posicao, 0 caso contrario
 */
int temPortalNaPosicao(int x, int y, tPortal portais[QTD_PORTAIS]);


///////////////////////////////////// FUNCOES DO JOGO ////////////////////////////////
/**
 * @brief Cria um novo jogo.
 *
 * @param dir Diretorio padrao do jogo
 * @return tJogo Struct do jogo criado
 */
tJogo inicializaJogo(char* dir);

/**
 * @brief Funcao que realiza o jogo. Chama as funcoes de movimentacao do Pacman e dos
 * fantasmas. Após isso, atualiza o mapa e imprime o estado do jogo.
 *
 * @param jogo Struct do jogo
 * @return tJogo Struct do jogo
 */
tJogo realizaJogo(tJogo jogo);

/**
 * @brief Atualiza os objetos do jogo de acordo com as posicoes dos personagens.
 *
 * @param jogo Struct do jogo
 * @return tJogo Struct do jogo atualizada
 */
tJogo atualizaObjetos(tJogo jogo);

/**
 * @brief Imprime o estado do jogo apos o movimento do pacman.
 *
 * @param jogo Struct do jogo
 */
void printEstadoJogo(tJogo jogo);

/**
 * @brief Verifica se o jogo acabou e imprime na tela o resultado.
 *
 * @param jogo Struct do jogo a ser verificado
 * @return int 1 se o jogador venceu, -1 se perdeu ou 0 caso o jogo nao tenha acabado
 */
int verificaFimDeJogo(tJogo jogo);

/**
 * @brief Verifica se o Pacman e um fantasma trocaram de posicao.
 *
 * Essa funcao eh necessaria para que o Pacman nao troque sua posicao com
 * um fantasma em vez de colidir com ele. O que causaria um bug, fazendo
 * com que o jogo nao terminasse nesse caso especifico.
 *
 * @param pacman Struct do Pacman
 * @param fantasmas Vetor de fantasmas
 * @return int 1 se o Pacman e um fantasma trocaram de posicao, 0 caso contrario
 */
int verificaTrocaDePosicao(tPacman pacman, tFantasma fantasmas[QTD_FANTASMAS]);


///////////////////////////////// FUNCOES DE ESTATISTICAS ////////////////////////////
/**
 * @brief Cria a pasta de saida do jogo.
 * 
 * @param dir Diretorio do jogo 
 */
void criaPastaSaida(char* dir);

/**
 * @brief Gera o arquivo 'inicializacao.txt' com as informacoes iniciais do jogo.
 *
 * @param jogo Struct do jogo
 */
void geraInicializacaoTxt(tJogo jogo);

/**
 * @brief Limpa o arquivo 'resumo.txt' para uma nova execucao
 *
 * @param dir Diretorio do jogo
 */
void limpaResumo(char* dir);

/**
 * @brief Atualiza o arquivo 'resumo.txt' com informacoes dos movimentos relevantes.
 *
 * @param jogo Struct do jogo
 */
void atualizaResumo(tJogo jogo);

/**
 * @brief Cria as structs de estatisticas de movimentos do jogo.
 *
 * @param jogo Struct do jogo
 * @return tJogo Struct do jogo com o vetor de estatisticas de movimentos
 */
tJogo criaEstatisticasMovimentos(tJogo jogo);

/**
 * @brief Atualiza as estatisticas de movimentos do jogo.
 *
 * @param jogo Struct do jogo
 * @return tJogo Struct do jogo atualizada
 */
tJogo atualizaEstatisticasMovimentos(tJogo jogo);

/**
 * @brief Gera o arquivo 'ranking.txt' com as estatisticas dos movimentos. Os
 * movimentos sao ordenados de acordo com quatro criterios: Mais comidas pegas, menos
 * colisoes com paredes, mais movimentos realizados, e por ordem alfabetica.
 *
 * @param jogo Struct do jogo
 * @return tJogo Struct do jogo atualizada
 */
void geraRankingTxt(tJogo jogo);

/**
 * @brief Gera o arquivo 'estatisticas.txt' com as estatisticas dos movimentos
 * do Pacman.
 *
 * @param jogo Struct do jogo
 */
void geraEstatisticasTxt(tJogo jogo);

/**
 * @brief Gera o arquivo 'trilha.txt' com a trilha percorrida pelo Pacman.
 *
 * @param jogo Struct do jogo
 */
void geraTrilhaTxt(tJogo jogo);


/****************************************** MAIN ********************************************/
int main(int argc, char* argv[])
{
    if (argc <= 1) {
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 1;
    }

    char dir[MAX_DIR_SIZE];
    strcpy(dir, argv[1]);

    tJogo jogo = inicializaJogo(dir);
    geraInicializacaoTxt(jogo);

    while (1) {
        jogo = realizaJogo(jogo);

        printEstadoJogo(jogo);

        jogo = atualizaEstatisticasMovimentos(jogo);
        atualizaResumo(jogo);

        if (verificaFimDeJogo(jogo))
            break;
    }

    geraEstatisticasTxt(jogo);
    geraRankingTxt(jogo);
    geraTrilhaTxt(jogo);

    return 0;
}


////////////////////////////////// FUNCOES DO MAPA ///////////////////////////////////
tMapa criaMapa(char* dir) {
    FILE* mapaFile;
    char mapa_dir[MAX_DIR_SIZE + 9];
    sprintf(mapa_dir, "%s/mapa.txt", dir);

    mapaFile = fopen(mapa_dir, "r");

    if (mapaFile == NULL) {
        printf("ERRO: arquivo 'mapa.txt' nao encontrado em %s\n", dir);
        exit(1);
    }

    tMapa mapa = { 0 };
    fscanf(mapaFile, "%d %d %d\n", &mapa.linhas, &mapa.colunas, &mapa.lim_movs);

    int i, j;
    for (i = 0; i < mapa.linhas; i++) {
        for (j = 0; j < mapa.colunas; j++) {
            fscanf(mapaFile, "%c", &mapa.mapa[i][j]);
        }
        fscanf(mapaFile, "\n");
    }

    fclose(mapaFile);

    return mapa;
}

tMapa atualizaMapa(tJogo jogo) {
    int i, j;
    tMapa mapa = jogo.mapa;

    for (i = 0; i < mapa.linhas; i++) {
        for (j = 0; j < mapa.colunas; j++) {
            if (mapa.mapa[i][j] == '#') continue;

            mapa.mapa[i][j] = ' ';

            if (temComidaNaPosicao(j, i, jogo.qtd_inicial_comidas, jogo.comidas)) {
                mapa.mapa[i][j] = '*';
            } else if (temPortalNaPosicao(j, i, jogo.portais)) {
                mapa.mapa[i][j] = '@';
            }

            if (jogo.pacman.x == j && jogo.pacman.y == i && !temFantasmaNaPosicao(j, i, jogo.fantasmas)) {
                mapa.mapa[i][j] = '>';
                mapa.trilha[i][j] = jogo.pacman.mov_atual;
            }

            if (temFantasmaNaPosicao(j, i, jogo.fantasmas)) {
                mapa.mapa[i][j] = temFantasmaNaPosicao(j, i, jogo.fantasmas);
            }
        }
    }

    return mapa;
}

tJogo criaTrilha(tJogo jogo) {
    int i, j;
    tMapa mapa = jogo.mapa;

    for (i = 0; i < mapa.linhas; i++) {
        for (j = 0; j < mapa.colunas; j++) {
            if (mapa.mapa[i][j] == '>') {
                mapa.trilha[i][j] = 0;
            } else {
                mapa.trilha[i][j] = -1;
            }
        }
    }

    jogo.mapa = mapa;

    return jogo;
}

void fprintMapa(tMapa mapa, FILE* saidaFile) {
    int i, j;

    for (i = 0; i < mapa.linhas; i++) {
        for (j = 0; j < mapa.colunas; j++) {
            fprintf(saidaFile, "%c", mapa.mapa[i][j]);
        }
        fprintf(saidaFile, "\n");
    }
}

void printMapa(tMapa mapa) {
    int i, j;

    for (i = 0; i < mapa.linhas; i++) {
        for (j = 0; j < mapa.colunas; j++) {
            printf("%c", mapa.mapa[i][j]);
        }
        printf("\n");
    }
}


////////////////////////////////// FUNCOES DO PACMAN /////////////////////////////////
tPacman criaPacman(tJogo jogo) {
    int i, j;

    for (i = 0; i < jogo.mapa.linhas; i++) {
        for (j = 0; j < jogo.mapa.colunas; j++) {
            char posicao = jogo.mapa.mapa[i][j];

            if (posicao == '>') {
                tPacman pacman = { 0 };

                pacman.y = i;
                pacman.x = j;
                pacman.pontos = 0;
                pacman.movs_restantes = jogo.mapa.lim_movs;
                pacman.mov_atual = 0;
                pacman.colidiu_com_parede = 0;
                pacman.colidiu_com_fantasma = 0;
                pacman.colidiu_com_comida = 0;
                pacman.colidiu_com_portal = 0;

                return pacman;
            }
        }
    }
}

tPacman movePacman(tJogo jogo) {
    tPacman pacman = jogo.pacman;

    pacman.mov_atual++;
    pacman.movs_restantes--;

    int new_x = pacman.x;
    int new_y = pacman.y;

    if (pacman.direcao == 'w') {
        new_y--;
    } else if (pacman.direcao == 'a') {
        new_x--;
    } else if (pacman.direcao == 's') {
        new_y++;
    } else if (pacman.direcao == 'd') {
        new_x++;
    }

    if (jogo.mapa.mapa[new_y][new_x] != '#') {
        if (!jogo.trocaram_posicao) {
            pacman.x = new_x;
            pacman.y = new_y;
        }
        pacman.colidiu_com_parede = 0;
    } else {
        pacman.colidiu_com_parede = 1;
    }

    return pacman;
}

tPacman verificaColisaoPacman(tJogo jogo) {
    tPacman pacman = jogo.pacman;

    pacman.colidiu_com_fantasma = 0;
    pacman.colidiu_com_comida = 0;
    pacman.colidiu_com_portal = 0;

    if (temFantasmaNaPosicao(pacman.x, pacman.y, jogo.fantasmas) || jogo.trocaram_posicao) {
        pacman.colidiu_com_fantasma = 1;
    } else if (temComidaNaPosicao(pacman.x, pacman.y, jogo.qtd_inicial_comidas, jogo.comidas)) {
        pacman.colidiu_com_comida = 1;
    }

    if (temPortalNaPosicao(pacman.x, pacman.y, jogo.portais)) {
        pacman.colidiu_com_portal = 1;
    }

    return pacman;
}

int temPacmanNaPosicao(int x, int y, tPacman pacman) {
    return (x == pacman.x && y == pacman.y);
}


//////////////////////////////// FUNCOES DOS FANTASMAS ///////////////////////////////
tJogo criaFantasmas(tJogo jogo) {
    int i, j, idx = 0;

    for (i = 0; i < jogo.mapa.linhas; i++) {
        for (j = 0; j < jogo.mapa.colunas; j++) {
            char posicao = jogo.mapa.mapa[i][j];

            if (posicao == 'B' || posicao == 'P' || posicao == 'I' || posicao == 'C') {
                tFantasma fantasma = { 0 };

                fantasma.x = j;
                fantasma.y = i;
                fantasma.existe = 1;
                fantasma.tipo = posicao;
                fantasma.colidiu_com_parede = 0;
                fantasma.colidiu_com_portal = 0;

                if (fantasma.tipo == 'B') fantasma.direcao = 'a';
                else if (fantasma.tipo == 'P') fantasma.direcao = 'w';
                else if (fantasma.tipo == 'I') fantasma.direcao = 's';
                else if (fantasma.tipo == 'C') fantasma.direcao = 'd';

                jogo.fantasmas[idx] = fantasma;

                idx++;
            }
        }
    }

    return jogo;
}

tJogo moveFantasmas(tJogo jogo) {
    int i;

    for (i = 0; i < QTD_FANTASMAS; i++) {
        if (!jogo.fantasmas[i].existe) continue;

        tFantasma fantasma = jogo.fantasmas[i];

        int new_x = fantasma.x;
        int new_y = fantasma.y;

        fantasma.colidiu_com_parede = 0;
        fantasma.colidiu_com_portal = 0;

        if (fantasma.direcao == 'w') {
            new_y--;
        } else if (fantasma.direcao == 'a') {
            new_x--;
        } else if (fantasma.direcao == 's') {
            new_y++;
        } else if (fantasma.direcao == 'd') {
            new_x++;
        }

        char prox_celula = jogo.mapa.mapa[new_y][new_x];

        if (prox_celula != '#' && prox_celula != '@') {
            fantasma.x = new_x;
            fantasma.y = new_y;
        } else if (prox_celula == '#') {
            fantasma.colidiu_com_parede = 1;
        } else if (prox_celula == '@') {
            fantasma.colidiu_com_portal = 1;
        }

        if (fantasma.colidiu_com_parede || fantasma.colidiu_com_portal) {

            if (fantasma.direcao == 'w') {
                fantasma.direcao = 's';
                fantasma.y++;
            } else if (fantasma.direcao == 's') {
                fantasma.direcao = 'w';
                fantasma.y--;
            } else if (fantasma.direcao == 'a') {
                fantasma.direcao = 'd';
                fantasma.x++;
            } else if (fantasma.direcao == 'd') {
                fantasma.direcao = 'a';
                fantasma.x--;
            }
        }

        jogo.fantasmas[i] = fantasma;
    }

    return jogo;
}

char temFantasmaNaPosicao(int x, int y, tFantasma fantasmas[QTD_FANTASMAS]) {
    int i;

    for (i = 0; i < QTD_FANTASMAS; i++) {
        if (!fantasmas[i].existe) continue;

        if (fantasmas[i].x == x && fantasmas[i].y == y) {
            return fantasmas[i].tipo;
        }
    }

    return '\0';
}


///////////////////////////////// FUNCOES DAS COMIDAS ////////////////////////////////
tJogo criaComidas(tJogo jogo) {
    int i, j;

    jogo.qtd_inicial_comidas = 0;
    jogo.comidas_restantes = 0;

    for (i = 0; i < jogo.mapa.linhas; i++) {
        for (j = 0; j < jogo.mapa.colunas; j++) {
            char posicao = jogo.mapa.mapa[i][j];

            if (posicao == '*') {
                tComida comida = { 0 };

                comida.x = j;
                comida.y = i;
                comida.foi_comida = 0;

                jogo.comidas[jogo.qtd_inicial_comidas] = comida;
                jogo.qtd_inicial_comidas++;
                jogo.comidas_restantes++;
            }
        }
    }

    return jogo;
}

tJogo comeComida(int x, int y, tJogo jogo) {
    int i;

    for (i = 0; i < jogo.qtd_inicial_comidas; i++) {
        tComida comida = jogo.comidas[i];

        if (comida.x == x && comida.y == y) {
            comida.x = -1;
            comida.y = -1;
            comida.foi_comida = 1;

            jogo.comidas[i] = comida;
            jogo.comidas_restantes--;

            return jogo;
        }
    }

    return jogo;
}

int temComidaNaPosicao(int x, int y, int qtd_inicial_comidas, tComida comidas[qtd_inicial_comidas]) {
    int i;

    for (i = 0; i < qtd_inicial_comidas; i++) {
        if (comidas[i].x == x && comidas[i].y == y) {
            if (!comidas[i].foi_comida) {
                return 1;
            }
        }
    }

    return 0;
}


///////////////////////////////// FUNCOES DOS PORTAIS ////////////////////////////////
tJogo criaPortais(tJogo jogo) {
    int i, j, idx = 0;

    for (i = 0; i < jogo.mapa.linhas; i++) {
        for (j = 0; j < jogo.mapa.colunas; j++) {
            char posicao = jogo.mapa.mapa[i][j];

            if (posicao == '@') {
                jogo.portais[idx].x = j;
                jogo.portais[idx].y = i;

                idx++;
            }
        }
    }

    return jogo;
}

tPacman teleportaPacman(tPacman pacman, tJogo jogo) {
    int i;

    for (i = 0; i < QTD_PORTAIS; i++) {
        tPortal portal = jogo.portais[i];

        if (pacman.x == portal.x && pacman.y == portal.y) {
            if (i == 0) {
                pacman.x = jogo.portais[1].x;
                pacman.y = jogo.portais[1].y;
            } else {
                pacman.x = jogo.portais[0].x;
                pacman.y = jogo.portais[0].y;
            }

            return pacman;
        }
    }

    return pacman;
}

int temPortalNaPosicao(int x, int y, tPortal portais[QTD_PORTAIS]) {
    int i;

    for (i = 0; i < QTD_PORTAIS; i++) {
        if (portais[i].x == x && portais[i].y == y) {
            return 1;
        }
    }

    return 0;
}


/////////////////////////////////// FUNCOES DO JOGO //////////////////////////////////
tJogo inicializaJogo(char* dir) {
    tJogo jogo = { 0 };

    strcpy(jogo.dir, dir);
    criaPastaSaida(jogo.dir);
    limpaResumo(jogo.dir);

    jogo.mapa = criaMapa(dir);
    jogo.pacman = criaPacman(jogo);
    jogo = criaFantasmas(jogo);
    jogo = criaComidas(jogo);
    jogo = criaPortais(jogo);
    jogo = criaEstatisticasMovimentos(jogo);
    jogo = criaTrilha(jogo);

    return jogo;
}

tJogo realizaJogo(tJogo jogo) {
    printEstadoJogo(jogo);
    scanf(" %c", &jogo.pacman.direcao);

    jogo = moveFantasmas(jogo);
    jogo.trocaram_posicao = verificaTrocaDePosicao(jogo.pacman, jogo.fantasmas);
    jogo.pacman = movePacman(jogo);

    jogo.pacman = verificaColisaoPacman(jogo);

    jogo = atualizaObjetos(jogo);
    jogo.mapa = atualizaMapa(jogo);

    return jogo;
}

tJogo atualizaObjetos(tJogo jogo) {
    tPacman pacman = jogo.pacman;

    if (pacman.colidiu_com_comida) {
        pacman.pontos++;
        jogo = comeComida(pacman.x, pacman.y, jogo);
    } else if (pacman.colidiu_com_portal) {
        jogo.mapa.trilha[pacman.y][pacman.x] = pacman.mov_atual;
        pacman = teleportaPacman(pacman, jogo);
    }

    jogo.pacman = pacman;

    return jogo;
}

void printEstadoJogo(tJogo jogo) {
    printf("Estado do jogo apos o movimento '%c':\n", jogo.pacman.direcao);
    printMapa(jogo.mapa);
    printf("Pontuacao: %d\n\n", jogo.pacman.pontos);
}

int verificaFimDeJogo(tJogo jogo) {
    int venceu = 0;
    tPacman pacman = jogo.pacman;

    if (jogo.comidas_restantes == 0) venceu = 1;
    if (pacman.movs_restantes == 0) venceu = -1;
    if (pacman.colidiu_com_fantasma) venceu = -1;

    if (venceu == 1) {
        printf("Voce venceu!\n");
        printf("Pontuacao final: %d\n", pacman.pontos);
    } else if (venceu == -1) {
        printf("Game over!\n");
        printf("Pontuacao final: %d\n", pacman.pontos);
    }

    return venceu;
}

int verificaTrocaDePosicao(tPacman pacman, tFantasma fantasmas[QTD_FANTASMAS]) {
    int i;
    char direcao_oposta;

    direcao_oposta = pacman.direcao == 'w' ? 's' : direcao_oposta;
    direcao_oposta = pacman.direcao == 'a' ? 'd' : direcao_oposta;
    direcao_oposta = pacman.direcao == 's' ? 'w' : direcao_oposta;
    direcao_oposta = pacman.direcao == 'd' ? 'a' : direcao_oposta;

    for (i = 0; i < QTD_FANTASMAS; i++) {
        if (!fantasmas[i].existe) continue;

        if (temPacmanNaPosicao(fantasmas[i].x, fantasmas[i].y, pacman)) {
            if (fantasmas[i].direcao == direcao_oposta) {
                return 1;
            }
        }
    }

    return 0;
}


//////////////////////////////// FUNCOES DE ESTATISTICAS //////////////////////////////
void criaPastaSaida(char* dir) {
    char pasta_saida_dir[MAX_DIR_SIZE + 7];
    sprintf(pasta_saida_dir, "%s/saida", dir);

    mkdir(pasta_saida_dir, 0777);
}

void geraInicializacaoTxt(tJogo jogo) {
    FILE* inicializacaoFile;
    char inicializacao_dir[MAX_DIR_SIZE + 26];
    sprintf(inicializacao_dir, "%s/saida/inicializacao.txt", jogo.dir);

    inicializacaoFile = fopen(inicializacao_dir, "w");

    if (inicializacaoFile == NULL) {
        printf("ERRO: Nao foi possivel abrir 'inicializacao.txt' em %s/saida\n", jogo.dir);
        exit(1);
    }

    fprintMapa(jogo.mapa, inicializacaoFile);
    fprintf(inicializacaoFile, "Pac-Man comecara o jogo na linha %d e coluna %d\n", jogo.pacman.y + 1, jogo.pacman.x + 1);

    fclose(inicializacaoFile);
}

void limpaResumo(char* dir) {
    FILE* resumoFile;
    char resumo_dir[MAX_DIR_SIZE + 19];
    sprintf(resumo_dir, "%s/saida/resumo.txt", dir);

    resumoFile = fopen(resumo_dir, "w");

    if (resumoFile == NULL) {
        printf("ERRO: Nao foi possivel abrir 'resumo.txt' em %s/saida\n", dir);
        exit(1);
    }

    fclose(resumoFile);
}

void atualizaResumo(tJogo jogo) {
    FILE* resumoFile;
    char resumo_dir[MAX_DIR_SIZE + 19];
    sprintf(resumo_dir, "%s/saida/resumo.txt", jogo.dir);

    resumoFile = fopen(resumo_dir, "a");

    if (resumoFile == NULL) {
        printf("ERRO: Nao foi possivel abrir 'resumo.txt' em %s/saida\n", jogo.dir);
        exit(1);
    }

    int i;
    tPacman pacman = jogo.pacman;

    if (pacman.colidiu_com_comida) {
        fprintf(resumoFile, "Movimento %d (%c) pegou comida\n", pacman.mov_atual, pacman.direcao);
    }
    if (pacman.colidiu_com_fantasma) {
        fprintf(resumoFile, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", pacman.mov_atual, pacman.direcao);
    }
    if (pacman.colidiu_com_parede && !pacman.colidiu_com_portal) {
        fprintf(resumoFile, "Movimento %d (%c) colidiu na parede\n", pacman.mov_atual, pacman.direcao);
    }

    fclose(resumoFile);
}

tJogo criaEstatisticasMovimentos(tJogo jogo) {
    jogo.estatistica_movs[W].direcao = 'w';
    jogo.estatistica_movs[W].comidas_pegas = 0;
    jogo.estatistica_movs[W].colisoes_parede = 0;
    jogo.estatistica_movs[W].colisoes_apenas_parede = 0;
    jogo.estatistica_movs[W].qtd_realizado = 0;

    jogo.estatistica_movs[S].direcao = 's';
    jogo.estatistica_movs[S].comidas_pegas = 0;
    jogo.estatistica_movs[S].colisoes_parede = 0;
    jogo.estatistica_movs[S].colisoes_apenas_parede = 0;
    jogo.estatistica_movs[S].qtd_realizado = 0;

    jogo.estatistica_movs[A].direcao = 'a';
    jogo.estatistica_movs[A].comidas_pegas = 0;
    jogo.estatistica_movs[A].colisoes_parede = 0;
    jogo.estatistica_movs[A].colisoes_apenas_parede = 0;
    jogo.estatistica_movs[A].qtd_realizado = 0;

    jogo.estatistica_movs[D].direcao = 'd';
    jogo.estatistica_movs[D].comidas_pegas = 0;
    jogo.estatistica_movs[D].colisoes_parede = 0;
    jogo.estatistica_movs[D].colisoes_apenas_parede = 0;
    jogo.estatistica_movs[D].qtd_realizado = 0;

    return jogo;
}

tJogo atualizaEstatisticasMovimentos(tJogo jogo) {
    int i;
    tPacman pacman = jogo.pacman;

    for (i = 0; i < QTD_MOVIMENTOS; i++) {
        tEstatisticaMovimento movimento = jogo.estatistica_movs[i];

        if (jogo.pacman.direcao == movimento.direcao) {
            movimento.comidas_pegas += (pacman.colidiu_com_comida);
            movimento.colisoes_parede += (pacman.colidiu_com_parede);
            movimento.colisoes_apenas_parede += (pacman.colidiu_com_parede && !pacman.colidiu_com_portal);
            movimento.qtd_realizado++;
        }

        jogo.estatistica_movs[i] = movimento;
    }

    return jogo;
}

void geraRankingTxt(tJogo jogo) {
    int i;

    // Ordena o ranking de movimentos
    for (i = 0; i < QTD_MOVIMENTOS; i++) {
        int j;
        int selected_idx = i;

        for (j = i + 1; j < QTD_MOVIMENTOS; j++) {
            tEstatisticaMovimento movimento1 = jogo.estatistica_movs[selected_idx];
            tEstatisticaMovimento movimento2 = jogo.estatistica_movs[j];

            if (movimento2.comidas_pegas > movimento1.comidas_pegas) {  // Primeiro criterio: mais comidas pegas
                selected_idx = j;
            } else if (movimento2.comidas_pegas == movimento1.comidas_pegas) {
                if (movimento2.colisoes_apenas_parede < movimento1.colisoes_apenas_parede) {  // Segundo criterio: menos colisoes com parede
                    selected_idx = j;
                } else if (movimento2.colisoes_apenas_parede == movimento1.colisoes_apenas_parede) {
                    if (movimento2.qtd_realizado > movimento1.qtd_realizado) {  // Terceiro criterio: mais movimentos realizados
                        selected_idx = j;
                    } else if (movimento2.qtd_realizado == movimento1.qtd_realizado) {
                        if (movimento2.direcao < movimento1.direcao) {  // Quarto criterio: ordem alfabetica
                            selected_idx = j;
                        }
                    }
                }
            }
        }

        if (selected_idx != i) {
            tEstatisticaMovimento movimento1 = jogo.estatistica_movs[selected_idx];
            tEstatisticaMovimento movimento2 = jogo.estatistica_movs[i];

            jogo.estatistica_movs[selected_idx] = movimento2;
            jogo.estatistica_movs[i] = movimento1;
        }
    }

    FILE* rankingFile;
    char ranking_dir[MAX_DIR_SIZE + 20];
    sprintf(ranking_dir, "%s/saida/ranking.txt", jogo.dir);

    rankingFile = fopen(ranking_dir, "w");

    if (rankingFile == NULL) {
        printf("ERRO: Nao foi possivel abrir 'ranking.txt' em %s/saida\n", jogo.dir);
        exit(1);
    }

    for (i = 0; i < QTD_MOVIMENTOS; i++) {
        tEstatisticaMovimento movimento = jogo.estatistica_movs[i];

        fprintf(rankingFile, "%c,%d,%d,%d\n", movimento.direcao, movimento.comidas_pegas, movimento.colisoes_apenas_parede, movimento.qtd_realizado);
    }

    fclose(rankingFile);
}

void geraEstatisticasTxt(tJogo jogo) {
    FILE* estatisticasFile;
    char estatisticas_dir[MAX_DIR_SIZE + 25];
    sprintf(estatisticas_dir, "%s/saida/estatisticas.txt", jogo.dir);

    estatisticasFile = fopen(estatisticas_dir, "w");

    if (estatisticasFile == NULL) {
        printf("ERRO: Nao foi possivel abrir 'estatisticas.txt' em %s/saida\n", jogo.dir);
        return;
    }

    int i;
    int numero_movs = 0;
    int numero_movs_sem_pontuar = 0;
    int numero_colisoes_parede = 0;
    int numero_movs_s = 0;
    int numero_movs_w = 0;
    int numero_movs_a = 0;
    int numero_movs_d = 0;

    for (i = 0; i < QTD_MOVIMENTOS; i++) {
        tEstatisticaMovimento movimento = jogo.estatistica_movs[i];

        numero_movs += movimento.qtd_realizado;
        numero_movs_sem_pontuar += movimento.qtd_realizado - movimento.comidas_pegas;
        numero_colisoes_parede += movimento.colisoes_parede;

        if (movimento.direcao == 's')
            numero_movs_s += movimento.qtd_realizado;
        else if (movimento.direcao == 'w')
            numero_movs_w += movimento.qtd_realizado;
        else if (movimento.direcao == 'a')
            numero_movs_a += movimento.qtd_realizado;
        else if (movimento.direcao == 'd')
            numero_movs_d += movimento.qtd_realizado;
    }

    fprintf(estatisticasFile, "Numero de movimentos: %d\n", numero_movs);
    fprintf(estatisticasFile, "Numero de movimentos sem pontuar: %d\n", numero_movs_sem_pontuar);
    fprintf(estatisticasFile, "Numero de colisoes com parede: %d\n", numero_colisoes_parede);
    fprintf(estatisticasFile, "Numero de movimentos para baixo: %d\n", numero_movs_s);
    fprintf(estatisticasFile, "Numero de movimentos para cima: %d\n", numero_movs_w);
    fprintf(estatisticasFile, "Numero de movimentos para esquerda: %d\n", numero_movs_a);
    fprintf(estatisticasFile, "Numero de movimentos para direita: %d\n", numero_movs_d);

    fclose(estatisticasFile);
}

void geraTrilhaTxt(tJogo jogo) {
    FILE* trilhaFile;
    char trilha_dir[MAX_DIR_SIZE + 19];
    sprintf(trilha_dir, "%s/saida/trilha.txt", jogo.dir);

    trilhaFile = fopen(trilha_dir, "w");

    if (trilhaFile == NULL) {
        printf("ERRO: Nao foi possivel abrir 'trilha.txt' em %s/saida\n", jogo.dir);
        return;
    }

    int i, j;
    tMapa mapa = jogo.mapa;
    for (i = 0; i < mapa.linhas; i++) {
        for (j = 0; j < mapa.colunas; j++) {
            if (mapa.trilha[i][j] >= 0) {
                if (j == mapa.colunas - 1)
                    fprintf(trilhaFile, "%d", mapa.trilha[i][j]);
                else
                    fprintf(trilhaFile, "%d ", mapa.trilha[i][j]);
            } else {
                if (j == mapa.colunas - 1)
                    fprintf(trilhaFile, "#");
                else
                    fprintf(trilhaFile, "# ");
            }
        }
        fprintf(trilhaFile, "\n");
    }

    fclose(trilhaFile);
}
