#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>
#include <SDL.h>
#include <SDL_mixer.h>

#define PASS 0
#define FAIL 1

#define TAMP 4
#define LINB 22
#define COLB 12
#define VELOCIDADE 50

#define TEXT_LENGTH 256

// Caminhos para os arquivos usados pelo programa.
#define BGM_PATH "assets/tetris_bgm.mp3"
#define MENU_PATH "assets/tetris-menu.txt"
#define INSTR_PATH "assets/instrucoes.txt"
#define PLACAR_UI_PATH "assets/placar-ui.txt"
#define RANKING_PATH "assets/recordes.txt"
#define CRED_PATH "assets/creditos.txt"
#define GAMEUI_PATH "assets/game-ui.txt"
#define GAMEOVER_PATH "assets/game-over.txt"

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
	int nivel; int score; int alinhas; float tempo; float delay;
} info;
typedef struct {
	char nome[4];
	unsigned int pontuacao;
} ranking;

//Protótipos:
int fileCheck(FILE **tetrisMenu, FILE **instrucoes, FILE **placarUI, FILE **creditos, FILE **gameUI, FILE **gameOverUI, FILE **recordes);
int audioInit(Mix_Music **tetrisBgm);
void initNcurses();
int colorCheck();
void initColors();
bool calculateOffset(FILE *fp, coord_t *center, coord_t *offset);
void colorMainMenu(coord_t *offset);
void colorInstrucoes(coord_t *offset);
void colorPlacar(coord_t *offset);
void colorCreditos(coord_t *offset);
void preencherPlacar(FILE *recordes, coord_t *offset);
void colorGameUI(coord_t *offset);
void fillGameUiInfo(borda *bordaJogo, peca_ap *peca, info *jogoInfo, coord_t *offset, int *cor2);
void printFileCentered(FILE *fp, coord_t *offset);
void loopJogo(FILE *gameUI, FILE *gameOverUI, FILE *recordes, coord_t *center, coord_t *offset, Mix_Music *tetrisBgm);
void rotacionarPeca(peca_ap *peca, borda *bordaJogo);
int sorteioPeca(int *inicial, int *cor2, peca_ap *peca, pecas *pecas_tetris);
int checarColisao(borda *bordaJogo, peca_ap *peca, coord *coord_);
void defPeca(pecas *pecas_tetris);
void lixo(borda *bordaJogo);
int check(borda *bordaJogo, info *jogoInfo);
void defBorda(borda *bordaJogo);
void moverPeca(borda *bordaJogo, peca_ap *peca, info *jogoInfo, FILE *gameUI, coord_t *center, coord_t *offset, int *cor2, char *ch);
void linhaCompleta(borda *bordaJogo, info *jogoInfo);
void removerLinha(int end, borda *bordaJogo);
void proximaPeca(peca_ap *peca, int cor2);
void ajuste(peca_ap *peca, coord *ajustePeca);
void telaDeGameOver(FILE *gameOverUI, FILE *recordes, int pontuacao, coord_t *center, coord_t *offset);
void colorGameOver(coord_t *offset);

int main() {
	char menuSelect;
	FILE *tetrisMenu, *instrucoes, *placarUI, *creditos, *gameUI, *gameOverUI, *recordes;
	Mix_Music *tetrisBgm;
	coord_t scrCenter, cursOffset;
	setlocale(LC_ALL, "");

	if(audioInit(&tetrisBgm) == FAIL) {
		fprintf(stderr, "Erro ao inicializar o sistema de áudio. Saindo...\n");
		return EXIT_FAILURE;
	}
	if(fileCheck(&tetrisMenu, &instrucoes, &placarUI, &creditos, &gameUI, &gameOverUI, &recordes) == FAIL) {
		fprintf(stderr, "Ocorreu um erro ao abrir os arquivos. Saindo...\n");
		return EXIT_FAILURE;
	}

	initNcurses();
	if(colorCheck() == FAIL) {
		endwin();
		fclose(tetrisMenu); fclose(instrucoes); fclose(placarUI);
		fclose(creditos); fclose(gameUI); fclose(gameOverUI); fclose(recordes);
		fprintf(stderr, "Este terminal não possui suporte de cores adequado. Saindo...\n");
		return EXIT_FAILURE;
	}

	initColors();

	do {
		if(calculateOffset(tetrisMenu, &scrCenter, &cursOffset)) {
			printFileCentered(tetrisMenu, &cursOffset);
			colorMainMenu(&cursOffset);
		}
		refresh();

		menuSelect = tolower(getch());

		switch(menuSelect) {
			case 'j':
				loopJogo(gameUI, gameOverUI, recordes, &scrCenter, &cursOffset, tetrisBgm);
				break;

			case 'i':
				do {
					if(calculateOffset(instrucoes, &scrCenter, &cursOffset) == TRUE) {
						printFileCentered(instrucoes, &cursOffset);
						colorInstrucoes(&cursOffset);
					}
					refresh();
				} while(tolower(getch()) != 'q');
				break;

			case 'p':
				do {
					if(calculateOffset(placarUI, &scrCenter, &cursOffset) == TRUE) {
						printFileCentered(placarUI, &cursOffset);
						colorPlacar(&cursOffset);
						preencherPlacar(recordes, &cursOffset);
					}
					refresh();
				} while(tolower(getch()) != 'q');
				break;

			case 'c':
				do {
					if(calculateOffset(creditos, &scrCenter, &cursOffset) == TRUE) {
						printFileCentered(creditos, &cursOffset);
						colorCreditos(&cursOffset);
					}
					refresh();
				} while(tolower(getch()) != 'q');
				break;
		}
	} while(menuSelect != 'q');

	endwin();
	fclose(tetrisMenu); fclose(instrucoes); fclose(placarUI);
	fclose(creditos); fclose(gameUI); fclose(gameOverUI); fclose(recordes);

	Mix_FreeMusic(tetrisBgm);
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}

int fileCheck(FILE **tetrisMenu, FILE **instrucoes, FILE **placarUI, FILE **creditos, FILE **gameUI, FILE **gameOverUI, FILE **recordes) {

	if((*tetrisMenu = fopen(MENU_PATH, "r")) == NULL) return FAIL;
	if((*instrucoes = fopen(INSTR_PATH, "r")) == NULL) return FAIL;
	if((*placarUI = fopen(PLACAR_UI_PATH, "r")) == NULL) return FAIL;
	if((*creditos = fopen(CRED_PATH, "r")) == NULL) return FAIL;
	if((*gameUI = fopen(GAMEUI_PATH, "r")) == NULL) return FAIL;
	if((*gameOverUI = fopen(GAMEOVER_PATH, "r")) == NULL) return FAIL;
	if((*recordes = fopen(RANKING_PATH, "r+")) == NULL) return FAIL;

	return PASS;
}

int audioInit(Mix_Music **tetrisBgm) {
	if(SDL_Init(SDL_INIT_AUDIO) < 0) return FAIL;
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) return FAIL;
	if((*tetrisBgm = Mix_LoadMUS(BGM_PATH)) == NULL) return FAIL;
	Mix_VolumeMusic(40);
	if(Mix_VolumeMusic(-1) != 40) return FAIL;

	return PASS;
}

void initNcurses() {
	initscr(); noecho(); curs_set(0); start_color();
}

int colorCheck() {
	if(!has_colors()) return FAIL;
	if(!can_change_color()) return FAIL;
	if(COLORS < 16) return FAIL;
	if(COLOR_PAIRS < 16) return FAIL;

	return PASS;
}

void initColors() {

	/*
	 * Como queremos que as cores sejam as mesmas, independentemente do tema do terminal,
	 * nós inicializamos as cores padrão com valores específicos.
	 *
	 * Também há distinção entre as cores de texto normal e de texto em negrito. Normalmente, esse último
	 * tem cores mais claras e, às vezes, diferentes das cores base, mas não queremos isso aqui.
	 */

	// Cores personalizadas:
	init_color(COLOR_BLACK, 0, 0, 0);
	init_color(COLOR_WHITE, 1000, 1000, 1000);
	init_color(COLOR_RED, 850, 200, 200);
	init_color(COLOR_ORANGE, 900, 450, 100);
	init_color(COLOR_YELLOW, 800, 800, 250);
	init_color(COLOR_GREEN, 150, 730, 150);
	init_color(COLOR_CYAN, 150, 800, 800);
	init_color(COLOR_BLUE, 300, 300, 900);
	init_color(COLOR_MAGENTA, 700, 180, 800);
	init_color(COLOR_RED_BOLD, 850, 200, 200);
	init_color(COLOR_ORANGE_BOLD, 900, 450, 100);
	init_color(COLOR_YELLOW_BOLD, 800, 800, 250);
	init_color(COLOR_GREEN_BOLD, 150, 730, 150);
	init_color(COLOR_CYAN_BOLD, 150, 800, 800);
	init_color(COLOR_BLUE_BOLD, 300, 300, 900);
	init_color(COLOR_MAGENTA_BOLD, 700, 180, 800);

	// Pares de cor para os blocos:
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(2, COLOR_BLACK, COLOR_ORANGE);
	init_pair(3, COLOR_BLACK, COLOR_YELLOW);
	init_pair(4, COLOR_BLACK, COLOR_GREEN);
	init_pair(5, COLOR_BLACK, COLOR_CYAN);
	init_pair(6, COLOR_BLACK, COLOR_BLUE);
	init_pair(7, COLOR_BLACK, COLOR_MAGENTA);

	// Pares de cor para texto:
	init_pair(8, COLOR_RED, COLOR_BLACK);
	init_pair(9, COLOR_ORANGE, COLOR_BLACK);
	init_pair(10, COLOR_YELLOW, COLOR_BLACK);
	init_pair(11, COLOR_GREEN, COLOR_BLACK);
	init_pair(12, COLOR_CYAN, COLOR_BLACK);
	init_pair(13, COLOR_BLUE, COLOR_BLACK);
	init_pair(14, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(15, COLOR_BLACK, COLOR_BLACK);
}

bool calculateOffset(FILE *fp, coord_t *center, coord_t *offset) {

	/*
	 * "offset" tem a ver com a posição do cursor na tela, que é "deslocada"
	 * em relação ao centro da tela.
	 */

	char tempString[TEXT_LENGTH];
	int largura = 0, altura = 0, aux;
	coord_t prevOffset = *offset;	// Lembrar do offset anterior para comparar depois
	center->x = COLS / 2; center->y = LINES / 2;

	while(fgets(tempString, TEXT_LENGTH, fp) == tempString) {
		aux = mbstowcs(NULL, tempString, TEXT_LENGTH);
		largura = aux > largura ? aux : largura;
		++altura;
	}
	rewind(fp);

	offset->x = center->x - (largura / 2); offset->y = center->y - (altura / 2);

	// Isso é para evitar que a tela fique preta caso a janela seja muito pequena.
	if(offset->x < 0) offset->x = 0;
	if(offset->y < 0) offset->y = 0;

	// Esse resultado é usado pelo if() externo para determinar se a tela precisa ser redesenhada.
	// Ajuda a reduzir as piscadas na tela.
	if((offset->x != prevOffset.x) || (offset->y != prevOffset.y)) return TRUE;
	else return FALSE;
}
void printFileCentered(FILE *fp, coord_t *offset) {
	char tempString[TEXT_LENGTH];

	clear();
	for(int i = 0; fgets(tempString, TEXT_LENGTH, fp) == tempString; ++i)
		mvaddstr(offset->y + i, offset->x, tempString);
	rewind(fp);
}

void colorMainMenu(coord_t *offset) {

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
	mvchgat(offset->y + 12, offset->x + 16, 2, WA_BOLD, 11, NULL);
	mvchgat(offset->y + 13, offset->x + 16, 2, WA_BOLD, 12, NULL);
	mvchgat(offset->y + 14, offset->x + 16, 2, WA_BOLD, 10, NULL);
	mvchgat(offset->y + 15, offset->x + 16, 2, WA_BOLD, 14, NULL);
	mvchgat(offset->y + 16, offset->x + 16, 2, WA_BOLD, 8, NULL);
}

void colorInstrucoes(coord_t *offset) {

	// Colorir os títulos de cada tópico e o 'q' no prompt na parte inferior
	mvchgat(offset->y + 2, offset->x + 3, 13, WA_BOLD, 10, NULL);
	mvchgat(offset->y + 11, offset->x + 3, 10, WA_BOLD, 11, NULL);
	mvchgat(offset->y + 23, offset->x + 32, 3, WA_BOLD, 8, NULL);
}

void preencherPlacar(FILE *recordes, coord_t *offset) {
	ranking info[10];

	for(int i = 0; i < 10; ++i) {
		// Poderíamos primeiro ler o arquivo todo e guardar as informações na matriz de structs,
		// e depois exibir as informações armazenadas nela, mas aí precisaríamos de dois loops for().
		// Desta maneira, fica mais simples.

		if(fscanf(recordes, "%3s %d", info[i].nome, &info[i].pontuacao) != 2) break;
		mvaddstr(offset->y + 7 + i, offset->x + 13, info[i].nome);
		mvprintw(offset->y + 7 + i, offset->x + 23, "%8d", info[i].pontuacao);
	}
	rewind(recordes);
}

void colorPlacar(coord_t *offset) {
	mvchgat(offset->y, offset->x, 40, WA_DIM, 0, NULL);
	for(int i = 1; i < 20; ++i) {
		mvchgat(offset->y + i, offset->x, 1, WA_DIM, 0, NULL);
		mvchgat(offset->y + i, offset->x + 39, 1, WA_DIM, 0, NULL);
	}
	mvchgat(offset->y + 20, offset->x, 40, WA_DIM, 0, NULL);

	for(int j = 0; j < 2; ++j) {
		mvchgat(offset->y + 3 + j, offset->x + 10, 3, WA_NORMAL, 10, NULL);
		mvchgat(offset->y + 3 + j, offset->x + 14, 3, WA_NORMAL, 9, NULL);
		mvchgat(offset->y + 3 + j, offset->x + 18, 3, WA_NORMAL, 14, NULL);
		mvchgat(offset->y + 3 + j, offset->x + 24, 6, WA_NORMAL, 13, NULL);
	}

	mvchgat(offset->y + 22, offset->x + 17, 3, WA_BOLD, 8, NULL);
}

void colorCreditos(coord_t *offset) {
	mvchgat(offset->y + 2, offset->x + 18, 7, WA_BOLD | WA_UNDERLINE, 12, NULL);
}

void colorGameUI(coord_t *offset) {

	// Eu quis fazer as bordas ficarem mais esmaecidas. Contraste ou algo assim.
	mvchgat(offset->y, offset->x, 51, WA_DIM, 0, NULL);
	for(int i = 1; i < 25; ++i) {
		mvchgat(offset->y + i, offset->x, 1, WA_DIM, 0, NULL);
		mvchgat(offset->y + i, offset->x + 50, 1, WA_DIM, 0, NULL);
	}
	mvchgat(offset->y + 25, offset->x, 51, WA_DIM, 0, NULL);

	// Colorir a logo do Tetris na interface do jogo. Mesma estratégia de antes.
	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 4; ++j)
			mvchgat(offset->y + 3 + i, offset->x + 28 + 3*j, 3, WA_NORMAL, j + 8, NULL);
		mvchgat(offset->y + 3 + i, offset->x + 40, 1, WA_NORMAL, 12, NULL);
		mvchgat(offset->y + 3 + i, offset->x + 41, 3, WA_NORMAL, 14, NULL);
	}

	// Colorir os outros indicadores (linhas eliminadas, nível, próxima peça)
	mvchgat(offset->y + 9, offset->x + 28, 5, WA_BOLD, 9, NULL);
	mvchgat(offset->y + 9, offset->x + 37, 6, WA_BOLD, 10, NULL);
	mvchgat(offset->y + 13, offset->x + 30, 12, WA_BOLD, 11, NULL);
}

void fillGameUiInfo(borda *bordaJogo, peca_ap *peca, info *jogoInfo, coord_t *offset, int *cor2) {
	int k, l;

	switch(*cor2) {
		case 3: k = 0; l = 1; break;
		case 5: k = -1; l = -1; break;
		default: k = 0; l = 0;
	}

	// Pontuação, nível e linhas:
	mvprintw(offset->y + 7, offset->x + 28, "%16d", jogoInfo->score);
	mvprintw(offset->y + 10, offset->x + 28, "%5d", jogoInfo->nivel);
	mvprintw(offset->y + 10, offset->x + 37, "%6d", jogoInfo->alinhas);

	// Próxima peça:
	for(int i = 0; i < TAMP; ++i) {
		for(int j = 0; j < TAMP; ++j) {
            if (!j)
                mvchgat(offset->y + 18 + i, offset->x + 32 + j, 2, WA_NORMAL, 0, NULL);
			if (peca->peca2[i][j] == '#')
				mvchgat(offset->y + 18 + i + k, offset->x + 33 + 2*j + l, 2, WA_NORMAL, *cor2, NULL);
			else
				mvchgat(offset->y + 18 + i + k, offset->x + 33 + 2*j + l, 2, WA_NORMAL, 0, NULL);
		}
	}

	for(int i = 1; i < LINB - 1; ++i) {
		for(int j = 1; j < COLB - 1; ++j) {
			if (bordaJogo->borda[i][j] == '#')
				mvchgat(offset->y + 3 + (i-1), offset->x + 6 + 2*(j-1), 2, WA_NORMAL, bordaJogo->corBorda[i][j], NULL);
			else
				mvchgat(offset->y + 3 + (i-1), offset->x + 6 + 2*(j-1), 2, WA_NORMAL, 0, NULL);
		}
	}
}

void loopJogo(FILE *gameUI, FILE *gameOverUI, FILE *recordes, coord_t *center, coord_t *offset, Mix_Music *tetrisBgm) {

	srand(time(NULL));
	borda bordaJogo;
    pecas pecas_tetris;
    peca_ap peca;
    info jogoInfo = {1, 0, 0, 1.0, jogoInfo.tempo * CLOCKS_PER_SEC};
    coord ajustePeca;
    int cor2; char ch = 0;
    lixo(&bordaJogo);
    defBorda(&bordaJogo);
    defPeca(&pecas_tetris);
    int gameOver = 0, inicial = 0;
	keypad(stdscr, TRUE); nodelay(stdscr, TRUE);
	Mix_PlayMusic(tetrisBgm, -1);

    while (!gameOver) {
		refresh();
        linhaCompleta(&bordaJogo, &jogoInfo);
        bordaJogo.cor = sorteioPeca(&inicial, &cor2, &peca, &pecas_tetris);
		if(calculateOffset(gameUI, center, offset) == TRUE) {
			printFileCentered(gameUI,  offset);
			colorGameUI(offset);
		}
		fillGameUiInfo(&bordaJogo, &peca, &jogoInfo, offset, &cor2);
        check(&bordaJogo, &jogoInfo);
        ajuste(&peca, &ajustePeca);
        moverPeca(&bordaJogo, &peca, &jogoInfo, gameUI, center, offset, &cor2, &ch);
        if (ch == 'q') break;
        gameOver = check(&bordaJogo, &jogoInfo);
    }
	keypad(stdscr, FALSE); nodelay(stdscr, FALSE);

	if(gameOver) {
		flash(); usleep(100000); flash();

		for(int i = 1; i < LINB-1; ++i) {
			for(int j = 1; j < COLB-1; ++j)
				bordaJogo.borda[i][j] = ' ';

			if(calculateOffset(gameUI, center, offset)) {
				printFileCentered(gameUI,  offset);
				colorGameUI(offset);
			}

			fillGameUiInfo(&bordaJogo, &peca, &jogoInfo, offset, &cor2);
			refresh();
			usleep(40000);
		}

		Mix_HaltMusic();
		telaDeGameOver(gameOverUI, recordes, jogoInfo.score, center, offset);
	}
	Mix_HaltMusic();
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

int check(borda *bordaJogo, info *jogoInfo) {
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
    coord_.posX = 1; coord_.posY = (COLB / 2) - 2;
    ajuste(peca, &coord_);
    int colidiu;
    clock_t inicial = clock();
    jogoInfo->delay = jogoInfo->tempo * CLOCKS_PER_SEC;
    while (1) {
        int teclou = 0;
        for (int i = 0; i < TAMP; i++) {
            for (int j = 0; j < TAMP; j++) {
                if (peca->peca1[i][j] == '#' && coord_.posX + i < LINB && coord_.posY + j < COLB && coord_.posX + i >= 0 && coord_.posY + j >= 0)
                    bordaJogo->borda[coord_.posX + i][coord_.posY + j] = ' ';
            }
        }
        int tecla = tolower(getch());
        switch (tecla) {
            case KEY_DOWN:
                coord_temp = (coord){coord_.posX + 1, coord_.posY};
                if (!checarColisao(bordaJogo, peca, &coord_temp)) {
                    coord_.posX = coord_temp.posX;
                    teclou = 1;
                    jogoInfo->score += 1;
                }
                break;
            case KEY_LEFT:
                coord_temp = (coord){coord_.posX, coord_.posY - 1};
                if (!checarColisao(bordaJogo, peca, &coord_temp)) {
                    coord_.posY = coord_temp.posY;
                    teclou = 1;
                }
                break;
            case KEY_RIGHT:
                coord_temp = (coord){coord_.posX, coord_.posY + 1};
                if (!checarColisao(bordaJogo, peca, &coord_temp)) {
                    coord_.posY = coord_temp.posY;
                    teclou = 1;
                }
                break;
            case 'z':
                rotacionarPeca(peca, bordaJogo);
                if (coord_.posX < 2 && checarColisao(bordaJogo, peca, &coord_) && bordaJogo->cor == 5)
                    coord_.posX += 2;
                if (coord_.posY < 2 && checarColisao(bordaJogo, peca, &coord_) && bordaJogo->cor != 3) {
                    if (bordaJogo->cor != 5)
                        coord_.posY +=1;
                    else
                        coord_.posY +=2;
                }
                if (coord_.posY > COLB - 7 && checarColisao(bordaJogo, peca, &coord_) && bordaJogo->cor != 3) {
                    if (bordaJogo->cor != 5)
                        coord_.posY -=1;
                    else
                        coord_.posY -=2;
                }
                if (bordaJogo->cor == 3) {
                    for (int i = 0; i < 3; i++)
                        rotacionarPeca(peca, bordaJogo);
                }
                teclou = 1;
                break;
            case 'x':
		int posX_inicial = coord_.posX;
                while (1) {
                    coord_temp = (coord){coord_.posX + 1, coord_.posY};
                    if (checarColisao(bordaJogo, peca, &coord_temp)) {
			jogoInfo->score += coord_.posX - posX_inicial;
                        break;
		    }
                    coord_.posX = coord_temp.posX;
                }
                teclou = 1;
                break;
            case 'c':
                while (1) {
                    tecla = tolower(getch());
                    if (tecla == 'c')
                        break;
                    if (tecla == 'q') {
                        *ch = 'q';
                        break;
                    }
                }
                teclou = 1;
                break;
            case 'q':
                *ch = 'q';
                break;
        }
        if (*ch == 'q')
            break;
        clock_t atual = clock();
        if ((atual - inicial) >= jogoInfo->delay || tecla == 'x') {
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
            inicial = atual;
        }
        for (int i = 0; i < TAMP; i++) {
            for (int j = 0; j < TAMP; j++) {
                if (peca->peca1[i][j] == '#' && coord_.posX + i < LINB && coord_.posY + j < COLB && coord_.posX + i >= 0 && coord_.posY + j >= 0) {
                    bordaJogo->borda[coord_.posX + i][coord_.posY + j] = peca->peca1[i][j];
                    bordaJogo->corBorda[coord_.posX + i][coord_.posY + j] = bordaJogo->cor;
                }
            }
        }
        if (teclou) {
            refresh();
        }
        if(calculateOffset(gameUI, center, offset))
            printFileCentered(gameUI, offset);
        colorGameUI(offset);
        fillGameUiInfo(bordaJogo, peca, jogoInfo, offset, cor2);
        check(bordaJogo, jogoInfo);
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
	        case 1: jogoInfo->score += 100 * jogoInfo->nivel; jogoInfo->alinhas+=(clinhas);  break;
	        case 2: jogoInfo->score += 300 * jogoInfo->nivel; jogoInfo->alinhas+=(clinhas);  break;
	        case 3: jogoInfo->score += 500 * jogoInfo->nivel; jogoInfo->alinhas+=(clinhas);  break;
	        case 4: jogoInfo->score += 800 * jogoInfo->nivel; jogoInfo->alinhas+=(clinhas);  break;
        }
        if (jogoInfo->alinhas >= jogoInfo->nivel*10 && jogoInfo->nivel <= 9) {
                jogoInfo->nivel +=1;
                jogoInfo->tempo -=0.1;
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

void ajuste(peca_ap *peca, coord *ajustePeca) {
    int min = TAMP;
    for (int i = 0; i < TAMP; i++) {
        for (int j = 0; j < TAMP; j++) {
            if (peca->peca1[i][j] == '#') {
                if (i < min)
                    min = i;
            }
        }
    }
    ajustePeca->posX = ajustePeca->posX - min;
}

void telaDeGameOver(FILE *gameOverUI, FILE *recordes, int pontuacao, coord_t *center, coord_t *offset) {
	bool novoRecorde = FALSE;
	char nomeJogador[4], auxNome[4];
	ranking info[10];
	int posicaoNovoRecorde, auxPontuacao;

	for(int i = 0; i < 10; ++i)
		fscanf(recordes, "%3s %d", info[i].nome, &info[i].pontuacao);
	rewind(recordes);

	for(int j = 0; j < 10; ++j) {
		if(pontuacao > info[j].pontuacao) {
			posicaoNovoRecorde = j;
			novoRecorde = TRUE;
			break;
		}
	}

	while(1) {
		if(calculateOffset(gameOverUI, center, offset) == TRUE) {
			printFileCentered(gameOverUI, offset);
			colorGameOver(offset);
			mvprintw(offset->y + 12, offset->x + 46, "%d", pontuacao);
		}

		if(novoRecorde == TRUE) {
			mvchgat(offset->y + 14, 0, -1, WA_NORMAL, 15, NULL);
			mvaddstr(offset->y + 15, offset->x + 58, "   ");
			move(offset->y + 15, offset->x + 58);
			refresh();

			curs_set(1); echo();
			getnstr(nomeJogador, 3);
			curs_set(0); noecho();
			if(strlen(nomeJogador) != 3) continue;

			for(int k = posicaoNovoRecorde; k < 10; ++k) {
				strcpy(auxNome, info[k].nome);
				strcpy(info[k].nome, nomeJogador);
				strcpy(nomeJogador, auxNome);

				auxPontuacao = info[k].pontuacao;
				info[k].pontuacao = pontuacao;
				pontuacao = auxPontuacao;
			}

			for(int l = 0; l < 10; ++l)
				fprintf(recordes, "%3s %d\n", info[l].nome, info[l].pontuacao);

			fflush(recordes);
			rewind(recordes);
			break;
		}
		else {
			mvchgat(offset->y + 13, 0, -1, WA_NORMAL, 15, NULL);
			mvchgat(offset->y + 15, 0, -1, WA_NORMAL, 15, NULL);
			refresh();
			if(tolower(getch()) == 'q') break;
		}
	}
}

void colorGameOver(coord_t *offset) {
	for(int i = 0; i < 6; ++i) {
		mvchgat(offset->y + 2 + i, offset->x + 3, 17, WA_NORMAL, 8, NULL);
		mvchgat(offset->y + 2 + i, offset->x + 20, 19, WA_NORMAL, 9, NULL);
		mvchgat(offset->y + 2 + i, offset->x + 43, 18, WA_NORMAL, 13, NULL);
		mvchgat(offset->y + 2 + i, offset->x + 61, 16, WA_NORMAL, 14, NULL);
	}

	mvchgat(offset->y + 13, offset->x + 34, 13, WA_BOLD | WA_BLINK, 10, NULL);
	mvchgat(offset->y + 14, offset->x + 28, 3, WA_BOLD, 8, NULL);
}
