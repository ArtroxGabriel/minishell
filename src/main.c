#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CMD_LEN 256
#define MAX_ARGS 32

// Array para armazenar PIDs de processos em background
pid_t bg_processes[10];
int bg_count = 0;
pid_t last_child_pid = 0; // Armazena PID do último processo filho

void check_error(int retval, char *msg) {
  if (retval < 0) {
    perror(msg);
    exit(retval);
  }
}

void parse_command(char *input, char **args, int *background) {
  int i = 0;
  char *delim = " ";

  args[i] = strtok(input, delim);
  while (args[i] != NULL)
    args[++i] = strtok(NULL, delim);
}

void execute_command(char **args, int background) {
  int retval = fork();

  check_error(retval, "Fork falhou");

  // processo filho
  if (retval == 0) {
    retval = execvp(args[0], args);
    check_error(retval, "Error ao executar comando externo");
    return;
  }

  // processo pai
  if (background == 0) {
    retval = wait(0);
    check_error(retval, "Error no wait");
  }
}

/// @brief Verify if the command is internal
/// @param args array of arguments, where args[0] is the command
/// @return 1 if internal command, 0 otherwise
int is_internal_command(char **args) {
  if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "pid") == 0 ||
      strcmp(args[0], "jobs") == 0 || strcmp(args[0], "wait") == 0)
    return 1;

  return 0;
}

void handle_internal_command(char **args) {
  char *command = args[0];

  if (strcmp(command, "exit") == 0) {
    printf("Shell encerrado!\n");
    exit(0);
  }

  if (strcmp(command, "pid") == 0) {
    pid_t pid = getpid();
    printf("%d %d\n", pid, bg_processes[0]);
    return;
  }

  // TODO: fazer quando for lidar com background
  if (strcmp(command, "jobs") == 0) {
    printf("job macedo\n");
    return;
  }

  // TODO: fazer quando for lidar com background
  if (strcmp(command, "wait") == 0) {

    printf("wait\n");
    return;
  }

  perror("Comando interno desconhecido");
  exit(1);
}

int main() {
  char input[MAX_CMD_LEN];
  char *args[MAX_ARGS];
  int background;

  printf("Mini-Shell iniciado (PID: %d)\n", getpid());
  printf("Digite 'exit' para sair\n\n");
  while (1) {
    printf("minishell> ");
    fflush(stdout);

    // Ler entrada do usuário
    if (!fgets(input, sizeof(input), stdin)) {
      break;
    }

    // Remover quebra de linha
    input[strcspn(input, "\n")] = 0;

    // Ignorar linhas vazias
    if (strlen(input) == 0) {
      continue;
    }

    // Fazer parsing do comando
    parse_command(input, args, &background);

    // Executar comando
    if (is_internal_command(args)) {
      handle_internal_command(args);
    } else {
      execute_command(args, background);
    }
  }

  return 0;
}
