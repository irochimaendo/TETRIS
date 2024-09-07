#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <locale.h>
#include <ctype.h>

// Dimensões da matriz onde os tetraminós vão ser empilhados.
#define GAME_W 15
#define GAME_H 18

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

//Protótipos:
void initColors();
int calculateOffset(FILE *fp, coord_t *center, coord_t *offset);
void colorMainMenu(coord_t *offset);
void colorInstructions(coord_t *offset);
void colorGameUI(coord_t *offset);
void fillGameUiInfo(char matrizJogo[GAME_H][GAME_W + 1], char proximaPeca[4][4], int *score, int *level, int *totalLines, coord_t *offset);
void printFileCentered(FILE *fp, coord_t *offset);
void loopJogo(FILE *gameUI, coord_t *center, coord_t *offset);

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

void fillGameUiInfo(char matrizJogo[GAME_H][GAME_W + 1], char proximaPeca[4][4], int *score, int *level, int *totalLines, coord_t *offset) {

	/*
	 * Essa função lida com as informações presentes na interface (pontuação, linhas, próxima peça),
	 * além de exibir a matriz principal de peças.
	 */

	// Pontuação, nível e linhas:
	mvprintw(offset->y + 7, offset->x + 23, "%16d", *score);
	mvprintw(offset->y + 10, offset->x + 23, "%5d", *level);
	mvprintw(offset->y + 10, offset->x + 32, "%6d", *totalLines);

	// Próxima peça:
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j)
			mvchgat(offset->y + 16 + i, offset->x + 29 + j, 1, WA_NORMAL, proximaPeca[i][j], NULL);
	}

	// Colorindo os blocos de acordo com os valores na matrizJogo
	for(int i = 0; i < GAME_H; ++i) {
		for(int j = 0; j < GAME_W; ++j)
			mvchgat(offset->y + 3 + i, offset->x + 6 + j, 1, WA_NORMAL, matrizJogo[i][j], NULL);
	}
}

void loopJogo(FILE *gameUI, coord_t *center, coord_t *offset) {

	/*
	 * Os valores que estão aqui só servem para demonstração. No programa de verdade,
	 * após toda a lógica ser feita, basta chamar as funções dentro do loop do-while
	 * (exceto as duas últimas, possivelmente), fornecendo as variáveis correspondentes.
	 */

	char matrizJogo[GAME_H][GAME_W + 1] = {}, proximaPeca[4][4] = {}, ch;
	int score = 69420, linhas = 21, nivel = 4;

	proximaPeca[1][1] = 3; proximaPeca[1][2] = 3;
	proximaPeca[2][1] = 3; proximaPeca[2][2] = 3;

	matrizJogo[GAME_H - 3][0] = '\x06';
	strcpy(&matrizJogo[GAME_H - 3][11], "\x02\x02");
	strcpy(&matrizJogo[GAME_H - 2][0], "\x06\x06\x06\x01\x01");
	matrizJogo[GAME_H - 2][7] = '\x07';
	strcpy(&matrizJogo[GAME_H - 2][11], "\x02\x02\x03\x03");
	strcpy(matrizJogo[GAME_H - 1], "\x05\x05\x05\x05\x01\x01\x07\x07\x07\x02\x02\x02\x02\x03\x03");
	strcpy(&matrizJogo[2][5], "\x05\x05\x05\x05");

	do { // Aqui é onde a mágica realmente acontece:
		if(calculateOffset(gameUI, center, offset))
			printFileCentered(gameUI,  offset);
		colorGameUI(offset);
		fillGameUiInfo(matrizJogo, proximaPeca, &score, &nivel, &linhas, offset);
		refresh();
		ch = 0;
		ch = getch();
		ch = tolower(ch);
	} while(ch != 'q');
}
