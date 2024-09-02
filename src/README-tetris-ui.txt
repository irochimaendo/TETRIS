Este código (tetris-ui.c) foi feito para ser compilado em sistemas baseados em Linux e precisa dos pacotes "libncurses-dev" e "gcc" instalados.
Se você deseja compilar o código e testar o programa, use este comando:

$ gcc tetris_ui.c -o ../<nome_do_programa> -std=c99 $(ncursesw6-config --cflags --libs)

Isso vai garantir que as flags de compilação certas para sua distro sejam usadas.

O código também pode ser compilado no Android através do Termux, precisando apenas do "clang".
O comando é praticamente idêntico, apenas trocando "gcc" por "clang".
