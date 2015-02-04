//
//  mandelbrot_zli48.cpp
//  
//
//  Created by Ze Li on 11/10/14.
//  zli48, 676755673
//



#include "mandel.h"
pid_t pid1,pid2;
struct my_struct1{
    long int type;
    bool done;
};

struct my_struct2{
    long int type;
    char filename[N];
};
struct my_struct1 msgstruct1;
struct my_struct2 msgstruct2;

//forward declarations
void mandelCalc(char[N],char[N]);
void mandelDisplay(char[N],char[N],char[N]);
void dup_helper(int,int);
void close_helper(int);
void handler(int);
void sigint_handler(int);
void sigtstp_handler(int);
void sigchld_handler(int);
void help_wait(int, int);
void print_info();
void get_info(double*,double*,double*,double*,int*,int*,int*,int);
int msgqID1,msgqID2,shmID;//msg qs
struct shmid_ds buf;

//using namespace std;
int main(int argc, char** argv){
    //to catch ^C and ^Z
    signal(SIGINT,sigint_handler);
    signal(SIGTSTP,sigtstp_handler);
    print_info();
    int msgsz1,msgsz2;
    int pipe1[2],pipe2[2];
    int status1,status2;
    char strqID1[N],strqID2[N],strmID[N];//string form of
    msgsz1 = sizeof(msgstruct1) - sizeof(long);
    msgsz2 = sizeof(msgstruct2) - sizeof(long);
    msgstruct1.type = 1;
    msgstruct2.type = 1;
    //create pipes
    if(pipe(pipe1) == -1){
        perror("pipe error on pipe1");
        exit(EXIT_FAILURE);
    }
    if(pipe(pipe2) == -1){
        perror("pipe error on pipe2");
        exit(EXIT_FAILURE);
    }
//    create msg queues
    msgqID1 = msgget(IPC_PRIVATE, 0600|IPC_CREAT);//for q1
    msgqID2 = msgget(IPC_PRIVATE, 0600|IPC_CREAT);//for q2
//  error checking on msgget
    if(msgqID1 == -1){
        perror("msgget failed on msgqID1");
        exit(EXIT_FAILURE);
    }if(msgqID2 == -1){
        perror("msgget failed on msgqID2");
        msgctl(msgqID1,IPC_RMID,NULL);
        exit(EXIT_FAILURE);
    }
    //create shared memory
    shmID = shmget(IPC_PRIVATE,M,0600|IPC_CREAT);
    if(shmID == -1){
        perror("shmget failed on shmID");
        msgctl(msgqID1,IPC_RMID,NULL);
        msgctl(msgqID2,IPC_RMID,NULL);
        exit(EXIT_FAILURE);
    }
    //convert integer to char array
    cout << "msgqID1: ";
    sprintf(strqID1,"%d",msgqID1);
    puts(strqID1);
    cout << "msgqID2: ";
    sprintf(strqID2,"%d",msgqID2);
    puts(strqID2);
    cout << "shmID: ";
    sprintf(strmID,"%d",shmID);
    puts(strmID);
    if((pid1 = fork())== -1){
        perror("fork error on pid1");
        close_helper(EXIT_FAILURE);
    }
    //child 1
    if(pid1 == 0){
        close(pipe1[1]);//close unused write end of child1
        close(pipe2[0]);//close unused read end of child1
        int paras[7];
                //dup2 on child1
        dup_helper(pipe1[0],0);
        dup_helper(pipe2[1],1);
        mandelCalc(strmID,strqID1);//execute mandelCalc
        close(pipe1[0]);
        close(pipe2[1]);
    }
    else{//error
        if((pid2=fork())==-1){
            perror("fork error on pid1");
            close_helper(EXIT_FAILURE);
        }
        if(pid2==0){//child 2
            close(pipe2[1]);
            dup_helper(pipe2[0],0);
            mandelDisplay(strmID,strqID1,strqID2);
            close(pipe2[0]);
        }
        else{
            close(pipe1[0]);//close unused pipe ends
            double xMin,xMax,yMin,yMax;
            int nRows,nCols,maxIters;
            char choice;
            int isDone = 0;
            int done;
            while(true){
                done = 0;
                if(!isDone){
                    cout << "Do you want to start? Y/N" << endl;
                }
                else{
                    cout << "Do you wish to continue? Y/N" << endl;
                }
                isDone++;
                cin >> choice;
                switch(choice){
                    case 'y':
                    case 'Y':
                        cout << "Please enter a filename > ";
                        cin >> msgstruct2.filename;
                        msgsnd(msgqID2,&msgstruct2,msgsz2,IPC_NOWAIT);
                        get_info(&xMin,&xMax,&yMin, &yMax,&nRows, &nCols, &maxIters, pipe1[1]);
                        msgrcv(msgqID1,&msgstruct1,N,0,IPC_NOWAIT);
                        if(msgstruct1.done)
                            done++;
                        usleep(nRows*nCols*3);
                        msgrcv(msgqID1,&msgstruct1,N,0,IPC_NOWAIT);
                        if(msgstruct1.done)
                            done++;
                        if(done==2)
                            cout << "Done for this cycle." << endl;
                            break;
                    case 'N':
                    case 'n':
        
                        help_wait(status1,status2);//deal with child
                    default:
                        cout << "Please enter Y/N."<< endl;
                        
                }
            }
        }
    }
}
//execute mandelCalc
void mandelCalc(char str1[N],char str2[N]){
    char arg[13];
    arg[0] = '.';
    arg[1] = '/';
    arg[2] = 'm';
    arg[3] = 'a';
    arg[4] = 'n';
    arg[5] = 'd';
    arg[6] = 'e';
    arg[7] = 'l';
    arg[8] = 'C';
    arg[9] = 'a';
    arg[10] = 'l';
    arg[11] = 'c';
    arg[12] = '\0';
    char * const args1[10] = {arg,str1,str2,NULL};
    execvp(args1[0],args1);
}

//execute mandelDisplay
void mandelDisplay(char str1[N],char str2[N],char str3[N]){
    char arg[16];
    arg[0] = '.';
    arg[1] = '/';
    arg[2] = 'm';
    arg[3] = 'a';
    arg[4] = 'n';
    arg[5] = 'd';
    arg[6] = 'e';
    arg[7] = 'l';
    arg[8] = 'D';
    arg[9] = 'i';
    arg[10] = 's';
    arg[11] = 'p';
    arg[12] = 'l';
    arg[13] = 'a';
    arg[14] = 'y';
    arg[15] = '\0';
    char * const args2[10] = {arg,str1,str2,str3,NULL};
    execvp(args2[0],args2);
}

//report errors on pipes
void dup_helper(int pipe,int in_out){
    if(dup2(pipe,in_out)==-1){
        perror("dup2 error");
        close_helper(EXIT_FAILURE);
    }
    close(pipe);
}

void close_helper(int info){
    msgctl(msgqID1,IPC_RMID,NULL);
    msgctl(msgqID2,IPC_RMID,NULL);
    shmctl(shmID,IPC_RMID,&buf);
    exit(info);
}
//controls when control c is pressed
void sigint_handler(int sig){
    int status1,status2;
    cout << " is pressed!" << endl;
    help_wait(status1,status2);
}

void sigtstp_handler(int sig){
    int status1,status2;
    cout << " is pressed!" << endl;
    close_helper(0);
}

void help_wait(int status1, int status2){
    kill(pid1,SIGUSR1);//send SIGUSR1
    kill(pid2,SIGUSR1);//send SIGUSR2
    waitpid(pid1,&status1,0);
    if(WIFEXITED(status1)){
        printf("Child1, pid: %d images calculated: %d\n",pid1, WEXITSTATUS(status1));
    }
    if(WIFSIGNALED(status1)){
        printf("Child1, pid: %d terminated on signal - %d\n",pid1, WTERMSIG(status1));
    }
    waitpid(pid2,&status2,0);
    if(WIFEXITED(status2)){
        printf("Child2, pid: %d images calculated: %d\n",pid2, WEXITSTATUS(status2));
    }
    if(WIFSIGNALED(status2)){
        printf("Child2, pid: %d terminated on signal - %d\n",pid2, WTERMSIG(status2));
    }
    close_helper(EXIT_SUCCESS);
}

void print_info(){
    cout << "This is a C++ version of hw4, mandelbrot program, CS361" << endl << endl;
    cout << "This is done by Ze Li, zli48,676755673" << endl << endl;
    cout << "Automatic type checking will be performed on xMin, xMax, yMin, yMax, nRows, nCols and maxIters" << endl;
    cout << "If these parameters are not in range, the program will ask you to retype the parameter" << endl;
}
void sigchld_handler(int sig){

}

void get_info(double *xMin, double *xMax, double *yMin, double *yMax, int *nRows, int *nCols, int *maxIters,int pipe1){
    do{
        cout << "Please enter a valid xMin [-2.0,2.0] > ";
        cin >> *xMin;
    }while(*xMin > 2.0 || *xMin < -2.0 );
    
    write(pipe1,xMin,SZDB);
    do{
        cout << "Please enter a valid xMax [-2.0,2.0] > ";
        cin >> *xMax;
    }while(*xMax > 2.0 || *xMax < -2.0 || *xMax < *xMin);
    write(pipe1,xMax,SZDB);
    do{
        cout << "Please enter a valid yMin [-1.5,1.5] > ";
        cin >> *yMin;
    }while(*yMin > 1.5 || *yMin < -1.5);
    write(pipe1,yMin,SZDB);
    do{
        cout << "Please enter a valid yMax [-1.5,1.5] > ";
        cin >> *yMax;
    }while(*yMax > 1.5 || *yMax < -1.5 || *yMax < *yMin);
    write(pipe1,yMax,SZDB);
    do{
        cout << "Please enter a positive integer nRows > ";
        cin >> *nRows;
    }while(*nRows <= 0);
    write(pipe1,nRows,SZINT);
    do{
        cout << "Please enter a positive integer nCols > ";
        cin >> *nCols;
    }while(*nCols <= 0);
    write(pipe1,nCols,SZINT);
    do{
        cout << "Please enter a positive integer maxIters > ";
        cin >> *maxIters;
    }while(*maxIters <=0);
    write(pipe1,maxIters,SZINT);
    usleep((*nRows)*(*nCols));
}
