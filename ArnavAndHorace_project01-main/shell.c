#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "tokens.h"
#include <sys/wait.h>

#define CAP_STEP 256

int limit = 255;
char **previous;

char* read_line() {
  char *line = NULL;
    size_t buflen = 0;
    int f;
    f = getline(&line, &buflen, stdin);
    return line;
}

void execute(char **tokens) {
  char  *current;
  char  *stringDir;
  char  *path;
  char buf[256];

  if(strcmp(tokens[0], "prev") == 0) {
    execute(previous);
    return;
  } else {
    previous = tokens;
  }
  if (strcmp(tokens[0], "exit") == 0) {
    printf("Bye bye.\n");
    free(previous);
    exit(0);
    return;
  } else if(strcmp(tokens[0], "help") == 0){
      printf("The built in commands in the program are as follows :\n 1.cd - Changes the directory that the user is in\n 2. source - Executes a script.\n 3. help - tells us what all the built in commands do\n 4. prev - prints and executes the previous command line\n");
  return;
  } else if (strcmp(tokens[0], "cd") == 0) {
      if(tokens[1] == NULL) {
       chdir(getenv("HOME"));
       return;
      }
      getcwd(buf, 256);
      stringDir = strcat(buf, "/");
      path = strcat(stringDir, tokens[1]);
     chdir(path);
    return;
  }
  
  pid_t pid = fork();
  
  if (pid == 0) {
    execvp(tokens[0], tokens);
    printf("%s: command not found\n", tokens[0]);
    exit(0);
  } else if (pid > 0) {
    int status;
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  } else {
    perror("problem forking");
  }
}

  
int main(int argc, char **argv) {
printf("Welcome to mini-shell.\n");// prints the welcome message
  while (1) {
    printf("shell $: ");
    //char input[256];
    //fgets(input, 256, stdin);
    //char **chars = get_tokens(input);
    char *commandLine = read_line();
    char *stringSplit = strtok(commandLine, ";");
    while (stringSplit != NULL) {
    char **tokens = get_tokens(stringSplit);    
    if (strcmp(tokens[0], "source") == 0) {
      FILE *file = fopen(tokens[1], "r");
      char line[256];
      while(fgets(line, sizeof(line), file) != NULL) {
        char **tokens2 = get_tokens(line);
        execute(tokens2);
      }
    } else if (tokens[0] != NULL) {
       execute(tokens);   
    }
    stringSplit = strtok(NULL, ";");
    }
    free(commandLine);
    free(stringSplit);
    //free(input);
  }
  return 0;
}
