Este código foi feito para ser compilado em sistemas baseados em Linux e precisa dos pacote "libncurses-dev", "libsdl2-dev" e "libsdl2-mixer-dev" instalados.

Se você deseja compilar o código e testar o programa, use este comando:
```
$ gcc main.c [-o <caminho_do_programa>] -std=c99 $(pkg-config ncursesw sdl2 SDL2_mixer --cflags --libs)
```
Isso vai garantir que as flags de compilação certas para sua distro sejam usadas.

O executável deve ser colocado na raíz do projeto, junto com o código fonte, no mesmo diretório da pasta "assets".
