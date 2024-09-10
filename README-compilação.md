Este código foi feito para ser compilado em sistemas baseados em Linux e precisa do pacote "libncurses-dev" instalado.

Se você deseja compilar o código e testar o programa, use este comando:
```
$ gcc main.c [-o <caminho_do_programa>] -std=c99 $(ncursesw6-config --cflags --libs)
```
se estiver usando o "gcc" ou:
```
$ clang main.c [-o <caminho_do_programa>] -std=c99 $(ncursesw6-config --cflags --libs)
```
se estiver usando o "clang".
Isso vai garantir que as flags de compilação certas para sua distro sejam usadas.

O código também pode ser compilado no Android através do Termux com o "clang".
O executável deve ser colocado na raíz do projeto, junto com o código fonte, no mesmo diretório da pasta "assets".
