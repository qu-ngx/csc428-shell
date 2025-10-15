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
