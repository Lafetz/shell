#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define DIM_CMDS "&"
#define DIM_CMD ">"
#define DIM_LS " \t\r\n\a"
extern char *strdup(const char *);

void printArr(char **arr) {
  for (int j = 0; arr[j] != NULL; j++) {
    printf("token: %s \n", arr[j]);
  }
}
char **split(char *input, char *dim) {
  char **tokens = malloc(1024 * sizeof(char *));
  if (!tokens) {
    fprintf(stderr, "malloc failed\n");
    exit(EXIT_FAILURE);
  }

  char *token = strtok(input, dim);
  int i = 0;
  while (token != NULL) {
    tokens[i] = token;
    i++;

    token = strtok(NULL, dim);
  }
  tokens[i] = NULL;

  return tokens;
}
void exeCommand(char **arr, char *path) {
  if (path == NULL) {
    printf("path is null\n");
    exit(EXIT_FAILURE);
    return;
  }
  char *command = strdup(path);
  strcat(command, "/");
  strcat(command, arr[0]);
  if (execv(command, arr) == -1) {
    perror("execv\n");
    exit(1);
  }
}
int main() {
  char *path = "/bin";
  char *input;
  puts("A Very Basic Shell v1.0.0");
  while (1) {
    fputs("wish> ", stdout);
    int read;
    size_t len = 0;
    read = getline(&input, &len, stdin);
    if (read == -1) {
      printf("Error reading line or end of file.\n");
      exit(1);
    }
    char **cmds = split(input, DIM_CMDS);

    for (int i = 0; cmds[i] != NULL; i++) {
      char **cmd = split(cmds[i], DIM_CMD);
      char **cmdTokens = split(cmd[0], DIM_LS);
      // check for builtin
      if (strcmp(cmdTokens[0], "exit") == 0) {
        exit(0);

      } else if (strcmp(cmdTokens[0], "cd") == 0) {
        if (chdir(cmdTokens[1]) == 0) {
        } else {
          perror("chdir() error");
        }
        continue;
      } else if (strcmp(cmdTokens[0], "path") == 0) {
        path = strdup(cmdTokens[1]);
        continue;
      }
      //

      int pid = fork();
      if (pid < 0) {
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
      } else if (pid == 0) {
        if (cmd[1] != NULL) {
          char output[64] = "./";
          char **trimed = split(cmd[1], " ");
          strcat(output, trimed[0]);
          int fd = open(output, O_CREAT | O_WRONLY | O_TRUNC, 0644);
          if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2");
            close(fd);
            exit(EXIT_FAILURE);
          }
        }
        exeCommand(cmdTokens, path);
      } else {
        wait(&pid);
      }
    }
  }
  return 0;
}
