#include <stdio.h>
#include <stdlib.h>
#include <string.h>   
#include <unistd.h>   
#include <sys/wait.h> 
#include <fcntl.h>

char SHELL_NAME[50] = "wish";
int QUIT = 0;
static char error[25] = "An error has occurred\n";
char **paths;
int lenghtPaths = 1;
int pids = 1;
int ejecutar(char **comandos);
int redireccionar(char **comandos, int index);
char **copiarComando(int start, int end, char **command);
int contRed(char **comandos, int len);
void selectCommand(char **comandos, int redir);
int comando_cd(char **args);
int comando_exit(char **args);
int agregarPath(char **args);
char *array_cmd[] = {"cd", "exit", "path"};
int (*array_func[])(char **) = {&comando_cd, &comando_exit, &agregarPath};
char *leerLinea();
char **splitLinea(char *line);
int execShell(char **args, char *copia);

int main(int argc, char **argv){

  char *bin = "/bin";
  paths = (char **)malloc(3 * sizeof(char *));
  paths[lenghtPaths - 1] = bin;
  
  if (argc == 1){
    char *line;
    char *copia;
    char **args;
    while (QUIT == 0){
      printf("%s> ", SHELL_NAME);
      line = leerLinea();
      copia = strdup(line);
      args = splitLinea(line);
      execShell(args, copia);
      free(line);
      free(args);
      free(copia);
    }
  } else if (argc == 2){
    FILE *fptr;
    char line[200];
    char **args;
    char *copia;
    fptr = fopen(argv[1], "r");
    if (fptr == NULL){
      write(STDERR_FILENO, error, strlen(error) * sizeof(char));
      return 1;
    }else{
      while (fgets(line, sizeof(line), fptr) != NULL){
        copia = strdup(line);
        args = splitLinea(line);
        execShell(args, copia);
      }
    }
    free(args);
    free(copia);
    fclose(fptr);
  }else{
    write(STDERR_FILENO, error, strlen(error) * sizeof(char));
    return 1;
  }
  return EXIT_SUCCESS;
}
//Funcion para lectura de lineas
char *leerLinea(){
  char *line = (char *)malloc(sizeof(char) * 1024);
  char c;
  int pos = 0, bufsize = 1024;
  if (!line){
    write(STDERR_FILENO, error, strlen(error) * sizeof(char));
    exit(1);
  }
  while (1){
    c = getchar();
    if (c == EOF || c == '\n'){
    
      line[pos] = '\0';
      return line;
    }
    else{
      line[pos] = c;
    }
    pos++;    
    if (pos >= bufsize){
      bufsize += 1024;
      line = realloc(line, sizeof(char) * bufsize);
      if (!line){      
        write(STDERR_FILENO, error, strlen(error) * sizeof(char));
        exit(1);
      }
    }
  }
}
//Divide la linea en comandos separados
char **splitLinea(char *line){

  char **tokens = (char **)malloc(sizeof(char *) * 64);
  char *token;
  char delim[10] = " \t\n\r\a";
  int pos = 0, bufsize = 64;
  pids = 1;

  if (!tokens){
    write(STDERR_FILENO, error, strlen(error) * sizeof(char));
    exit(1);
  }
  token = strtok(line, delim);
  while (token != NULL){
    if (strcmp(token, "&") == 0){
      pids++;
    }
    tokens[pos] = token;
    pos++;
    if (pos >= bufsize){
      bufsize += 64;
      line = realloc(line, bufsize * sizeof(char *));
      if (!line){
      // Buffer Allocation Failed
        write(STDERR_FILENO, error, strlen(error) * sizeof(char));
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, delim);
  }  
  tokens[pos] = NULL;
  return tokens;
}

int numBuiltin(){
  return sizeof(array_cmd) / sizeof(char *);
}

int comando_cd(char **args){

  if (args[1] == NULL){
    write(STDERR_FILENO, error, strlen(error) * sizeof(char));
  }else{
    if (chdir(args[1]) != 0){
      write(STDERR_FILENO, error, strlen(error) * sizeof(char));
    }
  }
  return 1;
}

int comando_exit(char **args){
  if (args[1] == NULL){
    QUIT = 1;
    exit(0);
  }
  write(STDERR_FILENO, error, strlen(error) * sizeof(char));
  return 1;
}
int contadorComandos(char *line){

  int count = 1;
  int length = strlen(line);

  for (int i = 1; i < length; i++){
    if (line[i] != ' ' && line[i - 1] == ' '){
      count++;
    }
  }
  return count;
}
void elegirComando(char** comandos, int red){
  for (size_t i = 0; i < numBuiltin(); i++){
      if (strcmp(comandos[0], array_cmd[i]) == 0){
        (*array_func[i])(comandos);  
      }       
  }
  if (strcmp(comandos[0], "cd") != 0 && strcmp(comandos[0], "path") != 0 && strcmp(comandos[0], "exit") != 0){
    if (red > 0){
      redireccionar(comandos, red);
    }
    else{
      ejecutar(comandos);
    }                         
  }
}

int execShell(char **args, char *copia){
  if (args[0] == NULL){    
    return 1;
  } 
  int ex = 0;
  int pid[pids];
  char *instrucciones;
  while (1){
    instrucciones = strsep(&copia, "&");
    if (instrucciones == NULL){
      break;
    }
    char *found;
    int x = 0;
    int aux = 0;
    int contador = contadorComandos(instrucciones);
    char *comandos[contador];
    int length = strlen(instrucciones);
    instrucciones[length] = '\0';
    for (int w = 0; w < length; w++){
      if (instrucciones[w] == '\t' || instrucciones[w] == '\n'){
        instrucciones[w] = ' ';
      }        
    }        
    do {
      found = strsep(&instrucciones, " ");
      if (found != NULL && strlen(found) > 0){
        aux = 1;
        comandos[x++] = found;
      }
    } while (found != NULL);

    if (aux == 1){
      comandos[x] = NULL;
      int red = contRed(comandos, x);
      if (pids > 1){
        if ((pid[ex++] = fork()) == 0){
          elegirComando(comandos,red);
          exit(0);
        }
      }else{
        elegirComando(comandos,red);       
      }
    }
  }
  int status;
  for (size_t j = 0; j < ex; j++){
    waitpid(pid[j], &status, 0);
  }
  return 1;  
}

int agregarPath(char **comandos){
  if (paths != NULL){
    free(paths);
  }
  paths = (char **)malloc(sizeof(char *));
  char *nombre = NULL;
  int i = 0;
  char **path = comandos;
  while (*(++path)){
    nombre = (char *)malloc(strlen(*path) * sizeof(char));
    stpcpy(nombre, *path);
    paths[i] = nombre;
    i++;
    paths = (char **)realloc(paths, (i + 1) * sizeof(char *));
  }
  paths[i] = NULL;
  lenghtPaths = i;
  return 1;
}
int ejecutar(char **comandos){

  int status = 1;
  for (int i = 0; i < lenghtPaths; i++){
    int fullPathLen = strlen(paths[i]) + strlen(comandos[0]) + 1;
    char auxpath[fullPathLen];
    strcpy(auxpath, paths[i]);
    strcat(auxpath, "/");
    strcat(auxpath, comandos[0]);
    if (access(auxpath, 1) == 0){
      int rc = fork();
      if (rc == 0){
        if (execv(auxpath, comandos) == -1){
          exit(1);
        }
        else{
          exit(0);
        }
      }else{
        wait(&status);
        if (status == 0){
          break;
        }
      }
    }else{
      status = 1;
    }
  }
  if (status == 1){
    write(STDERR_FILENO, error, strlen(error));
  }
  return 1;
}

int contRed(char **comandos, int len){

  for (int i = 0; i < len; i++){
    if (strcmp(comandos[i], ">\0") == 0){
      return i;
    }
  }
  return 0;
}

int redireccionar(char **comandos, int index){
  char **args = copiarComando(0, index, comandos);
  if (comandos[index + 1] == NULL || comandos[index + 2] != NULL){
    write(STDERR_FILENO, error, strlen(error));
  }else{
    int fd = open(comandos[index + 1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    int std_out = dup(STDOUT_FILENO);
    int std_err = dup(STDERR_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    ejecutar(args);
    close(fd);
    dup2(std_out, STDOUT_FILENO);
    dup2(std_err, STDERR_FILENO);
  }
  return 1;
}
char **copiarComando(int start, int end, char **command){
  char **nuevo = (char **)malloc((end - start + 1) * sizeof(char *));  
  for (int i = 0; i < end; i++){
    nuevo[i] = command[i];
  }
  nuevo[end] = NULL;
  return nuevo;
}