#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>
#include <ncurses.h>
#define TAMP 4
#define COL 20
#define LIN 20
#define LINB 22
#define COLB 23
#define VELOCIDADE 50

void rotacionarPeca(char peca[TAMP][TAMP], int cor);
int sorteioPeca(int *inicial, int *cor2, char peca[TAMP][TAMP], char peca2[TAMP][TAMP], char p1[TAMP][TAMP], char p2[TAMP][TAMP], char p3[TAMP][TAMP], char p4[TAMP][TAMP], char p5[TAMP][TAMP], char p6[TAMP][TAMP], char p7[TAMP][TAMP]);
void defPeca(char p1[TAMP][TAMP], char p2[TAMP][TAMP], char p3[TAMP][TAMP], char p4[TAMP][TAMP], char p5[TAMP][TAMP], char p6[TAMP][TAMP], char p7[TAMP][TAMP]);
void lixo(char borda[LINB][COLB], int corBorda[LINB][COLB]);
int mostra(int corBorda[LINB][COLB], int cor, char borda[LINB][COLB], int perd, int* score,int* alinhas);
void defBorda(char borda[LINB][COLB]);
void moverPeca(int corBorda[LINB][COLB], int cor, char borda[LINB][COLB], char peca[TAMP][TAMP], int perd, int* score, int *alinhas);
void linhaCompleta(char borda[LINB][COLB], int* score, int* alinhas);
void removerLinha(int end, char borda[LINB][COLB]);
void inic_ncurses();
void proximaPeca(char peca2[TAMP][TAMP], int cor2);
void limpa_proximaPeca(char peca2[TAMP][TAMP]);
void borda_proximaPeca();

int main() {
    inic_ncurses();
    srand(time(NULL));
    setlocale(LC_ALL, "Portuguese");
    char borda[LINB][COLB], p1[TAMP][TAMP], p2[TAMP][TAMP], p3[TAMP][TAMP], p4[TAMP][TAMP], p5[TAMP][TAMP], p6[TAMP][TAMP], p7[TAMP][TAMP];
    char peca[TAMP][TAMP], peca2[TAMP][TAMP];
    int cor, cor2, corBorda[LINB][COLB];
    lixo(borda, corBorda);
    defBorda(borda);
    defPeca(p1, p2, p3, p4, p5, p6, p7);
    int perd = 1, inicial = 0;
    int score = 0, alinhas= 0;
    while (perd) {
        borda_proximaPeca();
        linhaCompleta(borda, &score, &alinhas);
        cor = sorteioPeca(&inicial, &cor2, peca, peca2, p1, p2, p3, p4, p5, p6, p7);
        proximaPeca(peca2, cor2);
        mostra(corBorda, cor, borda, perd, &score,&alinhas);
        moverPeca(corBorda, cor, borda, peca, perd, &score,&alinhas);
        perd = mostra(corBorda, cor, borda, perd, &score,&alinhas);
        limpa_proximaPeca(peca2);
    }
    usleep(2000000);
    endwin();
    return 0;
}

void defBorda(char borda[LINB][COLB]) {
    for (int i = 0; i < LINB; i++) {
        for (int j = 0; j < COLB; j++) {
            if (i == 0 || i == LINB - 1)
                borda[i][j] = '-';
            else if (j == 0 || j == COLB - 1)
                borda[i][j] = '|';
            else
                borda[i][j] = ' ';
        }
    }
}

int mostra(int corBorda[LINB][COLB], int cor, char borda[LINB][COLB], int perd, int* score,int* alinhas) {
    for (int j = 1; j <= COLB-1; j++) {
        if (borda[1][j] == '#')
            return 0;
    }
    for (int i = 0; i < LINB; i++) {
        for (int j = 0; j < COLB; j++) {
            if (borda[i][j] == '#') {
                attron(COLOR_PAIR(corBorda[i][j]));
                addch(ACS_CKBOARD);
                attroff(COLOR_PAIR(corBorda[i][j]));
            } else {
                if (borda[i][j] == '|' || borda[i][j] == '-') {
                attron(COLOR_PAIR(8));
                mvaddch(i + 3, j + 40, ACS_CKBOARD);
                attroff(COLOR_PAIR(8));
                }
                else
                printw("%c", borda[i][j]);
            }
        }
        printw("\n");
    }
    printw ("\nscore: %d\n", *score);
    printw ("linhas apagadas:%d\n",*alinhas);
    return 1;
}

void lixo(char borda[LINB][COLB], int corBorda[LINB][COLB]) {
    for (int i = 0; i < LINB; i++) {
        for (int j = 0; j < COLB; j++) {
            borda[i][j] = ' ';
            corBorda[i][j] = 0;
        }
    }
}

void defPeca(char p1[TAMP][TAMP], char p2[TAMP][TAMP], char p3[TAMP][TAMP], char p4[TAMP][TAMP], char p5[TAMP][TAMP], char p6[TAMP][TAMP], char p7[TAMP][TAMP]) {
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j < TAMP; j++) {
            p1[i][j] = ' ';
            p2[i][j] = ' ';
            p3[i][j] = ' ';
            p4[i][j] = ' ';
            p5[i][j] = ' ';
            p6[i][j] = ' ';
            p7[i][j] = ' ';
        }
    }
    p1[2][0] = p1[2][1] = p1[2][2] = p1[2][3] = '#';
    p2[0][0] = p2[1][0] = p2[1][1] = p2[1][2] = '#';
    p3[1][0] = p3[1][1] = p3[1][2] = p3[0][2] = '#';
    p4[0][0] = p4[1][0] = p4[0][1] = p4[1][1] = '#';
    p5[0][1] = p5[1][1] = p5[0][2] = p5[1][0] = '#';
    p6[1][0] = p6[1][1] = p6[1][2] = p6[0][1] = '#';
    p7[0][1] = p7[0][0] = p7[1][1] = p7[1][2] = '#';
}

int sorteioPeca(int *inicial, int *cor2, char peca[TAMP][TAMP], char peca2[TAMP][TAMP], char p1[TAMP][TAMP], char p2[TAMP][TAMP], char p3[TAMP][TAMP], char p4[TAMP][TAMP], char p5[TAMP][TAMP], char p6[TAMP][TAMP], char p7[TAMP][TAMP]) {
    static int k = 0, k2 = 0;
    if (*inicial == 0) {
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
        case 1: pecaEscolhida = p1; cor = 1; break;  
        case 2: pecaEscolhida = p2; cor = 3; break;  
        case 3: pecaEscolhida = p3; cor = 4; break;  
        case 4: pecaEscolhida = p4; cor = 2; break;  
        case 5: pecaEscolhida = p5; cor = 5; break;  
        case 6: pecaEscolhida = p6; cor = 6; break;  
        case 7: pecaEscolhida = p7; cor = 7; break;  
    }
    if (pecaEscolhida != NULL) {
        for (int i = 0; i < TAMP; i++) {
            for (int j = 0; j < TAMP; j++) {
                peca[i][j] = pecaEscolhida[i][j];
            }
        }
    }
    k = k2;
    char (*proxPeca)[TAMP] = NULL;
    switch (k2) {
        case 1: proxPeca = p1; corProx = 1; break;  
        case 2: proxPeca = p2; corProx = 3; break;  
        case 3: proxPeca = p3; corProx = 4; break;  
        case 4: proxPeca = p4; corProx = 2; break;  
        case 5: proxPeca = p5; corProx = 5; break;  
        case 6: proxPeca = p6; corProx = 6; break;  
        case 7: proxPeca = p7; corProx = 7; break;  
    }
    *cor2 = corProx;
    if (proxPeca != NULL) {
        for (int i = 0; i < TAMP; i++) {
            for (int j = 0; j < TAMP; j++) {
                peca2[i][j] = proxPeca[i][j];
            }
        }
    }
    return cor;
}

int checarColisao(char borda[LINB][COLB], char peca[TAMP][TAMP], int posX, int posY) {
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j < TAMP; j++) {
            if (peca[i][j] == '#') {
                if (posX + i >= LINB - 1 || posX + i < 1 || posY + j >= COLB - 1 || posY + j < 1 || borda[posX + i][posY + j] == '#') {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void moverPeca(int corBorda[LINB][COLB], int cor, char borda[LINB][COLB], char peca[TAMP][TAMP], int perd, int* score, int* alinhas) {
    int posX = 1;
    int posY = (COLB / 2) - 1;
    int colidiu;
    int tempo = 0;
    static int vel = 0;
    while (1) {
        int teclou = 0;
        for (int i = 0; i < TAMP; i++) {
            for (int j = 0; j < TAMP; j++) {
                if (peca[i][j] == '#') {
                    if (posX + i < LINB && posY + j < COLB && posX + i >= 0 && posY + j >= 0) {
                        borda[posX + i][posY + j] = ' ';
                    }
                }
            }
        }
        int tecla = getch();
            switch (tecla) {
            case 's': 
                if (!checarColisao(borda, peca, posX + 1, posY)) {
                    posX++;
                    teclou = 1;
                }
                break;
            case 'a': 
                if (!checarColisao(borda, peca, posX, posY - 1)) {
                    posY--;
                    teclou = 1;
                }
                break;
            case 'd': 
                if (!checarColisao(borda, peca, posX, posY + 1)) {
                    posY++;
                    teclou = 1;
                }
                break;
            case 'r': 
                rotacionarPeca(peca, cor);
                if (checarColisao(borda, peca, posX, posY) || cor == 2) {
                    rotacionarPeca(peca, cor);
                    rotacionarPeca(peca, cor);
                    rotacionarPeca(peca, cor);
                }
                teclou = 1;
                break;
        }
        if (tempo >= VELOCIDADE) {
            posX++;
            colidiu = checarColisao(borda, peca, posX, posY);
            if (colidiu) {
                posX--;
                for (int i = 0; i < TAMP; i++) {
                    for (int j = 0; j < TAMP; j++) {
                        if (peca[i][j] == '#') {
                            if (posX + i < LINB && posY + j < COLB && posX + i >= 0 && posY + j >= 0) {
                                borda[posX + i][posY + j] = peca[i][j];
                                corBorda[posX + i][posY + j] = cor;
                            }
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
                if (peca[i][j] == '#') {
                    if (posX + i < LINB && posY + j < COLB && posX + i >= 0 && posY + j >= 0) {
                        borda[posX + i][posY + j] = peca[i][j];
                        corBorda[posX + i][posY + j] = cor;
                    }
                }
            }
        }
        vel += 1;
        if (teclou)
            refresh();
        else if (colidiu)
            refresh();
        else
            usleep(120000 - vel);
        mostra(corBorda, cor, borda, perd, score, alinhas);
    }
}


void rotacionarPeca(char peca[TAMP][TAMP], int cor) {
    char temp[TAMP][TAMP];
    int pont = (TAMP / 2) - 1, x, y;
    if (cor != 1) {
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j < TAMP; j++) {
            x = i - pont;
            y = j - pont;
            temp[pont + y][pont - x] = peca[i][j];
        }
    }
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j < TAMP; j++) {
            peca[i][j] = temp[i][j];
        }
    }
    }
    else {
        if (peca[0][1] == '#' && peca[1][1] == '#' && peca[2][1] == '#' && peca[3][1] == '#') {
            for (int i = 0; i < TAMP; i++) {
                for (int j = 0; j < TAMP; j++) {
                    peca[i][j] = ' ';
                }
            }
            peca[2][0] = '#'; peca[2][1] = '#', peca[2][2] = '#'; peca[2][3] = '#';
        }
        else if (peca[2][0] == '#' && peca[2][1] == '#' && peca[2][2] == '#' && peca[2][3] == '#') {
            for (int i = 0; i < TAMP; i++) {
                for (int j = 0; j < TAMP; j++) {
                    peca[i][j] = ' ';
                }
            }
            peca[0][2] = '#'; peca[1][2] = '#'; peca[2][2] = '#'; peca[3][2] = '#';
        }
        else if (peca[0][2] == '#' && peca[1][2] == '#' && peca[2][2] == '#' && peca[3][2] == '#') {
            for (int i = 0; i < TAMP; i++) {
                for (int j = 0; j < TAMP; j++) {
                    peca[i][j] = ' ';
                }
            }
            peca[1][0] = '#'; peca[1][1] = '#'; peca[1][2] = '#'; peca[1][3] = '#';
        }
        else if (peca[1][0] == '#' && peca[1][1] == '#' && peca[1][2] == '#' && peca[1][3] == '#') {
            for (int i = 0; i < TAMP; i++) {
                for (int j = 0; j < TAMP; j++) {
                    peca[i][j] = ' ';
                }
            }
            peca[0][1] = '#'; peca[1][1] = '#'; peca[2][1] = '#'; peca[3][1] = '#';
        }
    }
}

void linhaCompleta(char borda[LINB][COLB], int* score,int* alinhas){
    int clinhas = 0;
    for (int i = 1; i < LINB - 1; i++) {
        int val = 1;
        for (int j = 1; j < COLB - 1; j++) {
            if (borda[i][j] == ' ') {
                val = 0;
                break;
            }
        }
        if (val) {
	    clinhas++;
            removerLinha(i, borda);
        }
    }
    switch (clinhas) {
	case 1: *score += (clinhas*100);     *alinhas+=(clinhas);  break;
	case 2: *score += (clinhas*100)+100; *alinhas+=(clinhas);  break;
	case 3: *score += (clinhas*100)+200; *alinhas+=(clinhas);  break;
	case 4: *score += (clinhas*100)+400; *alinhas+=(clinhas);  break;
    }	
}

void removerLinha(int end, char borda[LINB][COLB]) {
    for (int i = end; i > 1; i--) {
        for (int j = 1; j < COLB - 1; j++) {
            borda[i][j] = borda[i - 1][j];
        }
    }
    for (int j = 1; j < COLB - 1; j++) {
        borda[1][j] = ' ';
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
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
}

void proximaPeca(char peca2[TAMP][TAMP], int cor2) {
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j< TAMP; j++) {
            if (peca2[i][j] == '#') {
                attron(COLOR_PAIR(cor2));
                if (cor2 == 1)
                    mvaddch(i + 4, j + 15, ACS_CKBOARD);
                else if (cor2 == 2)
                    mvaddch(i + 5, j + 16, ACS_CKBOARD);
                else
                    mvaddch(i + 5, j + 15, ACS_CKBOARD);
                attroff(COLOR_PAIR(cor2));
            }
        }
    }
    refresh();
}

void limpa_proximaPeca(char peca2[TAMP][TAMP]) {
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j< TAMP; j++) {
                mvaddch(i + 4, j + 15, ' ');
        }
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
