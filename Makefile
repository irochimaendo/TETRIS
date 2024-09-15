build:
	gcc -o ./tetris -std=c99 main.c $$(pkgconf ncursesw sdl2 SDL2_mixer --cflags --libs)
