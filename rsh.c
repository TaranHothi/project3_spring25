#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12               // number of allowed commands
#define MAX_ARGS 20        // maximum arguments per command
#define LINE_SIZE 256      // maximum input line length

extern char **environ;

char *allowed[N] = {
    "cp", "touch", "mkdir", "ls", "pwd",
    "cat", "grep", "chmod", "diff",
    "cd", "exit", "help"
};

//Return 1 if cmd is in allowed[], else 0
int isAllowed(const char *cmd) {
    for (int i = 0; i < N; i++) {
        if (strcmp(cmd, allowed[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

//Print the help listing
void print_help(void) {
    printf("The allowed commands are:\n");
    for (int i = 0; i < N; i++) {
        printf("%d: %s\n", i + 1, allowed[i]);
    }
}

int main(void) {
    char line[LINE_SIZE];
    char *argv[MAX_ARGS + 1];  

    while (1) {
        //Prompt on stderr
        fprintf(stderr, "rsh>");
        fflush(stderr);

        //Reads a line
        if (!fgets(line, LINE_SIZE, stdin))
            continue;
        if (strcmp(line, "\n") == 0)
            continue;

        //Strips trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        //Tokenize into argv[]
        int argc = 0;
        char *token = strtok(line, " ");
        while (token && argc < MAX_ARGS) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        if (argc == 0)
            continue;

        //The Built‑ins
        if (strcmp(argv[0], "exit") == 0) {
            return 0;
        }
        if (strcmp(argv[0], "help") == 0) {
            print_help();
            continue;
        }
        if (strcmp(argv[0], "cd") == 0) {
            if (argc > 2) {
                printf("-rsh: cd: too many arguments\n");
            } else if (argc == 2) {
                chdir(argv[1]);
            }
            continue;
        }

        //First 9 Commands
        if (isAllowed(argv[0])) {
            if (strcmp(argv[0], "cd") != 0 &&
                strcmp(argv[0], "exit") != 0 &&
                strcmp(argv[0], "help") != 0) {

                pid_t pid;
                int status;
                if (posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ) != 0) {
                    printf("NOT ALLOWED!\n");
                } else {
                    waitpid(pid, &status, 0);
                }
                continue;
            }
        }

        printf("NOT ALLOWED!\n");
    }

    return 0;
}
