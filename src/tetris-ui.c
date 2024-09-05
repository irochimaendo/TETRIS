#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <locale.h>
#define GAME_W 15
#define GAME_H 18
#define TEXT_LENGTH 256
#define MENU_PATH "./assets/tetris-menu.txt"
#define INSTR_PATH "./assets/instrucoes.txt"
#define CRED_PATH "./assets/creditos.txt"
#define GAMEUI_PATH "./assets/game-ui.txt"
#define COLOR_ORANGE 8

typedef struct {
	int x;
	int y;
} coord_t;

//Protótipos:
void initColors();
void colorMenuLogo(coord_t *center, coord_t *offset);
void colorGameUI(char matrizJogo[][GAME_W + 1], coord_t *center, coord_t *offset);
void printFileCentered(FILE *fp, coord_t *center, coord_t *offset, coord_t *prevOffset);
void loopJogo(FILE *gameUI, coord_t *center, coord_t *offset, coord_t *prevOffset);

int main() {
	char select; // Variável para o seletor do menu
	FILE *tetrisMenu, *instrucoes, *creditos, *gameUI; // Arquivos a serem usados
	coord_t scrCenter, cursOffset, prevCursOffset; // Variáveis para calcular o centro da tela
	setlocale(LC_ALL, ""); // Define o locale do programa para o locale do sistema

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

	// Define as cores e os pares de cores que serão usados ao longo do programa
	initColors();

	do {
		// Mostra o menu principal centralizado na tela
		printFileCentered(tetrisMenu, &scrCenter, &cursOffset, &prevCursOffset);
		colorMenuLogo(&scrCenter, &cursOffset);
		refresh();

		// Seletor do menu principal
		select = getch();
		switch(select) {
			case 'p':
				// Lógica principal do jogo
				loopJogo(gameUI, &scrCenter, &cursOffset, &prevCursOffset);
				break;
			case 'i':
				do {
					printFileCentered(instrucoes, &scrCenter, &cursOffset, &prevCursOffset);
					refresh();
					select = getch();
				} while(select != 'q');
				select = 0;
				break;
			case 'c':
				do {
					printFileCentered(creditos, &scrCenter, &cursOffset, &prevCursOffset);
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
	 * a cor laranja, que não é definida por padrão pela <curses.h>.
	 *
	 * Como esses valores são personalizados, eles não dependem do esquema de cores
	 * do terminal atual.
	 */

	init_color(COLOR_BLACK, 0, 0, 0);
	init_color(COLOR_WHITE, 1000, 1000, 1000);
	init_color(COLOR_RED, 850, 0, 0);
	init_color(COLOR_ORANGE, 900, 450, 0);
	init_color(COLOR_YELLOW, 800, 800, 0);
	init_color(COLOR_GREEN, 0, 800, 0);
	init_color(COLOR_CYAN, 0, 800, 800);
	init_color(COLOR_BLUE, 50, 50, 900);
	init_color(COLOR_MAGENTA, 750, 0, 850);

	init_pair(1, COLOR_RED, COLOR_RED);
	init_pair(2, COLOR_ORANGE, COLOR_ORANGE);
	init_pair(3, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(4, COLOR_GREEN, COLOR_GREEN);
	init_pair(5, COLOR_CYAN, COLOR_CYAN);
	init_pair(6, COLOR_BLUE, COLOR_BLUE);
	init_pair(7, COLOR_MAGENTA, COLOR_MAGENTA);
	init_pair(8, COLOR_RED, COLOR_BLACK);
	init_pair(9, COLOR_ORANGE, COLOR_BLACK);
	init_pair(10, COLOR_YELLOW, COLOR_BLACK);
	init_pair(11, COLOR_GREEN, COLOR_BLACK);
	init_pair(12, COLOR_CYAN, COLOR_BLACK);
	init_pair(13, COLOR_BLUE, COLOR_BLACK);
	init_pair(14, COLOR_MAGENTA, COLOR_BLACK);
}

void printFileCentered(FILE *fp, coord_t *center, coord_t *offset, coord_t *prevOffset) {

	/*
	 * Essa função calcula o centro da janela atual e imprime o texto
	 * de um arquivo. O texto é centralizado dinamicamente de acordo com
	 * as dimensões do arquivo.
	 *
	 * A maneira como essa função é inserida no programa faz com que a interface
	 * exibida permaneça centralizada o tempo todo, mesmo que o usuário mude o
	 * tamanho da janela.
	 */

	char str[TEXT_LENGTH];	// String a ser usada pelo fgets() e pelo mvaddstr()
	int largura = 0, altura = 0, l;	// "l" é uma variável de controle
	center->x = COLS / 2; center->y = LINES / 2; // Armazena as coordenadas do centro da tela para mais tarde

	while(fgets(str, TEXT_LENGTH, fp) == str) {
		l = mbstowcs(NULL, str, TEXT_LENGTH); // Conta quantos caracteres multibyte cada linha tem.
		largura = l > largura ? l : largura;  // Compara com o valor que já temos: se for maior, guarda.
		++altura; // Incrementa o valor da altura
	}
	rewind(fp);

	// Atribui a posição do cursor para que o texto exibido fique centralizado.
	// Também impede que essa posição tenha um valor negativo, para que o cursor
	// não fique fora dos limites da tela devido a um underflow.
	offset->x = center->x - (largura / 2); offset->y = center->y - (altura / 2);
	if(offset->x < 0) offset->x = 0;
	if(offset->y < 0) offset->y = 0;

	// Verifica se a posição do cursor mudou para limpar a tela e "imprimir" o arquivo na tela de novo ou não.
	// Ajuda a reduzir as piscadas na tela.
	// Essa parte talvez vire sua própria função mais tarde.
	if(offset->x != prevOffset->x || offset->y != prevOffset->y) {
		clear();
		for(int i = 0; fgets(str, TEXT_LENGTH, fp) == str; ++i)
			mvaddstr(offset->y + i, offset->x, str);
		rewind(fp);
		*prevOffset = *offset;
	}
}

void colorMenuLogo(coord_t *center, coord_t *offset) {

	/*
	 * Essa função colore as letras da logo do menu principal
	 * tal qual a logo do jogo original.
	 *
	 * Se a posição da logo no arquivo "tetris-menu.txt" for alterada,
	 * essa função terá que ser alterada também.
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
}

void colorGameUI(char matrizJogo[][GAME_W + 1], coord_t *center, coord_t *offset) {

	/*
	 * O mesmo que a função anterior, mas para a interface do jogo.
	 */

	// Aqui dá para simplificar mais quando comparado à colorMenuLogo().
	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 4; ++j)
			mvchgat(offset->y + 3 + i, offset->x + 25 + 3*j, 3, WA_NORMAL, j + 8, NULL);
		mvchgat(offset->y + 3 + i, offset->x + 37, 1, WA_NORMAL, 12, NULL);
		mvchgat(offset->y + 3 + i, offset->x + 38, 3, WA_NORMAL, 14, NULL);
	}

	// Colorindo os blocos de acordo com os valores na matrizJogo
	for(int i = 0; i < GAME_H; ++i) {
		for(int j = 0; j < GAME_W; ++j)
			mvchgat(offset->y + 3 + i, offset->x + 8 + j, 1, WA_NORMAL, matrizJogo[i][j], NULL);
	}
}

void loopJogo(FILE *gameUI, coord_t *center, coord_t *offset, coord_t *prevOffset) {
	char matrizJogo[GAME_H][GAME_W + 1] = {}, proximaPeca[4][4] = {}, ch;

	/*
	 * Esse primeiro conjunto de operações é só para definir uma matriz para demonstração.
	 * Elas podem ser removidas quando a lógica real for colocadas aqui.
	 */

	matrizJogo[GAME_H - 3][0] = '\x06';
	strcpy(&matrizJogo[GAME_H - 3][11], "\x02\x02");
	strcpy(&matrizJogo[GAME_H - 2][0], "\x06\x06\x06\x01\x01");
	matrizJogo[GAME_H - 2][7] = '\x07';
	strcpy(&matrizJogo[GAME_H - 2][11], "\x02\x02\x03\x03");
	strcpy(matrizJogo[GAME_H - 1], "\x05\x05\x05\x05\x01\x01\x07\x07\x07\x02\x02\x02\x02\x03\x03");
	strcpy(&matrizJogo[2][5], "\x05\x05\x05\x05");

	do { // Aqui é onde a mágica realmente acontece
		printFileCentered(gameUI, center, offset, prevOffset);
		colorGameUI(matrizJogo, center, offset);
		refresh();
		ch = 0;
		ch = getch();
	} while(ch != 'q');
}
