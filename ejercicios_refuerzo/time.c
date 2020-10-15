#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("time: time [COMMAND]\n");
        return 1;
    }
    //Declaraciones para el tiempo inicial y final
    struct timeval inicial, final;
    //Se captura el primer tiempo
    gettimeofday(&inicial, NULL);
    //Fork
    int pid = fork(), status;
    //Si el fork falla
    if (pid < 0)
    {
        fprintf(stderr, "fork failed\n");
        return 1;
    }
    //Codigo del hijo
    if (pid == 0)
    {
        //funcion exec que correr el comando del sistema ingresado como argv[1]
        execlp(argv[1], argv[1], NULL);
        return 1;
    }
    else
    {
        //Codigo del padre
        wait(&status);
        //Si el proceso hijo terminó correctamente se toma el tiempo final y se imprime el tiempo total
        if (WIFEXITED(status) != 0)
        {
            gettimeofday(&final, NULL);
            printf("Elapsed time: %ld.%ld\n", final.tv_sec - inicial.tv_sec, final.tv_usec - inicial.tv_usec);
        }
    }
    return 0;
}