#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 256
#define PATH_SEP ':'

void trim_newline(char *s) {
  if (!s)
    return;
  size_t n = strlen(s);
  if (n && s[n - 1] == '\n') {
    s[n - 1] = '\0';
  } // remove the trailing end (\n) and mark it as NULL (\0) to terminate the
    // command
}

int split_args(char *line, char *argv[],
               int maxv) { // argument vector to store all arguments in line
  int argc = 0;
  char *saveptr = NULL;
  char *tok = strtok_r(
      line, " \t\r\n",
      &saveptr); // use strok to tokenize the string and store it to saveptr

  // traverse token like a linkedlist
  while (tok != NULL && argc < maxv - 1) {
    argv[argc] = tok;
    tok = strtok_r(NULL, " \t\r\n", &saveptr);
    argc++;
  }

  argv[argc] = NULL;
  return argc;
}

// define all shell supports
int is_builtin(const char *cmd) {
  return cmd && (strcmp(cmd, "exit") == 0 || strcmp(cmd, "echo") == 0 ||
                 strcmp(cmd, "pwd") == 0 || strcmp(cmd, "cd") == 0 ||
                 strcmp(cmd, "clear") == 0);
}

int is_number(const char *s) {
  if (s == NULL || *s == '\0')
    return 0;
  if (*s == '+' || *s == '-')
    s++;

  if (*s == '\0')
    return 0;

  while (*s != '\0') {
    unsigned char ch = (unsigned char)*s;
    if (!isdigit(ch)) {
      return 0;
    }
    s++;
  }

  return 1;
}

void sh_exit(char *args[]) {
  int code = 0;
  if (args[0] && is_number(args[0]))
    code = atoi(args[0]);
  else if (args[0])
    code = 1;
  exit(code);
}

void sh_echo(char *args[]) {
  int i = 0;
  while (args[i]) {
    if (i)
      putchar(' ');
    fputs(args[i], stdout);
    i++;
  }
  putchar('\n');
}

void sh_pwd(void) {
  char buf[4096];
  if (getcwd(buf, sizeof(buf)))
    puts(buf);
  else
    perror("pwd");
}

void sh_cd(char *args[]) {
  const char *dir = args[0];
  if (!dir) {
    fprintf(stderr, "cd: please specifiy a directory");
    return;
  }

  if (strcmp(dir, "~") == 0) {
    const char *home = getenv("HOME");
    if (home)
      dir = home;
  }

  if (chdir(dir) != 0)
    fprintf(stderr, "cd: %s: %s\n", dir, strerror(errno));
}

void sh_clear(void) {
  pid_t pid = fork();
  if (pid == 0) {
    execlp(
        "clear", "clear",
        (char *)
            NULL); // replace the current process with new image by using clear
    perror("clear");
    _exit(127); // clean out - 127 command not found
  } else if (pid > 0) {
    int st;
    (void)waitpid(pid, &st, 0);
  } else {
    perror("fork");
  }
}

void run_child(char *cmd, char *argv[]) {
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return;
  }

  if (pid == 0) {
    execvp(cmd, argv);
    fprintf(stderr, "%s: command not found\n", cmd);
    _exit(127);
  }

  int status = 0;
  if (waitpid(pid, &status, 0) < 0)
    perror("waitpid");
}

// END HELPER

int main(void) {
  char *line = NULL;
  size_t cap = 0;

  while (true) {
    fputs("$ ", stdout);
    fflush(stdout);

    ssize_t n =
        getline(&line, &cap,
                stdin); /* read from stdin to store in a string: line and cap */

    if (n < 0) {
      if (feof(stdin)) { /* feof check end of file, stdin - FILE* obj */
        putchar('\n');   /* if end of file obj, append "\n" to stdin stream */
        break;
      }
      perror("getline");
      continue;
    }

    trim_newline(
        line); // TODO: Implement this, trim the /n (and make it /0 (NULL))

    if (line[0] == '\0')
      continue; /* case empty enter */

    char *argv[MAX_ARGS];
    int argc =
        split_args(line, argv, MAX_ARGS); // TODO: Implement this, return an int

    if (argc == 0)
      continue;

    char *mode = argv[0];

    // TODO: COMPARE MODE: Implement this every mode + invalid mode
    if (strcmp(mode, "exit") == 0)
      sh_exit(&argv[1]);
    else if (strcmp(mode, "echo") == 0)
      sh_echo(&argv[1]);
    else if (strcmp(mode, "pwd") == 0)
      sh_pwd();
    else if (strcmp(mode, "cd") == 0)
      sh_cd(&argv[1]);
    else if (strcmp(mode, "clear") == 0)
      sh_clear();
    else
      fprintf(stderr, "%s: command not supported\n", mode);
  }

  free(line);
  return 0;
}
