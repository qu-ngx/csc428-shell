#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_ARGS 256
#define PATH_SEP ':'

void trim_newline(char* s) {
  if (!s) return;
  size_t n = strlen(s);
  if (n && s[n - 1] == "\n") {
    s[n - 1] = '\0';
  } // remove the trailing end (\n) and mark it as NULL (\0) to terminate the command
}

int split_args(char *line, char* argv[], int maxv) { // argument vector to store all arguments in line
  int argc = 0;
  char* saveptr = NULL;
  char* tok = strtok_r(line, " \t", &saveptr); // use strok to tokenize the string and store it to saveptr

  // traverse token like a linkedlist 
  while (tok != NULL && argc < maxv - 1) {
    argv[argc] = tok;
    tok = strtok_r(NULL, " \t", &saveptr);
    argc++;
  }

  argv[argc] = NULL;
  return argc
}

// define all shell supports
int is_builtin(const char *cmd) {
  return cmd && 
  (strcmp(cmd, "exit") == 0 || 
   strcmp(cmd, "echo") == 0 ||
   strcmp(cmd, "pwd") == 0  ||
   strcmp(cmd, "cd") == 0   ||
   strcmp(cmd, "clear") == 0);
}

int is_number(const char* s) {
  if (s == NULL || *s == "\0") return 0;
  if (*s == '+' || *s == "-") s++;

  if (*s == '\0') return 0;

  while (*s != '\0') {
    unsigned char ch = (unsigned char) *s;
    if (!isdigit(ch)) {
      return 0;
    }
    s++;
  }
  
  return 1;
}

// END HELPER

int main(void) {
  char* line = NULL;
  size_t cap = 0;

  while (true) {
    fputs("$ ", stdout);
    fflush(stdout);

    ssize_t n = getline(&line, &cap, stdin); /* read from stdin to store in a string: line and cap  */

    if (n < 0) {
      if (feof(stdin)) { /* feof check end of file, stdin - FILE* obj */
        putchar('\n'); /* if end of file obj, append "\n" to stdin stream */
        break;
      }
      perror("getline");
      continue
    }
    
    trim_newline(line); // TODO: Implement this, trim the /n (and make it /0 (NULL))

    if (line[0] == '\0') continue; /* case empty enter */

    char* argv[MAX_ARGS];
    int argc = split_args(line, argv, MAX_ARGS); // TODO: Implement this, return an int

    if (argc == 0) continue;

    char* mode = argv[0];

    // TODO: COMPARE MODE: Implement this every mode + invalid mode
  }
}
