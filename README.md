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

Para crear un daemon que mida y registre una línea con la información requerida del sistema en el log del sistema cada t segundos por un tiempo total de p segundos, se debe ejecutar el siguiente comando dentro de la shell:

	mishell:$ daemon <t> <p>
 Luego, para matar al daemon basta con ejecutar el comando kill junto con el PID del demonio (el cual es entregado) desde otra terminal, de la siguiente manera:
 
 	$ kill <PID>
