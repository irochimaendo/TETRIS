#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>

// Dimensões da matriz onde os tetraminós vão ser empilhados.
#define GAME_W 15
#define GAME_H 18

#define TAMP 4
#define LINB 19
#define COLB 16
#define VELOCIDADE 50

// Usado por fgets() e mvaddstr()
#define TEXT_LENGTH 256

// Caminhos para os arquivos usados neste programa.
#define MENU_PATH "./assets/tetris-menu.txt"
#define INSTR_PATH "./assets/instrucoes.txt"
#define CRED_PATH "./assets/creditos.txt"
#define GAMEUI_PATH "./assets/game-ui.txt"

// Define a cor laranja e as variantes em negrito, para não usar as cores brilhantes
// do terminal no texto em negrito e manter a consistência ao longo do programa.
#define COLOR_ORANGE 8
#define COLOR_RED_BOLD 9
#define COLOR_GREEN_BOLD 10
#define COLOR_YELLOW_BOLD 11
#define COLOR_BLUE_BOLD 12
#define COLOR_MAGENTA_BOLD 13
#define COLOR_CYAN_BOLD 14
#define COLOR_ORANGE_BOLD 15

// Estrutura usada para armazenar algumas posições de referência em relação à origem da tela.
typedef struct {
	int x;
	int y;
} coord_t;
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
//Protótipos:
void initColors();
int calculateOffset(FILE *fp, coord_t *center, coord_t *offset);
void colorMainMenu(coord_t *offset);
void colorInstructions(coord_t *offset);
void colorGameUI(coord_t *offset);
void fillGameUiInfo(borda *bordaJogo, peca_ap *peca, info *jogoInfo, coord_t *offset, int *cor2);
void printFileCentered(FILE *fp, coord_t *offset);
void loopJogo(FILE *gameUI, coord_t *center, coord_t *offset);
void rotacionarPeca(peca_ap *peca, borda *bordaJogo);
int sorteioPeca(int *inicial, int *cor2, peca_ap *peca, pecas *pecas_tetris);
int checarColisao(borda *bordaJogo, peca_ap *peca, coord *coord_);
void defPeca(pecas *pecas_tetris);
void lixo(borda *bordaJogo);
int mostra(borda *bordaJogo, info *jogoInfo);
void defBorda(borda *bordaJogo);
void moverPeca(borda *bordaJogo, peca_ap *peca, info *jogoInfo, FILE *gameUI, coord_t *center, coord_t *offset, int *cor2, char *ch);
void linhaCompleta(borda *bordaJogo, info *jogoInfo);
void removerLinha(int end, borda *bordaJogo);
void inic_ncurses();
void proximaPeca(peca_ap *peca, int cor2);
void limpa_proximaPeca();
void borda_proximaPeca();

int main() {
	char select;					   // Variável para o seletor do menu
	FILE *tetrisMenu, *instrucoes, *creditos, *gameUI; // Arquivos a serem usados
	coord_t scrCenter, cursOffset;			   // Variáveis para calcular o centro da tela
	setlocale(LC_ALL, "");				   // Define o locale do programa para o locale do sistema.

	// Verifica se não houve erros ao abrir os arquivos das telas de título,
	// de instruções ou de créditos
	if((tetrisMenu = fopen(MENU_PATH, "r")) == NULL) {
		printf("Erro ao abrir o arquivo do menu. Saindo...\n");
		return 1;
	}
	if((instrucoes = fopen(INSTR_PATH, "r")) == NULL) {
		printf("Erro ao abrir o arquivo de instruções. Saindo...\n");
		return 1;
	}
	if((creditos = fopen(CRED_PATH, "r")) == NULL) {
		printf("Erro ao abrir o arquivo de créditos. Saindo...\n");
		return 1;
	}
	if((gameUI = fopen(GAMEUI_PATH, "r")) == NULL) {
		printf("Erro ao abrir o arquivo da interface do jogo. Saindo...\n");
		return 1;
	}

	// Inicializa a janela ncurses com alguns parâmetros
	initscr(); noecho(); curs_set(0); start_color(); 
	cbreak(); keypad(stdscr, TRUE); nodelay(stdscr, TRUE);

	// Verifica se o terminal tem suporte a cores e a redefinição de cores.
	// Ambos os recursos são usados para gerar e mostrar as cores personalizadas usadas aqui.
	if(!(has_colors() && can_change_color())) {
		endwin();
		fclose(tetrisMenu); fclose(instrucoes); fclose(creditos); fclose(gameUI);
		printf("Este terminal não possui suporte de cores adequado. Saindo...\n");
		return 1;
	}

	// Define as cores e os pares de cores que serão usados ao longo do programa
	initColors();

	do {
		// Mostra o menu principal centralizado na tela
		if(calculateOffset(tetrisMenu, &scrCenter, &cursOffset)) {
			printFileCentered(tetrisMenu, &cursOffset);
			colorMainMenu(&cursOffset);
		}
		refresh();

		// Seletor do menu principal
		select = getch();
		select = tolower(select);
		switch(select) {
			case 'j':
				// Lógica principal do jogo:
				loopJogo(gameUI, &scrCenter, &cursOffset);
				break;
			case 'i':
				do { // Tela de instruções:
					if(calculateOffset(instrucoes, &scrCenter, &cursOffset)) {
						printFileCentered(instrucoes, &cursOffset);
						colorInstructions(&cursOffset);
					}
					refresh();
					select = getch();
				} while(select != 'q');
				select = 0;
				break;
			case 'c':
				do { // Tela de créditos:
					if(calculateOffset(creditos, &scrCenter, &cursOffset))
						printFileCentered(creditos, &cursOffset);
					refresh();
					select = getch();
				} while(select != 'q');
				select = 0;
				break;
		}
	} while(select != 'q');	// Sai do loop quando o usuário pressionar 'q'

	// Fecha todos os arquivos antes de fechar o programa (importante!)
	fclose(tetrisMenu);
	fclose(instrucoes);
	fclose(creditos);

	// Finaliza a janela ncurses
	endwin();

	return 0;
}

void initColors() {

	/*
	 * Essa função configura todas as cores que o programa irá utilizar, além de definir
	 * a cor laranja e as cores de texto em negrito, que não são definidos por padrão
	 * pela <curses.h>.
	 *
	 * Como esses valores são personalizados, eles não são afetados pelo
	 * esquema de cores do terminal atual.
	 */

	// Cores personalizadas:
	init_color(COLOR_BLACK, 0, 0, 0);
	init_color(COLOR_WHITE, 1000, 1000, 1000);
	init_color(COLOR_RED, 850, 200, 200);
	init_color(COLOR_ORANGE, 900, 450, 70);
	init_color(COLOR_YELLOW, 800, 800, 250);
	init_color(COLOR_GREEN, 150, 730, 150);
	init_color(COLOR_CYAN, 150, 800, 800);
	init_color(COLOR_BLUE, 200, 200, 900);
	init_color(COLOR_MAGENTA, 700, 180, 800);
	init_color(COLOR_RED_BOLD, 850, 200, 200);
	init_color(COLOR_ORANGE_BOLD, 900, 450, 70);
	init_color(COLOR_YELLOW_BOLD, 800, 800, 250);
	init_color(COLOR_GREEN_BOLD, 150, 730, 150);
	init_color(COLOR_CYAN_BOLD, 150, 800, 800);
	init_color(COLOR_BLUE_BOLD, 200, 200, 900);
	init_color(COLOR_MAGENTA_BOLD, 700, 180, 800);

	// Pares de cor para os blocos:
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(2, COLOR_BLACK, COLOR_ORANGE);
	init_pair(3, COLOR_BLACK, COLOR_YELLOW);
	init_pair(4, COLOR_BLACK, COLOR_GREEN);
	init_pair(5, COLOR_BLACK, COLOR_CYAN);
	init_pair(6, COLOR_BLACK, COLOR_BLUE);
	init_pair(7, COLOR_BLACK, COLOR_MAGENTA);

	// Pares de cor de texto normal:
	init_pair(8, COLOR_RED, COLOR_BLACK);
	init_pair(9, COLOR_ORANGE, COLOR_BLACK);
	init_pair(10, COLOR_YELLOW, COLOR_BLACK);
	init_pair(11, COLOR_GREEN, COLOR_BLACK);
	init_pair(12, COLOR_CYAN, COLOR_BLACK);
	init_pair(13, COLOR_BLUE, COLOR_BLACK);
	init_pair(14, COLOR_MAGENTA, COLOR_BLACK);

	// Pares de cor de texto em negrito:
	init_pair(15, COLOR_RED_BOLD, COLOR_BLACK);
	init_pair(16, COLOR_ORANGE_BOLD, COLOR_BLACK);
	init_pair(17, COLOR_YELLOW_BOLD, COLOR_BLACK);
	init_pair(18, COLOR_GREEN_BOLD, COLOR_BLACK);
	init_pair(19, COLOR_CYAN_BOLD, COLOR_BLACK);
	init_pair(20, COLOR_BLUE_BOLD, COLOR_BLACK);
	init_pair(21, COLOR_MAGENTA_BOLD, COLOR_BLACK);
}

int calculateOffset(FILE *fp, coord_t *center, coord_t *offset) {

	/* Essa função calcula a posição do cursor para que o texto exibido fique centralizado. */

	char str[TEXT_LENGTH];
	int largura = 0, altura = 0, l;			// 'l' é uma variável auxiliar.
	coord_t prevOffset = *offset;			// Guardar a posição anterior do cursor.
	center->x = COLS / 2; center->y = LINES / 2;	// Armazena as coordenadas do centro da tela para mais tarde.

	while(fgets(str, TEXT_LENGTH, fp) == str) {
		l = mbstowcs(NULL, str, TEXT_LENGTH);
		largura = l > largura ? l : largura;
		++altura;
	}
	rewind(fp);

	/* 
	 * Atribui a posição do cursor para que o texto exibido fique centralizado.
	 * Também impede que essa posição tenha um valor negativo, para que o cursor
	 * não fique fora dos limites da tela devido a um underflow.
	 */

	offset->x = center->x - (largura / 2); offset->y = center->y - (altura / 2);
	if(offset->x < 0) offset->x = 0;
	if(offset->y < 0) offset->y = 0;

	// Esse resultado é usado para determinar se a tela precisa ser redesenhada.
	// Ajuda a reduzir as piscadas na tela.
	if((offset->x != prevOffset.x) || (offset->y != prevOffset.y)) return 1;
	else return 0;
}
void printFileCentered(FILE *fp, coord_t *offset) {

	/*
	 * Essa função imprime o texto de um arquivo. O texto é centralizado dinamicamente
	 * de acordo com os resultados de calculateOffset().
	 */

	char str[TEXT_LENGTH];

	clear();
	for(int i = 0; fgets(str, TEXT_LENGTH, fp) == str; ++i)
		mvaddstr(offset->y + i, offset->x, str);
	rewind(fp);
}

void colorMainMenu(coord_t *offset) {

	/*
	 * Essa função adiciona cores em vários elementos do menu principal, mais
	 * especificamente o título "TETRIS" e as opções abaixo dele.
	 */

	// Colorir cada letra linha a linha. Como as letras possuem larguras
	// diferentes, não dá para fazer uma estrutura de repetição eficiente.
	for(int i = 0; i < 6; ++i) {
		mvchgat(offset->y + 2 + i, offset->x + 3, 9, WA_NORMAL, 8, NULL);
		mvchgat(offset->y + 2 + i, offset->x + 12, 8, WA_NORMAL, 9, NULL);
		mvchgat(offset->y + 2 + i, offset->x + 20, 9, WA_NORMAL, 10, NULL);
		mvchgat(offset->y + 2 + i, offset->x + 29, 8, WA_NORMAL, 11, NULL);
		mvchgat(offset->y + 2 + i, offset->x + 37, 3, WA_NORMAL, 12, NULL);
		mvchgat(offset->y + 2 + i, offset->x + 40, 8, WA_NORMAL, 14, NULL);
	}

	// Colorir as letras de cada opção.
	mvchgat(offset->y + 12, offset->x + 16, 2, WA_BOLD, 18, NULL);
	mvchgat(offset->y + 13, offset->x + 16, 2, WA_BOLD, 19, NULL);
	mvchgat(offset->y + 14, offset->x + 16, 2, WA_BOLD, 21, NULL);
	mvchgat(offset->y + 15, offset->x + 16, 2, WA_BOLD, 15, NULL);
}

void colorInstructions(coord_t *offset) {

	/* O mesmo que colorMainMenu(), mas para a tela de instruções. */

	// Colorir os títulos de cada tópico e o 'q' no prompt na parte inferior
	mvchgat(offset->y + 2, offset->x + 3, 13, WA_BOLD, 17, NULL);
	mvchgat(offset->y + 9, offset->x + 3, 10, WA_BOLD, 18, NULL);
	mvchgat(offset->y + 21, offset->x + 32, 3, WA_BOLD, 21, NULL);
}

void colorGameUI(coord_t *offset) {

	/* O mesmo que colorInstructions(), mas para a interface do jogo. */

	mvchgat(offset->y, offset->x, 46, WA_DIM, 0, NULL);
	for(int i = 1; i < 23; ++i) {
		mvchgat(offset->y + i, offset->x, 1, WA_DIM, 0, NULL);
		mvchgat(offset->y + i, offset->x + 45, 1, WA_DIM, 0, NULL);
	}
	mvchgat(offset->y + 23, offset->x, 46, WA_DIM, 0, NULL);

	// Colorir a logo do Tetris na interface do jogo:
	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 4; ++j)
			mvchgat(offset->y + 3 + i, offset->x + 23 + 3*j, 3, WA_NORMAL, j + 8, NULL);
		mvchgat(offset->y + 3 + i, offset->x + 35, 1, WA_NORMAL, 12, NULL);
		mvchgat(offset->y + 3 + i, offset->x + 36, 3, WA_NORMAL, 14, NULL);
	}

	// Colorir os outros indicadores (linhas eliminadas, nível, próxima peça)
	mvchgat(offset->y + 9, offset->x + 23, 5, WA_BOLD, 16, NULL);
	mvchgat(offset->y + 9, offset->x + 32, 6, WA_BOLD, 18, NULL);
	mvchgat(offset->y + 13, offset->x + 25, 12, WA_BOLD, 19, NULL);
}

void fillGameUiInfo(borda *bordaJogo, peca_ap *peca, info *jogoInfo, coord_t *offset, int *cor2) {
	/*
	 * Essa função lida com as informações presentes na interface (pontuação, linhas, próxima peça),
	 * além de exibir a matriz principal de peças.
	 */

	// Pontuação, nível e linhas:
	mvprintw(offset->y + 7, offset->x + 23, "%16d", jogoInfo->score);
	mvprintw(offset->y + 10, offset->x + 23, "%5d", jogoInfo->nivel);
	mvprintw(offset->y + 10, offset->x + 32, "%6d", jogoInfo->alinhas);

	// Próxima peça:
	for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j< TAMP; j++)
                mvaddch(offset->y + 16 + i, offset->x + 29 + j, ' ');
    }
	for(int i = 0; i < TAMP; ++i) {
		for(int j = 0; j < TAMP; ++j) {
			if (peca->peca2[i][j] == '#') {
				int k = (*cor2 == 5) ? i-1 : i;
				int l = (*cor2 == 3) ? j+1 : j;
				attron(COLOR_PAIR(*cor2));
				mvaddch(offset->y + 17 + k, offset->x + 29 + l, ACS_CKBOARD);
				attroff(COLOR_PAIR(*cor2));
			}
		}
	}
	// Colorindo os blocos de acordo com os valores na matrizJogo
	for(int i = 0; i < GAME_H; ++i) {
		for(int j = 0; j < GAME_W; ++j) {
			if (bordaJogo->borda[i][j] == '#') {
				attron(COLOR_PAIR(bordaJogo->corBorda[i][j]));
				mvaddch(offset->y + 3 + i, offset->x + 6 + j, ACS_CKBOARD);
				attroff(COLOR_PAIR(bordaJogo->corBorda[i][j]));
			}
			else
				mvaddch(offset->y + 3 + i, offset->x + 6 + j, ' ');
		}
	}
}

void loopJogo(FILE *gameUI, coord_t *center, coord_t *offset) {

	srand(time(NULL));
	borda bordaJogo;
    pecas pecas_tetris;
    peca_ap peca;
    info jogoInfo = {1, 0, 0, 200000};
    int cor2; char ch;
    lixo(&bordaJogo);
    defBorda(&bordaJogo);
    defPeca(&pecas_tetris);
    int gameOver = 0, inicial = 0;
    while (!gameOver || ch == 'q') {
		refresh();
        linhaCompleta(&bordaJogo, &jogoInfo);
        bordaJogo.cor = sorteioPeca(&inicial, &cor2, &peca, &pecas_tetris);
		if(calculateOffset(gameUI, center, offset))
			printFileCentered(gameUI,  offset);
		colorGameUI(offset);
		fillGameUiInfo(&bordaJogo, &peca, &jogoInfo, offset, &cor2);
        mostra(&bordaJogo, &jogoInfo);
        moverPeca(&bordaJogo, &peca, &jogoInfo, gameUI, center, offset, &cor2, &ch);
        gameOver = mostra(&bordaJogo, &jogoInfo);
        limpa_proximaPeca(&peca);
    }
    endwin();
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
            return 1;
    }
    return 0;
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
        case 1: pecaEscolhida = pecas_tetris->p1; cor = 5; break;  
        case 2: pecaEscolhida = pecas_tetris->p2; cor = 6; break;  
        case 3: pecaEscolhida = pecas_tetris->p3; cor = 2; break;  
        case 4: pecaEscolhida = pecas_tetris->p4; cor = 3; break;  
        case 5: pecaEscolhida = pecas_tetris->p5; cor = 4; break;  
        case 6: pecaEscolhida = pecas_tetris->p6; cor = 7; break;  
        case 7: pecaEscolhida = pecas_tetris->p7; cor = 1; break;  
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
        case 1: proxPeca = pecas_tetris->p1; corProx = 5; break;  
        case 2: proxPeca = pecas_tetris->p2; corProx = 6; break;  
        case 3: proxPeca = pecas_tetris->p3; corProx = 2; break;  
        case 4: proxPeca = pecas_tetris->p4; corProx = 3; break;  
        case 5: proxPeca = pecas_tetris->p5; corProx = 4; break;  
        case 6: proxPeca = pecas_tetris->p6; corProx = 7; break;  
        case 7: proxPeca = pecas_tetris->p7; corProx = 1; break;  
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

void moverPeca(borda *bordaJogo, peca_ap *peca, info *jogoInfo, FILE *gameUI, coord_t *center, coord_t *offset, int *cor2, char *ch) {
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
                if (checarColisao(bordaJogo, peca, &coord_) || bordaJogo->cor == 3) {
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
			case 'q': case 'Q':
				*ch = tolower(tecla);
				break;
        }
		if (*ch == 'q')
			break;
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
        if (teclou || colidiu) {
            refresh();
		}
        else
            usleep(jogoInfo->vel);
		if(calculateOffset(gameUI, center, offset))
			printFileCentered(gameUI,  offset);
		colorGameUI(offset);
		fillGameUiInfo(bordaJogo, peca, jogoInfo, offset, cor2);
        mostra(bordaJogo, jogoInfo);
    }
}

void rotacionarPeca(peca_ap *peca, borda *bordaJogo) {
    char temp[TAMP][TAMP];
    int pont = (TAMP / 2) - 1, x, y;
    if (bordaJogo->cor != 5) {
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

void limpa_proximaPeca() {
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j< TAMP; j++)
                mvaddch(i + 4, j + 15, ' ');
    }
}
