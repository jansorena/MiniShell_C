#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

char *read_input() { //leer una linea de entrada
    char *input = NULL;
    size_t input_size = 0;
    getline(&input, &input_size, stdin);
    return input;
}

void parse_input(char *input, char **command, char **arguments) { //parsear la linea de entrada en comando y argumentos
    char *token = strtok(input, " \n");
    *command = token;
    int arg_index = 0;

    while (token != NULL) {
        arguments[arg_index] = token;
        arg_index++;
        token = strtok(NULL, " \n");
    }

    arguments[arg_index] = NULL;
}

void execute_command(char *command, char *arguments[]) { //ejecutar comando en un proceso hijo
    pid_t pid = fork();

    if (pid == 0) { //proceso hijo
		if (execvp(command, arguments) < 0) {
			printf("Comando ingresado no existe\n");
			exit(0);
		}
    } else if (pid > 0){ //proceso padre
        int status;
        waitpid(pid, &status, 0);
    } else {
		printf("error en fork");
        exit(1);
	}
}

int main() {
    char *input;
    char *command;
    char *arguments[100];

    while (1) {
        printf("mishell:$ ");
        input = read_input(); 

        if (strlen(input) <= 1) { //comando vacio (solo presionar enter)
            continue;
        }

        parse_input(input, &command, arguments);  //parsear entrada

        if (strcmp(command, "exit") == 0) { //terminar shell
            break;
        } else {
            execute_command(command, arguments);  //ejecutar comando
        }
    }
    return 0;
}