#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <locale.h>
#define GAME_W 15
#define GAME_H 20
#define TEXT_LENGTH 256
#define MENU_PATH "./assets/tetris_menu.txt"
#define INSTR_PATH "./assets/instrucoes.txt"
#define CRED_PATH "./assets/creditos.txt"
#define GAMEUI_PATH "./assets/game_ui.txt"
#define COLOR_ORANGE 8

typedef struct {
	int x;
	int y;
} coord_t;

//Protótipos:
void initColors();
void colorMenuLogo(coord_t *center, coord_t *offset);
void colorGameUI(coord_t *center, coord_t *offset);
void printFileCentered(FILE *fp, coord_t *center, coord_t *offset);
void loopJogo(FILE *gameUI, coord_t *center, coord_t *offset);

int main() {
	char select; // Variável para o seletor do menu
	FILE *tetrisMenu, *instrucoes, *creditos, *gameUI; // Arquivos a serem usados
	coord_t scrCenter, cursOffset; // Variáveis para calcular o centro da tela
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
		printFileCentered(tetrisMenu, &scrCenter, &cursOffset);
		colorMenuLogo(&scrCenter, &cursOffset);
		refresh();
		
		// Seletor do menu principal
		select = getch();
		switch(select) {
			case 'p':
				// Lógica principal do jogo
				loopJogo(gameUI, &scrCenter, &cursOffset);
				break;
			case 'i':
				do {
					printFileCentered(instrucoes, &scrCenter, &cursOffset);
					refresh();
					select = getch();
				} while(select != 'q');
				select = 0;
				break;
			case 'c':
				do {
					printFileCentered(creditos, &scrCenter, &cursOffset);
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

void printFileCentered(FILE *fp, coord_t *center, coord_t *offset) {

	/*
	 * Essa função calcula o centro da janela atual e imprime o texto
	 * de um arquivo. O texto é centralizado dinamicamente de acordo com
	 * as dimensões do arquivo.
	 *
	 * A maneira como essa função é inserida no programa faz com que a interface
	 * exibida permaneça centralizada o tempo todo, mesmo que o usuário mude o
	 * tamanho da janela.
	 */

	char str[TEXT_LENGTH];
	int largura = 0, altura = 0, l;
	center->x = COLS / 2; center->y = LINES / 2;

	while(fgets(str, TEXT_LENGTH, fp) == str) {
		l = mbstowcs(NULL, str, TEXT_LENGTH);
		largura = l > largura ? l : largura;
		++altura;
	}
	rewind(fp);

	offset->x = largura / 2; offset->y = altura / 2;
	if(center->x < offset->x) offset->x = center->x; 
	if(center->y < offset->y) offset->y = center->y; 

	clear();
	for(int i = 0; fgets(str, TEXT_LENGTH, fp) == str; ++i)
		mvaddstr(center->y - offset->y + i, center->x - offset->x,str);
	rewind(fp);
}

void colorMenuLogo(coord_t *center, coord_t *offset) {

	/*
	 * Essa função colore as letras da logo do menu principal
	 * tal qual a logo do jogo original.
	 *
	 * Se a posição da logo no arquivo "tetris_menu.txt" for alterada,
	 * essa função terá que ser alterada também.
	 */

	for(int i = 0; i < 6; ++i) {
		mvchgat((center->y - offset->y) + 2 + i, (center->x - offset->x) + 3, 9, WA_NORMAL, 8, NULL);
		mvchgat((center->y - offset->y) + 2 + i, (center->x - offset->x) + 12, 8, WA_NORMAL, 9, NULL);
		mvchgat((center->y - offset->y) + 2 + i, (center->x - offset->x) + 20, 9, WA_NORMAL, 10, NULL);
		mvchgat((center->y - offset->y) + 2 + i, (center->x - offset->x) + 29, 8, WA_NORMAL, 11, NULL);
		mvchgat((center->y - offset->y) + 2 + i, (center->x - offset->x) + 37, 3, WA_NORMAL, 12, NULL);
		mvchgat((center->y - offset->y) + 2 + i, (center->x - offset->x) + 40, 8, WA_NORMAL, 14, NULL);
	}
}

void colorGameUI(coord_t *center, coord_t *offset) {

	/*
	 * O mesmo que a função anterior, mas para a interface do jogo 
	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 4; ++j)
			mvchgat((center->y - offset->y) + 3 + i, (center->x - offset->x) + 25 + 3*j, 3, WA_NORMAL, j + 8, NULL);
		mvchgat((center->y - offset->y) + 3 + i, (center->x - offset->x) + 37, 1, WA_NORMAL, 12, NULL);
		mvchgat((center->y - offset->y) + 3 + i, (center->x - offset->x) + 38, 3, WA_NORMAL, 14, NULL);
	}
}

void loopJogo(FILE *gameUI, coord_t *center, coord_t *offset) {
	char ch;
	do {
		printFileCentered(gameUI, center, offset);
		colorGameUI(center, offset);
		refresh();
		ch = 0;
		ch = getch();
	} while(ch != 'q');
}
