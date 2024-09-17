# TETRIS
Projeto de final de semestre Introdução à Programação (Professor Lucas)

## Requisitos
Este programa foi feito para ser compilado e/ou executado em sistemas operacionais baseados em Linux (Ubuntu, Linux Mint, Pop!\_OS etc.) para computadores desktop.
Ele não funciona ou possui funcionalidade limitada em outras plataformas.

### Execução
Este jogo deve ser aberto via linha de comando e o terminal deve ter suporte a cores e a redefinição de cores.
Sem esses recursos, algumas das cores não serão exibidas corretamente. Em alguns casos, o programa pode não abrir.

Você também precisará instalar estes pacotes:

+ libncursesw6
+ libsdl2-2.0-0
+ libsdl2-mixer-2.0-0

Se você planeja compilar a partir do código fonte, esses pacotes já devem ser instalados automaticamente junto com os pacotes de desenvolvimento
(terminados em "-dev").

Para abrir o jogo, basta utilizar este comando na mesma pasta do arquivo executável:
```
$ ./tetris
```

Caso você não consiga abrir o arquivo, talvez seja necessário marcá-lo como executável primeiro. Para isso, use este comando:
```
$ chmod +x ./tetris
```

### Compilação
Para compilar o jogo a partir do código fonte, você irá precisar destes pacotes instalados em seu sistema:

+ git
+ gcc
+ make
+ pkgconf
+ libncurses-dev
+ libsdl2-dev
+ libsdl2-mixer-dev

Utilize o gerenciador de pacotes de sua distribuição para instalar essas dependências.
Em seguida, abra uma janela do terminal e execute estes comandos, um por um:
```
$ git clone https://github.com/irochimaendo/TETRIS.git
$ cd TETRIS
$ make
```
Um arquivo binário deve ser gerado. Para abri-lo, use o comando:
```
$ ./tetris
```

Novamente, talvez seja necessário marcar o arquivo recém-gerado como executável.
