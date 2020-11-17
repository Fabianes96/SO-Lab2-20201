#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


#define n 1000

int interactiveMode();

int batchMode(char *fileName);

int execute(char *const command);

char **path;

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
        } else if (strstr(str, cd) != NULL) {
            printf("Esto es cd");
        } else if (strstr(str, path) != NULL) {
            printf("Esto es path");
        } else {
            execute(str);
        }

    } while (1);

    return 0;
}

int batchMode(char *fileName) {
    printf("Estamos en modo bacht %s", fileName);
}

int execute(char * command) {


//    path = (char **)realloc(path, (pathLen) * sizeof(char *));
    int pid = fork(), status;
    if (pid < 0) {
        fprintf(stderr, "fork failed\n");
        return 1;
    }
    if (pid == 0) {

//        char **arguments;
//        char delimitador[] = " ";
//        arguments[0] = strtok(command, delimitador);
//        arguments[2] = strtok(command, delimitador);
//        arguments[3] = strtok(command, delimitador);
//
//        //obtengo la direccion del ejecutable
//        int fullPathLen = strlen(path[0]) + strlen(arguments[0]);
//        char fullPath[fullPathLen + 1];
//        strcpy(fullPath, path[0]);
//        strcat(fullPath, "/");
//        strcat(fullPath, arguments[0]);
//        int successs = execv(fullPath, arguments);
//        if (successs < 0 && paralel == 0)
//        {
//            exit(1);
//        }


        char *myargs[3];
        /*
         * myargs[0] = strdup("wc");   // program: "wc" (word count)
        myargs[1] = strdup("p3.c"); // argument: file to count
        myargs[2] = NULL;           // marks end of array
         */
        myargs[0] = strdup("ls");
        myargs[1] = strdup("-la");
        myargs[2] = NULL;
        execvp(myargs[0], myargs);  // runs word count


//char *const argv2[] = {'ls', '-la', NULL};
//execv("/bin/ls", argv2);


//execlp(command, "-ls", NULL);
//        char *prog[] = { command, "-la", NULL };
//        execv("/bin/ls", prog);


//        if ((execvp(*command,'\0')==-1)) {
//            printf("invalid command\n $s", command);
//            // exit(1);
//        }

//        printf("My command is:%s", command);
//        int aaa = execlp("/bin/ls","ls",NULL);
//        int aaa = execlp("/bin/ls","ls",NULL);

//        int aaa = execlp(command, command, NULL);
//        printf("int vale : %i.", aaa);
        return 1;
    } else {
        wait(&status);
        if (WIFEXITED(status) != 0) {
//            printf("padre ok\n");
        }
    }
    return 0;
}
