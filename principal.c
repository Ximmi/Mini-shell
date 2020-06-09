#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

void checkHostName(int hostname);
void redireccionar(int pid);
void pasartuberia(int pid);
int ubicar(char c);
int leer();
char *instruccion[20];
char arg[20][100];
int main(){
    int elec;
/////////////////////////////////////////////////////////
    char hostbuffer[100];
    char  currentdir[100];
    int hostname, cwdir, pid; 
  
    //Para el nombre del equipo actual 
    hostname = gethostname(hostbuffer, sizeof(hostbuffer)); 
    checkHostName(hostname); 
    //Para el directorio actual
    getcwd(currentdir, sizeof(currentdir));
    printf(":H: %s /D: %s $", hostbuffer, currentdir);
////////////////////////////////////////////////////////
    if(pid==-1){
        perror("Error en el fork\n");
        exit(-1);
    }
    else{
        elec=leer();
        printf("elec= %d\n", elec);
        switch(elec){
            case 1:
                printf("Es un argumento\n");
                execvp(instruccion[0], instruccion);
                perror("---Error en la llamada a exec---\n");   
            break;
            case 2:
                printf("Es una tuberia\n");  
                pasartuberia(pid); 
            break;
            case 3:
                printf("Es un redireccionamiento\n");  
                redireccionar(pid);
            break;
            default:
                printf("No se identificó la cadena\n");
            break;
        }
    }
    /*else{
        wait(1);
        printf("\n // Padre a dormir// \n");
        sleep(1);
        
    }*/
//////////////////////////////////////////////////////// 

return 0;
}

/////////////////////////////////////////////////////////

void checkHostName(int hostname){ 
    if (hostname == -1){ 
        perror("gethostname"); 
        exit(1); 
    } 
}

/////////////////////////////////////////////////////////

int ubicar(char c){
    int flags = 1, i=0, j;    
    while(flags){
        if(instruccion[i][0]!=c){
            i++;
        }
        else{
            j=i;
            flags=0;
        }   
    }
    return j;
}
/////////////////////////////////////////////////////////
void redireccionar(int pid){
    int tuberia[2];
    int ubicacion= ubicar('>');
    char *redizq[50];
    char nomarchivo[50];
    int x;
    int fd;
    for(x=0; x<ubicacion; x++){
        redizq[x]=instruccion[x];
    }
    redizq[ubicacion]= 0;
    strcpy(nomarchivo, instruccion[ubicacion+1]);
    if(pipe(tuberia) == -1){
        perror("Error en pipe \n");
        exit(-1);
    }
    printf("%i %i \n", tuberia[0], tuberia[1]);   
        close(0);
        dup(tuberia[0]);
        close(1);
        open(nomarchivo, O_CREAT|O_RDWR, S_IRWXU);
        close(tuberia[0]);
        close(tuberia[1]);
        printf("Listo para redireccionar\n");
        execvp(redizq[0], redizq);
        perror("\error en el exec\n");

}
/////////////////////////////////////////////////////////

void pasartuberia(int pid){
    int tuberia[2];
    int ubicacion= ubicar('|');
    char *redizq[50];
    char *redder[50];
    int x, y=0;
    for(x=0; x<ubicacion; x++){
        redizq[x]=instruccion[x];
    }
    redizq[ubicacion]= 0;
    while(instruccion[ubicacion+1+y]!=NULL){
        redder[y]=instruccion[ubicacion+1+y];
        y++;
    }
    redder[y+1]= 0;
    
    if(pipe(tuberia) == -1){
        perror("Error en pipe \n");
        exit(-1);
    }
    printf("%i %i \n", tuberia[0], tuberia[1]);
    if(pid==0){
        close(1);
        dup(tuberia[1]);
        close(tuberia[0]);
        close(tuberia[1]);
        //execlp("sort", "sort", "-r", NULL);
        //perror("\nError en el execlp\n");
        execvp(redder[0], redder);
        perror("\error en el exec\n");
    }
    else{
        close(0);
        dup(tuberia[0]);
        close(tuberia[0]);
        close(tuberia[1]);
        execvp(redizq[0], redizq);
        perror("\error en el exec\n");
    }
}

/////////////////////////////////////////////////////////
  
int leer(){
    int i=0, j=0, caso=1;    
    char c;
    int flag=1;    
    while(flag){
        c=getchar();
        if(c!='\n'){
            if(c==' '){
                instruccion[i] = arg[i];
                //printf("arg[%d]= %s", i, arg[i]);
                i++;
                j=0;        
            }
            else if(c=='|'){
                caso=2;
                arg[i][j]=c;
                j++;
            }
            else if(c=='>'){
                caso=3;
                arg[i][j]=c;
                j++;
            }
            else{
                arg[i][j]=c;
                j++;
            }
        }
        else
            flag=0;
    }
    return caso;
}
