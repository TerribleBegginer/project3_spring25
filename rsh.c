#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MAX_ARGS 21  // 20 arguments + NULL
#define N 12

extern char **environ;

char *allowed[N] = {
    "cp", "touch", "mkdir", "ls", "pwd", "cat", "grep", "chmod", "diff",
    "cd", "exit", "help"
};


int isAllowed(const char *cmd) {
    for (int i = 0; i < N; i++) {
        if (strcmp(allowed[i], cmd) == 0) {
            return i;
        }
    }
    return -1;
}

void printHelp() {
    printf("The allowed commands are:\n");
    for (int i = 0; i < N; i++) {
        printf("%d: %s\n", i + 1, allowed[i]);
    }
}

int main() {
    char line[256];

    while (1) {
        fprintf(stderr, "rsh>");
        if (fgets(line, sizeof(line), stdin) == NULL) continue;
        if (strcmp(line, "\n") == 0) continue;

        line[strcspn(line, "\n")] = '\0';

      
        char *argv[MAX_ARGS];
        int argc = 0;
        char *token = strtok(line, " ");
        while (token != NULL && argc < MAX_ARGS - 1) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        if (argc == 0) continue;

        int cmdIndex = isAllowed(argv[0]);

        if (cmdIndex == -1) {
            printf("NOT ALLOWED!\n");
            continue;
        }

     
        if (strcmp(argv[0], "exit") == 0) {
            return 0;
        } else if (strcmp(argv[0], "help") == 0) {
            printHelp();
        } else if (strcmp(argv[0], "cd") == 0) {
            if (argc > 2) {
                printf("-rsh: cd: too many arguments\n");
            } else {
                const char *target = argc == 2 ? argv[1] : getenv("HOME");
                if (chdir(target) != 0) {
                    perror("cd failed");
                }
            }
        } else {
          
            pid_t pid;
            int status;
            posix_spawnattr_t attr;

            posix_spawnattr_init(&attr);
            if (posix_spawnp(&pid, argv[0], NULL, &attr, argv, environ) != 0) {
                perror("spawn failed");
            } else {
                waitpid(pid, &status, 0);
            }
            posix_spawnattr_destroy(&attr);
        }
    }

    return 0;
}
