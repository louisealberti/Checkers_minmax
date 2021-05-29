#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define T 8

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Pecas                                                                     */
/*****************************************************************************/
typedef struct peca {
    int dama; /* eh dama 0 ou 1 */
    char jogador; /* 'H' (humano) ou 'C' (computador) */
} Peca;

/*****************************************************************************/
/* Espacos do tabuleiro - vale a pena?                                       */
/*****************************************************************************/
typedef struct espaco {
    int col;
    int lin;
    Peca p;
} Espaco;

/*****************************************************************************/
/* Jogadas                                                                   */
/*****************************************************************************/
typedef struct {
    Espaco j; /* Jogador */
    Espaco o; /* Oponente */
} Jogada;

/*****************************************************************************/
/* Coloca as peças no tabuleiro                                              */
/*****************************************************************************/
void colocarPecas(Espaco tabuleiro[T][T]) {
    for(int i = 0; i < T; i++) {
        for(int j = 0; j < T; j++) {
            char ch = 32;
            Espaco e;
            e.lin = i;
            e.col = j;
            e.p.dama = 0;

            if((i % 2 == 0 && j % 2 == 1) || (i % 2 == 1 && j % 2 == 0)) {
                if(i < 3) ch = 'C';         /* Computador */
                else if(i > 4) ch = 'H';    /* Humano */
                e.p.jogador = ch;
            }
            else e.p.jogador = ch; /* espaco vazio */
            tabuleiro[i][j] = e;
        }
    }
} /* end of colocarPecas */

/*****************************************************************************/
/* Imprime o tabuleiro                                                       */
/*****************************************************************************/
void imprimeTabuleiro(Espaco tabuleiro[T][T]){

    printf("  A B C D E F G H\n");

    for(int i = 0; i < T; i++){
        printf("%d ", i +1);
        for(int j = 0; j < T; j++){
            char ch = tabuleiro[i][j].p.jogador;
            j != 7 ? printf("%c|", ch) : printf("%c", ch);
        }
        i != 7 ? printf("\n  -+-+-+-+-+-+-+-\n") : printf("\n\n"); 
    }

} /* end of imprimeTabuleiro */

/*****************************************************************************/
/* Verifica se eh possivel mover um espaco.                                  */
/*****************************************************************************/
Espaco* jogadaSimples(Espaco tabuleiro[T][T], Espaco origem, int *k) {
    Espaco *jogadas;
    jogadas = (Espaco*)malloc(sizeof(Espaco));
    Espaco d;
    if(origem.lin + 1 < 8) {             /* confirma se a linha está dentro do tabuleiro */
        d.lin = origem.lin + 1;
        if(origem.col + 1 < 8) {         /* confirma se a coluna está dentro do tabuleiro */
            d.col = origem.col + 1;
            if(tabuleiro[d.lin][d.col].p.jogador == 32) {
                jogadas[*k].lin = d.lin;
                jogadas[*k].col = d.col;
                (*k)++;
            }
        }
        if(origem.col - 1 >= 0) {   /* confirma se a coluna está dentro do tabuleiro */
            d.col = origem.col - 1;
            if(tabuleiro[d.lin][d.col].p.jogador == 32) {
                if(k > 0) jogadas = (Espaco*)realloc(jogadas, ((*k) + 1) * sizeof(Espaco));
                jogadas[(*k)].lin = d.lin;
                jogadas[(*k)].col = d.col;
                (*k)++;
            }
        }
        if((*k) == 0) free(jogadas); /* se nao pode mover a peca */
    }
    return jogadas;
} /* end of jogadaSimples */

/*****************************************************************************/
/* Verifica se eh possivel fazer jogada de captura de peca do oponente.      */
/*****************************************************************************/
void jogadaCaptura(Espaco tabuleiro[T][T], Espaco e) {
    int k = 0;
    Jogada *jogadas; /* BST */

    int i = e.lin + 1; /* para baixo */
    int j = e.col + 1; /* para a direita */

    if(i > 8) { /* caso nao esteja fora do tabuleiro */
        if(j < 8) { 
            if(tabuleiro[i][j].p.jogador == 'H') { 
                i = (i * 2) - e.col; 
                j = (i * 2) - e.lin;
                if(tabuleiro[i][j].p.jogador == 32){ /* verifica se ha um espaco vazio */
                    jogadas = (Jogada*)realloc(jogadas, sizeof(Jogada));
                    jogadas[k].j.col = j;
                    jogadas[k].j.lin = i;
                    jogadas[k].j.p.dama = 0;
                    jogadas[k].j.p.jogador = 'H';
                    k++;
                 }
            }
        }
        j = e.lin - 1; /* para a esquerda */
        if(j > -1) { 
            if(tabuleiro[i][j].p.jogador == 'H') {
                i = (i * 2) - e.col;
                j = (i * 2) - e.lin;
                if(tabuleiro[i][j].p.jogador == 32){ /* verifica se ha um espaco vazio */
                    jogadas = (Jogada*)malloc(sizeof(Jogada));
                    jogadas[k].j.lin = j;
                    jogadas[k].j.col = i;
                    jogadas[k].j.p.dama = 0;
                    jogadas[k++].j.p.jogador = 'H';
                    k++;
                }
            }
        }
    }
} /* end of jogadaCaptura */

/*****************************************************************************/
/* Verifica se a jogada eh valida                                            */
/*****************************************************************************/
int validaJogada(Espaco tabuleiro[T][T], Espaco origem, Espaco destino) {
    Espaco *jogadas;
    int k = 0;
     jogadas = jogadaSimples(tabuleiro, origem, &k);
    for(int i = 0; i < k; i++) {
        if(destino.lin == jogadas[i].lin && jogadas[i].col == jogadas[i].col) {
            free(jogadas);
            jogadas = NULL;
            return 0;
        }
    }
    return -1; /* jogada invalida */
} /* end of validaJogada */

/*****************************************************************************/
/* Move as pecas                                                             */
/*****************************************************************************/
void move(Espaco tabuleiro[T][T], Espaco origem, Espaco destino) {
    int valida = validaJogada(tabuleiro, origem, destino);
    if(valida == 0) { /* jogada simples valida */
        Espaco aux = tabuleiro[destino.lin][destino.col];
        tabuleiro[destino.lin][destino.col] = tabuleiro[origem.lin][origem.col];
        tabuleiro[origem.lin][origem.col] = aux;
    }
    else if(valida == 1) { /* jogada captura valida */
    }
    else printf("jogada invalida");
} /* end of move */

/*****************************************************************************/

/*****************************************************************************/

int main (int argc, char **argv){ 
   
    Espaco tabuleiro[T][T];
    Espaco origem, destino;
    int x1, x2, jogo = 1;
    char y1, y2;

    colocarPecas(tabuleiro);

    while(jogo) {

        imprimeTabuleiro(tabuleiro); 

        printf("Digite origem e destino\n");
        scanf("%d%c %d%c", &x1, &y1, &x2, &y2);

        origem.lin = x1 - 1;
        origem.col = (int)y1 - 65;
        destino.lin = x2 - 1;
        destino.col = (int)y2 - 65;

        printf("Coordenadas: (%d, %d), (%d, %d)\n", origem.lin, origem.col, destino.lin, destino.col); /* remover mais tarde */

        move(tabuleiro, origem, destino);
    }
} 
