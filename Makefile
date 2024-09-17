build:
	gcc -o ./tetris -std=c99 -Wall main.c $$(pkgconf ncursesw sdl2 SDL2_mixer --cflags --libs)
