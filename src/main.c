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

void check_error(int, char *);
void add_bg_process(pid_t pid);
void clean_finished_processes();

void parse_command(char *input, char **args, int *background) {
  int i = 0;
  char *delim = " ";

  args[i] = strtok(input, delim);
  while (args[i] != NULL)
    args[++i] = strtok(NULL, delim);

  if (i > 0 && strcmp(args[i - 1], "&") == 0) {
    *background = 1;
    args[i - 1] = NULL;
  }
}

void execute_command(char **args, int background) {
  pid_t pid = fork();

  int retval = pid;
  check_error(retval, "Fork falhou");

  // processo filho
  if (retval == 0) {
    retval = execvp(args[0], args);
    check_error(retval, "Error ao executar comando externo");
    return;
  }

  // processo pai
  if (background) {
    add_bg_process(pid);
    printf("[%d] %d\n", bg_count, pid);
  } else {
    retval = wait(NULL);
    check_error(retval, "Error no wait");
  }
}

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

  if (strcmp(command, "jobs") == 0) {
    if (bg_count == 0) {
      printf("Nenhum processo em background\n");
      return;
    }

    printf("Processos em background:\n");
    for (register int i = 0; i < bg_count; i++)
      printf("[%d] %d Running\n", i + 1, bg_processes[i]);

    return;
  }

  if (strcmp(command, "wait") == 0) {
    printf("Aguardando processo em background\n");
    while (bg_count > 0) {
      int status;
      pid_t pid = wait(&status);

      for (register int i = 0; i < bg_count; i++) {
        if (pid != bg_processes[i])
          continue;

        for (register int j = i; j < bg_count - 1; j++) {
          bg_processes[j] = bg_processes[j + 1];
        }
        bg_count--;
        break;
      }
    }

    printf("Todos os processos terminaram\n");
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
    clean_finished_processes();
    background = 0;

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

void check_error(int retval, char *msg) {
  if (retval < 0) {
    perror(msg);
    exit(retval);
  }
}

void add_bg_process(pid_t pid) {
  if (bg_count < 10) {
    bg_processes[bg_count++] = pid;
    last_child_pid = pid;
  }
}

void clean_finished_processes() {
  int status;
  pid_t pid;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    for (register int i = 0; i < bg_count; i++) {
      if (bg_processes[i] == pid) {
        printf("[%d]+ Done\n", i + 1);
        for (register int j = i; j < bg_count - 1; j++) {
          bg_processes[j] = bg_processes[j + 1];
        }
        bg_count--;
        break;
      }
    }
  }
}
