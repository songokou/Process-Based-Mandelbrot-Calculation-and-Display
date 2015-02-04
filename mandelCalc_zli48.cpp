//
//  mandelCalc.cpp
//  
//
//  Created by Ze Li on 11/10/14.
//
//
//


#include "mandel.h"


struct my_struct1{
    long int type;
    bool done;
//    int ti;
};

struct my_struct1 msgstruct1;
struct test *shmstruct;
int counter = 0;
//forward declaration
void mandelcalc(int*,double,double,double,double,int,int,int);
void handler(int);
void rwdb(double*);
void rwint(int*);
void rwparas(double*,double*,double*,double*,int*,int*,int*);
int main(int argc, char* argv[]) {
    int shmID = atoi(argv[1]);
    int paras[7];
    double xMin, xMax,yMin,yMax,deltaX,deltaY;
    int nRows,nCols,maxIters;
    int msgqID1 = atoi(argv[2]);
    int msgsz = sizeof(msgstruct1) - sizeof(long);
    int status,r,c,n;
    struct shmid_ds buf;
    msgstruct1.type = 1;
    while(true){
        signal(SIGUSR1,handler);//signal handler for SIGUSR1
        //read and write
        rwparas(&xMin,&xMax,&yMin,&yMax,&nRows,&nCols,&maxIters);
        int *data;
        data = (int *)shmat(shmID,NULL,0);
        mandelcalc(data,xMin,xMax,yMin,yMax,nRows,nCols,maxIters);
        shmdt(data);
        msgstruct1.done = true;
        msgsnd(msgqID1,&msgstruct1,msgsz,IPC_NOWAIT);
        counter++;
    }
    return 1;
         
}

void mandelcalc(int *data,double xMin, double xMax, double yMin, double yMax, int nRows, int nCols, int maxIters){
    int r,c,n;
    double deltaX = (xMax-xMin)/(nCols - 1);
    double deltaY = (yMax-yMin)/(nRows - 1);
    for(r = 0; r < nRows;r++){
        double cy = yMin + r*deltaY;
        for(c = 0; c < nCols; c++){
            double cx = xMin + c*deltaX;
            double zx = 0, zy = 0;
            for (n = 0; n < maxIters; n++){
                if(zx*zx + zy*zy >=4)
                    break;
                double zx_next = zx*zx - zy*zy + cx;
                double zy_next = 2*zx*zy+cy;
                zx = zx_next;
                zy = zy_next;
            }
            if(n >= maxIters)
                *(data + r*nCols + c) = -1;
            else
                *(data+r*nCols + c) = n;
        }
    }

}

void rwparas(double *xMin,double *xMax,double *yMin,double *yMax,int *nRows,int *nCols, int *maxIters){
    rwdb(xMin);
    rwdb(xMax);
    rwdb(yMin);
    rwdb(yMax);
    rwint(nRows);
    rwint(nCols);
    rwint(maxIters);
}
void rwdb(double* info){
    read(0,info,SZDB);
    write(1,info,SZDB);
}
void rwint(int* info){
    read(0,info,SZINT);
    write(1,info,SZINT);
}
void handler(int sig){
    if(sig==SIGUSR1){
        exit(counter);
    }
}
