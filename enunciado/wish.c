#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


#define n 1000

int interactiveMode();

int batchMode(char *fileName);

int execute(char *fileName);

int main(int argc, char *argv[]) {
    if (argc == 1) {
        interactiveMode();
    }
    if (argc == 2) {
        batchMode(argv[1]);
    } else {
        //error
    }
    return 0;
}

int interactiveMode() {

    char salir[n] = "exit";
    char cd[n] = "cd";
    char path[n] = "path";

    do {
        char str[n];
        printf("wish> ");
        fgets(str, n, stdin);

        if (strstr(str, salir) != NULL) {
            return 0;
        }

        if (strstr(str, cd) != NULL) {
            printf("Esto es cd");
        }

        if (strstr(str, path) != NULL) {
            printf("Esto es path");
        }
        execute(str);

    } while (1);

}

int batchMode(char *fileName) {
    printf("Estamos en modo bacht %s", fileName);
}

int execute(char *command) {
    int pid = fork(), status;
    if (pid < 0) {
        fprintf(stderr, "fork failed\n");
        return 1;
    }
    if (pid == 0) {
         int aaa = execlp(command, command, NULL);
         printf("int vale : %i. %s", aaa, errno);
        return 1;
    } else {
        wait(&status);
    }
    return 0;
}