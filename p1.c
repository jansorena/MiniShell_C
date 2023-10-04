#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <syslog.h>

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


//////////////
////daemon////
//////////////
void log_system_info() {
    FILE *cpuinfo_file = fopen("/proc/stat", "r"); //processes, procs_running y procs_blocked no estan en /proc/cpuinfo estan en /proc/stat
    if (cpuinfo_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    while (fgets(line, sizeof(line), cpuinfo_file)) {
        // Extract and log relevant information from /proc/cpuinfo.
        if (strstr(line, "processes") || strstr(line, "procs_running") || strstr(line, "procs_blocked")) {
            syslog(LOG_INFO, "%s", line);
        }
    }

    fclose(cpuinfo_file);
}

void start_daemon(int t, int p) {
    // Fork to create a daemon.
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent process (shell) displays a message.
        printf("Daemon started with PID: %d\n", pid);
        return; // Return to the shell.
    }

    // Child process (daemon) continues here.

    // Create a new session and detach from the terminal.
    setsid();

    // Set the umask to an appropriate value.
    umask(0);

    // Open the system log.
    openlog("system_info_daemon", LOG_PID, LOG_DAEMON);

    // Main loop to log system information.
	while (p > 0) {
		// Read and log system information from /proc/cpuinfo.
        // Example: read from /proc/cpuinfo, collect the required data, and use syslog to log it.
        log_system_info(); // Log system information from /proc/cpuinfo.

        // Sleep for 't' seconds.
        sleep(t);

        // Decrease the remaining time.
        p -= t;
    }

    // Clean up and exit.
    closelog();
}
//////////////
////daemon////
//////////////


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

	if (strcmp(commands[0][0], "exit") == 0) { //terminar shell
            break;
        } else if (strcmp(commands[0][0], "daemon") == 0) { //comando para daemon
		if (commands[0][1] == NULL && commands[0][2] == NULL) {
		printf("Uso: daemon <t> <p>\n");
		} else {
		int t = atoi(commands[0][1]);
		int p = atoi(commands[0][2]);
		start_daemon(t, p);
		}
	} else {
            execute_commands(commands, num_commands);  //ejecutar comandos
        }
    }
    return 0;
}
