#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

char *read_input() { //leer una linea de entrada
    char *input = NULL;
    size_t input_size = 0;
    getline(&input, &input_size, stdin);
    return input;
}

void parse_input(char *input, char *commands[][100], int *num_commands) {
    *num_commands = 0;
    char* ptr_pipes;
    char* token = strtok_r(input,"|\n", &ptr_pipes);
    while (token != NULL) {
        int arg_index = 0;
        while (*token == ' ')  {
            ++token;
        }
        char* ptr_cmds;
        char* command_token = strtok_r(token, " ", &ptr_cmds);

        while (command_token != NULL) {
            commands[*num_commands][arg_index++] = command_token;
            command_token = strtok_r(NULL, " ", &ptr_cmds);
        }
        commands[*num_commands][arg_index] = NULL;
        (*num_commands)++;
        token = strtok_r(NULL,"|\n",&ptr_pipes);
    }
}

void execute_commands(char *commands[][100], int num_commands) {
    int i, prev_pipe, fd[2];
    prev_pipe = STDIN_FILENO;
    pid_t pids[num_commands]; // Almacenar los PID de los procesos hijos

    for (i = 0; i < num_commands; i++) {
        if (pipe(fd) == -1) {
            perror("Error al crear el pipe");
            exit(EXIT_FAILURE);
        }
        pid_t pid = fork();
        if (pid == -1) {
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        }else if (pid == 0) { // Proceso hijo
            // Redireccionar el pipe anterior a stdin
            dup2(prev_pipe, STDIN_FILENO);
            if(i != num_commands - 1){
                // Redireccionar stdout al pipe actual
                dup2(fd[1], STDOUT_FILENO);
            }
            close(fd[0]);
            // Ejecutar el comando actual
            execvp(commands[i][0], commands[i]);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }else{
            wait(NULL);
            // Cerrar el extremo de escritura del pipe actual (no es necesario en el proceso padre)
            close(fd[1]);
            // Guardar el extremo de lectura del pipe actual para la próxima iteración
            prev_pipe = fd[0];
        }    
    }
}

int main() {
    char *input;
    char *commands[100][100];
    int num_commands;

    while (1) {
        printf("mishell:$ ");
        input = read_input(); 

        if (strlen(input) <= 1) { //comando vacio (solo presionar enter)
            continue;
        }
        parse_input(input, commands, &num_commands);  //parsear entrada

        execute_commands(commands, num_commands);
    }

    return 0;
}
