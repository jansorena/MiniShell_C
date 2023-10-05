# Proyecto1_SistemasOperativos

Para compilar la shell, se debe seguir el proceso habitual de compilado con cmake:

console
$ mkdir build
$ cd build
$ cmake ..
$ make

La shell soporta comandos simples como:
mishell:$ ls
mishell:$ echo
mishell:$ cat
mishell:$ grep
mishell:$ head
mishell:$ tail
mishell:$ wc
mishell:$ sort
mishell:$ paste
Además, soporta comandos con pipes, del tipo $comando1 | comando2 tales como:
mishell:$ ps -aux | sort -nr -k 4 | head -10
mishell:$ cat texto.txt | wc -w
