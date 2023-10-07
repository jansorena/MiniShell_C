#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <syslog.h>

char* read_input() { // leer una linea de entrada
  char* input = NULL;
  size_t input_size = 0;
  getline(&input, &input_size, stdin);
  return input;
}

void parse_input(char* input, char* commands[][100], int* num_commands) { // parsear entrada
  *num_commands = 0;
  char* ptr_pipes;
  char* token = strtok_r(input, "|\n", &ptr_pipes); // primer strtok divide en comandos entre pipes
  while (token != NULL) {
    int arg_index = 0;
    while (*token == ' ') {
      ++token;
    }
    char* ptr_cmds;
    char* command_token = strtok_r(token, " ", &ptr_cmds); // divide cada uno en un comando y sus argumentos
    while (command_token != NULL) {
      commands[*num_commands][arg_index++] = command_token;
      command_token = strtok_r(NULL, " ", &ptr_cmds);
    }
    commands[*num_commands][arg_index] = NULL;
    (*num_commands)++;
    token = strtok_r(NULL, "|\n", &ptr_pipes);
  }
}

void execute_commands(char* commands[][100], int num_commands) { //ejecutar comando o comandos
  int i, prev_pipe, fd[2]; // una pipe y una variable para guardar la lectura anterior
  prev_pipe = STDIN_FILENO; // standard input

  for (i = 0; i < num_commands; i++) {
    if (pipe(fd) == -1) {
      perror("Error al crear el pipe");
      exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == -1) {
      perror("Error al crear el proceso hijo");
      exit(EXIT_FAILURE);
    } else if (pid == 0) { // Proceso hijo
      dup2(prev_pipe, STDIN_FILENO); // Redireccionar stdin al pipe anterior
      if (i != num_commands - 1) {
        dup2(fd[1], STDOUT_FILENO); // Redireccionar stdout al pipe lectura actual
        close(fd[1]);
      }
      close(fd[0]);
      if (execvp(commands[i][0], commands[i]) < 0) { // Ejecutar el comando actual
        perror("Comando ingresado no existe");
        exit(EXIT_FAILURE);
      }
    } else {
      wait(NULL); // Esperar hijos
      close(fd[1]); // Cerrar el extremo de escritura del pipe actual (no es necesario en el padre)
      prev_pipe = fd[0]; // Guardar el extremo de lectura del pipe actual para la próxima iteración
    }
  }
}

void start_daemon(int t, int p) { // Ejecutar el daemon
  pid_t pid = fork(); // Fork para crear el daemon
  if (pid < 0) {
    printf("error en fork\n");
    exit(EXIT_FAILURE);
  } else if (pid > 0) { // Proceso padre
    printf("Daemon creado con PID: %d\n", pid);
    return; // Retorna a la shell.
  }
  // Proceso hijo (Daemon)
  setsid(); // Crea una nueva sesion y se desprende de la terminal (si se cierra la terminal no se mata el daemon)
  umask(0); // Archivos utilizados por el daemon tienen los permisos necesarios
  openlog("mi_daemon", LOG_PID, LOG_DAEMON); // logea en syslog con nombre "mi_daemon"
  while (p > 0) {
    // Recopilar la informacion necesaria de /proc/stat y luego registra en syslog
    FILE* cpuinfo_file = fopen("/proc/stat", "r"); //processes, procs_running y procs_blocked no estan en /proc/cpuinfo estan en /proc/stat
    if (cpuinfo_file == NULL) {
      perror("fopen");
      exit(EXIT_FAILURE);
    }
    char line[1024];
    while (fgets(line, sizeof(line), cpuinfo_file)) {
      // Extraer informacion relevante
      if (strstr(line, "processes") || strstr(line, "procs_running") || strstr(line, "procs_blocked")) { //si la linea corresponde a processes, procs_running o procs_blocked, la logea en syslog
        syslog(LOG_INFO, "%s", line); // escribe en syslog
      }
    }
    fclose(cpuinfo_file);
    sleep(t); // Duerme por t segundos
    p -= t; // Decrementa el tiempo total
  }
  closelog(); // Cerrar el log y terminar
}

int main() {
  char* input;
  char* commands[100][100];
  int num_commands;
  while (1) {
    printf("mishell:$ ");
    input = read_input();
    if (strlen(input) <= 1) { //comando vacio (solo presionar enter)
      continue;
    }
    parse_input(input, commands, &num_commands); //parsear entrada
    if (strcmp(commands[0][0], "exit") == 0) { //terminar shell
      break;
    } else if (strcmp(commands[0][0], "daemon") == 0) { //comando para ejecutar daemon
      if (commands[0][1] == NULL && commands[0][2] == NULL) {
        printf("Uso: daemon <t> <p>\n");
      } else {
        int t = atoi(commands[0][1]);
        int p = atoi(commands[0][2]);
        start_daemon(t, p);
      }
    } else {
      execute_commands(commands, num_commands); //ejecutar comando o comandos
    }
  }
  return 0;
}
