#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>   
#include <sys/wait.h> 
#include <unistd.h>  
#include <string.h> 

#define MAX_COMMANDS 12
#define MAX_ARGS 20
#define LINE_SIZE 256

extern char **environ;  

//List of allowed commands in rsh
char *allowed[N] = {"cp","touch","mkdir","ls","pwd","cat","grep","chmod","diff","cd","exit","help"};

//Check if a command is in the allowed list
int isAllowed(const char *cmd) {
    for (int i = 0; i < MAX_COMMANDS; i++) {
        if (strcmp(cmd, allowed[i]) == 0) {
            return 1;  // If found
        }
    }
    return 0;  // If not found
}

//Print the help message listing allowed commands
void print_help(void) {
    printf("The allowed commands are:\n");
    for (int i = 0; i < MAX_COMMANDS; i++) {
        printf("%d: %s\n", i + 1, allowed[i]);
    }
}

int main(void) {
    char line[LINE_SIZE];
    char *argv[MAX_ARGS + 1];  

    while (1) {
        //Prints shell prompt to stderr so tests ignore it
        fprintf(stderr, "rsh>");
        fflush(stderr);

        //Reads a line from stdin
        if (!fgets(line, LINE_SIZE, stdin)) {
            continue;  
        }
        //Skips blank lines
        if (strcmp(line, "\n") == 0) {
            continue;
        }

        //Removes trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        //Splits the line into tokens separated by spaces
        int argc = 0;
        char *token = strtok(line, " ");
        while (token != NULL && argc < MAX_ARGS) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;  //argv must end with NULL

        //If no command entered, just reprompt
        if (argc == 0) {
            continue;
        }

        //Handles any built-in commands
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

        //Handles external commands 
        if (isAllowed(argv[0])) {
            if (strcmp(argv[0], "cd") != 0 &&
                strcmp(argv[0], "exit") != 0 &&
                strcmp(argv[0], "help") != 0) {

                pid_t pid;
                int status;
                //Will spawn child process to run the command
                if (posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ) != 0) {
                    printf("NOT ALLOWED!\n");
                } else {
                    waitpid(pid, &status, 0);
                }
                continue;
            }
        }

        //If command not allowed, print error
        printf("NOT ALLOWED!\n");
    }

    return 0;
}
