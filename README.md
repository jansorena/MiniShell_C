# Proyecto1_SistemasOperativos

Para compilar la shell, se debe realizar make en el directorio correspondiente:

	$ make

En caso de no contar con makefile utilizar:

	$ gcc -Wall -o mishell p1.c
	$ ./mishell

La shell soporta comandos simples como ls, echo, cat, grep, head, tail, wc, sort, etc.
Además, soporta comandos con pipes, del tipo $comando1 | comando2 | ... | comandon-ésimo tales como:

	mishell:$ ps -aux | sort -nr -k 4 | head -10
	mishell:$ cat texto.txt | wc -w
