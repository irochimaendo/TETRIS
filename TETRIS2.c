#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>
#include <ncurses.h>

#define TAMP 4
#define LINB 22
#define COLB 23
#define VELOCIDADE 50
typedef struct {
    char p1[TAMP][TAMP]; char p2[TAMP][TAMP]; char p3[TAMP][TAMP]; char p4[TAMP][TAMP];
    char p5[TAMP][TAMP]; char p6[TAMP][TAMP]; char p7[TAMP][TAMP];
} pecas;
typedef struct {
    char borda[LINB][COLB]; int corBorda[LINB][COLB]; int cor; 
} borda;
typedef struct {
        int posX; int posY;
} coord;
typedef struct {
    char peca1[TAMP][TAMP]; char peca2[TAMP][TAMP];
} peca_ap;
typedef struct {
    int nivel; int score; int alinhas; int vel;
} info;

void rotacionarPeca(peca_ap *peca, borda *bordaJogo);
int sorteioPeca(int *inicial, int *cor2, peca_ap *peca, pecas *pecas_tetris);
int checarColisao(borda *bordaJogo, peca_ap *peca, coord *coord_);
void defPeca(pecas *pecas_tetris);
void lixo(borda *bordaJogo);
int mostra(borda *bordaJogo, info *jogoInfo);
void defBorda(borda *bordaJogo);
void moverPeca(borda *bordaJogo, peca_ap *peca, info *jogoInfo);
void linhaCompleta(borda *bordaJogo, info *jogoInfo);
void removerLinha(int end, borda *bordaJogo);
void inic_ncurses();
void proximaPeca(peca_ap *peca, int cor2);
void limpa_proximaPeca();
void borda_proximaPeca();

int main() {
    inic_ncurses();
    srand(time(NULL));
    setlocale(LC_ALL, "Portuguese");
    borda bordaJogo;
    pecas pecas_tetris;
    peca_ap peca;
    info jogoInfo = {1, 0, 0, 200000};
    int cor2;
    lixo(&bordaJogo);
    defBorda(&bordaJogo);
    defPeca(&pecas_tetris);
    int perd = 1, inicial = 0;
    while (perd) {
        borda_proximaPeca();
        linhaCompleta(&bordaJogo, &jogoInfo);
        bordaJogo.cor = sorteioPeca(&inicial, &cor2, &peca, &pecas_tetris);
        proximaPeca(&peca, cor2);
        mostra(&bordaJogo, &jogoInfo);
        moverPeca(&bordaJogo, &peca, &jogoInfo);
        perd = mostra(&bordaJogo, &jogoInfo);
        limpa_proximaPeca(&peca);
    }
    usleep(2000000);
    endwin();
    return 0;
}

void defBorda(borda *bordaJogo) {
    for (int i = 0; i < LINB; i++) {
        for (int j = 0; j < COLB; j++) {
            if (i == 0 || i == LINB - 1)
                bordaJogo->borda[i][j] = '-';
            else if (j == 0 || j == COLB - 1)
                bordaJogo->borda[i][j] = '|';
            else
                bordaJogo->borda[i][j] = ' ';
        }
    }
}

int mostra(borda *bordaJogo, info *jogoInfo) {
    for (int j = 1; j <= COLB-1; j++) {
        if (bordaJogo->borda[1][j] == '#')
            return 0;
    }
    for (int i = 0; i < LINB; i++) {
        for (int j = 0; j < COLB; j++) {
            if (bordaJogo->borda[i][j] == '#') {
                attron(COLOR_PAIR(bordaJogo->corBorda[i][j]));
                addch(ACS_CKBOARD);
                attroff(COLOR_PAIR(bordaJogo->corBorda[i][j]));
            } 
            else if (bordaJogo->borda[i][j] == '|' || bordaJogo->borda[i][j] == '-') {
                attron(COLOR_PAIR(8));
                mvaddch(i + 3, j + 40, ACS_CKBOARD);
                attroff(COLOR_PAIR(8));
            }
            else
                addch(bordaJogo->borda[i][j]);
        }
        printw("\n");
    }
    printw("\nscore: %d\n", jogoInfo->score);
    printw("linhas apagadas:%d\n",jogoInfo->alinhas);
    printw("velocidade: %d\n", jogoInfo->vel);
    printw("nivel: %d\n", jogoInfo->nivel);
    return 1;
}

void lixo(borda *bordaJogo) {
    for (int i = 0; i < LINB; i++) {
        for (int j = 0; j < COLB; j++) {
            bordaJogo->borda[i][j] = ' ';
            bordaJogo->corBorda[i][j] = 0;
        }
    }
}

void defPeca(pecas *pecas_tetris) {
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j < TAMP; j++) {
            pecas_tetris->p1[i][j] = ' ';
            pecas_tetris->p2[i][j] = ' ';
            pecas_tetris->p3[i][j] = ' ';
            pecas_tetris->p4[i][j] = ' ';
            pecas_tetris->p5[i][j] = ' ';
            pecas_tetris->p6[i][j] = ' ';
            pecas_tetris->p7[i][j] = ' ';
        }
    }
    pecas_tetris->p1[2][0] = pecas_tetris->p1[2][1] = pecas_tetris->p1[2][2] = pecas_tetris->p1[2][3] = '#';
    pecas_tetris->p2[0][0] = pecas_tetris->p2[1][0] = pecas_tetris->p2[1][1] = pecas_tetris->p2[1][2] = '#';
    pecas_tetris->p3[1][0] = pecas_tetris->p3[1][1] = pecas_tetris->p3[1][2] = pecas_tetris->p3[0][2] = '#';
    pecas_tetris->p4[0][0] = pecas_tetris->p4[1][0] = pecas_tetris->p4[0][1] = pecas_tetris->p4[1][1] = '#';
    pecas_tetris->p5[0][1] = pecas_tetris->p5[1][1] = pecas_tetris->p5[0][2] = pecas_tetris->p5[1][0] = '#';
    pecas_tetris->p6[1][0] = pecas_tetris->p6[1][1] = pecas_tetris->p6[1][2] = pecas_tetris->p6[0][1] = '#';
    pecas_tetris->p7[0][1] = pecas_tetris->p7[0][0] = pecas_tetris->p7[1][1] = pecas_tetris->p7[1][2] = '#';
}

int sorteioPeca(int *inicial, int *cor2, peca_ap *peca, pecas *pecas_tetris) {
    static int k = 0, k2 = 0;
    if (!(*inicial)) {
        k = 1 + rand() % 7;
        k2 = 1 + rand() % 7;
        *inicial = 1;
    }
    else {
        k = k2;
        k2 = 1 + rand() % 7;
    }
    int cor, corProx;
    char (*pecaEscolhida)[TAMP] = NULL;
    switch (k) {
        case 1: pecaEscolhida = pecas_tetris->p1; cor = 1; break;  
        case 2: pecaEscolhida = pecas_tetris->p2; cor = 3; break;  
        case 3: pecaEscolhida = pecas_tetris->p3; cor = 4; break;  
        case 4: pecaEscolhida = pecas_tetris->p4; cor = 2; break;  
        case 5: pecaEscolhida = pecas_tetris->p5; cor = 5; break;  
        case 6: pecaEscolhida = pecas_tetris->p6; cor = 6; break;  
        case 7: pecaEscolhida = pecas_tetris->p7; cor = 7; break;  
    }
    if (pecaEscolhida != NULL) {
        for (int i = 0; i < TAMP; i++) {
            for (int j = 0; j < TAMP; j++)
                peca->peca1[i][j] = pecaEscolhida[i][j];
        }
    }
    k = k2;
    char (*proxPeca)[TAMP] = NULL;
    switch (k2) {
        case 1: proxPeca = pecas_tetris->p1; corProx = 1; break;  
        case 2: proxPeca = pecas_tetris->p2; corProx = 3; break;  
        case 3: proxPeca = pecas_tetris->p3; corProx = 4; break;  
        case 4: proxPeca = pecas_tetris->p4; corProx = 2; break;  
        case 5: proxPeca = pecas_tetris->p5; corProx = 5; break;  
        case 6: proxPeca = pecas_tetris->p6; corProx = 6; break;  
        case 7: proxPeca = pecas_tetris->p7; corProx = 7; break;  
    }
    *cor2 = corProx;
    if (proxPeca != NULL) {
        for (int i = 0; i < TAMP; i++) {
            for (int j = 0; j < TAMP; j++)
                peca->peca2[i][j] = proxPeca[i][j];
        }
    }
    return cor;
}

int checarColisao(borda *bordaJogo, peca_ap *peca, coord *coord_temp) {
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j < TAMP; j++) {
                if (peca->peca1[i][j] == '#' && (coord_temp->posX + i >= LINB - 1 || coord_temp->posX + i < 1 || coord_temp->posY + j >= COLB - 1 || coord_temp->posY + j < 1 || bordaJogo->borda[coord_temp->posX + i][coord_temp->posY + j] == '#'))
                    return 1;
        }
    }
    return 0;
}

void moverPeca(borda *bordaJogo, peca_ap *peca, info *jogoInfo) {
    coord coord_, coord_temp;
    coord_.posX = 1; coord_.posY = (COLB / 2) - 1;
    int colidiu, tempo = 0;
    while (1) {
        int teclou = 0;
        for (int i = 0; i < TAMP; i++) {
            for (int j = 0; j < TAMP; j++) {
                    if (peca->peca1[i][j] == '#' && coord_.posX + i < LINB && coord_.posY + j < COLB && coord_.posX + i >= 0 && coord_.posY + j >= 0)
                        bordaJogo->borda[coord_.posX + i][coord_.posY + j] = ' ';
            }
        }
        int tecla = getch(); 
        switch (tecla) {
            case 's': case 'S': case KEY_DOWN:
            coord_temp = (coord){coord_.posX + 1, coord_.posY};
                if (!checarColisao(bordaJogo, peca, &coord_temp)) {
                    coord_.posX = coord_temp.posX;
                    teclou = 1;
                }
                break;
            case 'a':  case 'A': case KEY_LEFT:
            coord_temp = (coord){coord_.posX, coord_.posY - 1};
                if (!checarColisao(bordaJogo, peca, &coord_temp)) {
                    coord_.posY = coord_temp.posY;
                    teclou = 1;
                }
                break;
            case 'd': case 'D': case KEY_RIGHT:
            coord_temp = (coord){coord_.posX, coord_.posY + 1};
                if (!checarColisao(bordaJogo, peca, &coord_temp)) {
                    coord_.posY = coord_temp.posY;
                    teclou = 1;
                }
                break;
            case 'r': case 'R':
                rotacionarPeca(peca, bordaJogo);
                if (checarColisao(bordaJogo, peca, &coord_) || bordaJogo->cor == 2) {
                    for (int i = 0; i < 3; i++)
                        rotacionarPeca(peca, bordaJogo);
                }
                teclou = 1;
                break;
            case 'x': case 'X':
                while (1) {
                    coord_temp = (coord){coord_.posX + 1, coord_.posY};
                    if (checarColisao(bordaJogo, peca, &coord_temp))
                        break;
                    coord_.posX = coord_temp.posX;
                }
                teclou = 1;
                break;
            case ' ':
                while (1) {
                    if (tecla == getch())
                        break;
                }
                teclou = 1;
                break;
        }
        if (tempo >= VELOCIDADE) {
            coord_.posX++;
            colidiu = checarColisao(bordaJogo, peca, &coord_);
            if (colidiu) {
                coord_.posX--;
                for (int i = 0; i < TAMP; i++) {
                    for (int j = 0; j < TAMP; j++) {
                        if (peca->peca1[i][j] == '#' && coord_.posX + i < LINB && coord_.posY + j < COLB && coord_.posX + i >= 0 && coord_.posY + j >= 0) {
                            bordaJogo->borda[coord_.posX + i][coord_.posY + j] = peca->peca1[i][j];
                            bordaJogo->corBorda[coord_.posX + i][coord_.posY + j] = bordaJogo->cor;
                            }
                    }
                }
                break;
            }
            tempo = 0;
        } else {
            tempo += 10;
        }
        for (int i = 0; i < TAMP; i++) {
            for (int j = 0; j < TAMP; j++) {
                if (peca->peca1[i][j] == '#' && peca->peca1[i][j] == '#' && coord_.posX + i < LINB && coord_.posY + j < COLB && coord_.posX + i >= 0 && coord_.posY + j >= 0) {
                    bordaJogo->borda[coord_.posX + i][coord_.posY + j] = peca->peca1[i][j];
                    bordaJogo->corBorda[coord_.posX + i][coord_.posY + j] = bordaJogo->cor;
                }
            }
        }
        if (teclou || colidiu)
            refresh();
        else
            usleep(jogoInfo->vel);
        mostra(bordaJogo, jogoInfo);
    }
}

void rotacionarPeca(peca_ap *peca, borda *bordaJogo) {
    char temp[TAMP][TAMP];
    int pont = (TAMP / 2) - 1, x, y;
    if (bordaJogo->cor != 1) {
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j < TAMP; j++) {
            x = i - pont;
            y = j - pont;
            temp[pont + y][pont - x] = peca->peca1[i][j];
        }
    }
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j < TAMP; j++)
            peca->peca1[i][j] = temp[i][j];
    }
    }
    else {
        if (peca->peca1[0][1] == '#' && peca->peca1[1][1] == '#' && peca->peca1[2][1] == '#' && peca->peca1[3][1] == '#') {
            for (int i = 0; i < TAMP; i++) {
                for (int j = 0; j < TAMP; j++)
                    peca->peca1[i][j] = ' ';
            }
            peca->peca1[2][0] = '#'; peca->peca1[2][1] = '#', peca->peca1[2][2] = '#'; peca->peca1[2][3] = '#';
        }
        else if (peca->peca1[2][0] == '#' && peca->peca1[2][1] == '#' && peca->peca1[2][2] == '#' && peca->peca1[2][3] == '#') {
            for (int i = 0; i < TAMP; i++) {
                for (int j = 0; j < TAMP; j++)
                    peca->peca1[i][j] = ' ';
            }
            peca->peca1[0][2] = '#'; peca->peca1[1][2] = '#'; peca->peca1[2][2] = '#'; peca->peca1[3][2] = '#';
        }
        else if (peca->peca1[0][2] == '#' && peca->peca1[1][2] == '#' && peca->peca1[2][2] == '#' && peca->peca1[3][2] == '#') {
            for (int i = 0; i < TAMP; i++) {
                for (int j = 0; j < TAMP; j++)
                    peca->peca1[i][j] = ' ';
            }
            peca->peca1[1][0] = '#'; peca->peca1[1][1] = '#'; peca->peca1[1][2] = '#'; peca->peca1[1][3] = '#';
        }
        else if (peca->peca1[1][0] == '#' && peca->peca1[1][1] == '#' && peca->peca1[1][2] == '#' && peca->peca1[1][3] == '#') {
            for (int i = 0; i < TAMP; i++) {
                for (int j = 0; j < TAMP; j++)
                    peca->peca1[i][j] = ' ';
            }
            peca->peca1[0][1] = '#'; peca->peca1[1][1] = '#'; peca->peca1[2][1] = '#'; peca->peca1[3][1] = '#';
        }
    }
}

void linhaCompleta(borda *bordaJogo, info *jogoInfo) {
    int clinhas = 0;
    for (int i = 1; i < LINB - 1; i++) {
        int val = 1;
        for (int j = 1; j < COLB - 1; j++) {
            if (bordaJogo->borda[i][j] == ' ') {
                val = 0;
                break;
            }
        }
        if (val) {
            clinhas++;
            removerLinha(i, bordaJogo);
            i--;
        }
    }
        switch (clinhas) {
	        case 1: jogoInfo->score += (clinhas*100);     jogoInfo->alinhas+=(clinhas);  break;
	        case 2: jogoInfo->score += (clinhas*100)+100; jogoInfo->alinhas+=(clinhas);  break;
	        case 3: jogoInfo->score += (clinhas*100)+200; jogoInfo->alinhas+=(clinhas);  break;
	        case 4: jogoInfo->score += (clinhas*100)+400; jogoInfo->alinhas+=(clinhas);  break;
        }
        if (jogoInfo->alinhas >= jogoInfo->nivel*10 && jogoInfo->vel > 20000) {
            if (jogoInfo->vel > 20000) {
                jogoInfo->nivel +=1;
                jogoInfo->vel -=20000;
            }
        }
}

void removerLinha(int end, borda *bordaJogo) {
    for (int i = end; i > 1; i--) {
        for (int j = 1; j < COLB - 1; j++) {
            bordaJogo->borda[i][j] = bordaJogo->borda[i - 1][j];
            bordaJogo->corBorda[i][j] = bordaJogo->corBorda[i -1][j];
        }
    }
}    

void inic_ncurses() {
    initscr();
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_CYAN);
    init_pair(2, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(3, COLOR_BLUE, COLOR_BLUE);
    init_pair(4, 208, 208);
    init_pair(5, 82, 82);
    init_pair(6, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(7, COLOR_RED, COLOR_RED);
    init_pair(8, 240, 240);
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
}

void proximaPeca(peca_ap *peca, int cor2) {
    coord pos;
    pos.posY = (cor2 == 1) ? 4 : 5;
    pos.posX = (cor2 == 2) ? 16 : 15;
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j< TAMP; j++) {
            if (peca->peca2[i][j] == '#') {
                attron(COLOR_PAIR(cor2));
                mvaddch(i + pos.posY, j + pos.posX, ACS_CKBOARD);
                attroff(COLOR_PAIR(cor2));
            }
        }
    }
    refresh();
}

void limpa_proximaPeca() {
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j< TAMP; j++)
                mvaddch(i + 4, j + 15, ' ');
    }
}

void borda_proximaPeca() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            if (!i || !j || i == 7 || j == 11) {
                attron(COLOR_PAIR(8));
                mvaddch(i + 2, j + 11, ACS_CKBOARD);
                attroff(COLOR_PAIR(8));
            }
        }
    }
}
