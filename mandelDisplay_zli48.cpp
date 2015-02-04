//
//  mandelDisplay.cpp
//  
//
//  Created by Ze Li on 11/10/14.
//
//


#include "mandel.h"
#include <fstream>

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
struct test *shmstruct;
int counter;
//forward declaration
void handler(int);
void readparas(double*,double*,double*,double*,int*,int*);
void mandisplay(ofstream,int*,char*,int,int);
int main(int argc, char* argv[]) {
    counter = 0;
    char colors[15];// a secure way of declaring char array
    colors[0] = '.';
    colors[1] = '-';
    colors[2] = '~';
    colors[3] = ':';
    colors[4] = '+';
    colors[5] = '*';
    colors[6] = '%';
    colors[7] = 'O';
    colors[8] = '8';
    colors[9] = '&';
    colors[10] = '?';
    colors[11] = '$';
    colors[12] = '@';
    colors[13] = '#';
    colors[14] = 'X';
    int shmID = atoi(argv[1]);
    int msgqID1 = atoi(argv[2]);
    int msgqID2 = atoi(argv[3]);
    int msgsz1 = sizeof(msgstruct1) - sizeof(long);
    int msgsz2 = sizeof(msgstruct2) - sizeof(long);
    int params[7];
    double xMin,xMax,yMin,yMax;
    int nRows,nCols,m,r,c,n;
    struct shmid_ds buf;
    msgstruct1.type = 1;
    msgstruct1.done = true;
    msgstruct2.type = 1;
    int* data;
    data = (int*)shmat(shmID,NULL,0);
    while(true){
        signal(SIGUSR1,handler);
        readparas(&xMin,&xMax,&yMin,&yMax,&nRows,&nCols);
        read(0,&m,SZINT);
        msgrcv(msgqID2,&msgstruct2,N,0,IPC_NOWAIT);
        ofstream fout(msgstruct2.filename);
        
        usleep(nRows*nCols);
        cout << setw(nCols + 5) << yMin << endl;
        for(r = nRows-1; r>=0;r--){
            for(c = 0; c < nCols; c++){

                n = *(data + r*nCols + c );
                if(n<0){
                    fout << -1 << " ";
                    cout << " ";
                }
                else{
                    fout << n%15 << " ";
                    cout << colors[n%15];
                }
            }
            fout << endl;
            cout << endl;
        }
        cout << setw(nCols + 5) << yMax << endl;
        cout << xMin << setw(nCols - 5) << xMax << endl;

//        shmdt(data);
        counter++;
        msgstruct1.done = true;
        msgsnd(msgqID1,&msgstruct1,msgsz1,IPC_NOWAIT);
    }
    
    return 1;
}

void readparas(double *xMin, double *xMax, double *yMin, double *yMax, int *nRows, int *nCols){
    read(0,xMin,SZDB);
    read(0,xMax,SZDB);
    read(0,yMin,SZDB);
    read(0,yMax,SZDB);
    read(0,nRows,SZINT);
    read(0,nCols,SZINT);

}
void handler(int sig){
    if(sig==SIGUSR1){
        exit(counter);
    }
}
