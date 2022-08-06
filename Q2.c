#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>
#include<math.h>

// PLEASE ADD -lm FLAG when compiling eg gcc -pthread Q2.c -lm

float w[20][20];
float x[20][20];
float b[20];
float y[20];
float z[20];
float a[20];
float y2[20];
float z2[20];
float result[20];
int rw,cw,rx,cx,ry,cy,rb,cb,rz,cz,ra,ca;

void displayMatrices(){
    printf("--------------------------\n");
    printf("W matrix: \n");
    for(int i=0;i<rw;i++){
        for(int j=0;j<cw;j++){
            printf("%f\t",w[i][j]);
        }
        printf("\n");
    }
    printf("X matrix: \n");
    for(int i=0;i<rx;i++){
        printf("%f\n",x[i][0]);
    }
    printf("Y matrix: \n");
    for(int i=0;i<ry;i++){
        printf("%f\n",y[i]);
    }
    printf("Z matrix: \n");
    for(int i=0;i<rz;i++){
        printf("%f\n",z[i]);
    }
    printf("--------------------------\n");
}

void displayW(){
    printf("--------------------------\n");
    printf("W matrix: \n");
    for(int i=0;i<rw;i++){
        for(int j=0;j<cw;j++){
            printf("%f\t",w[i][j]);
        }
        printf("\n");
    }
    printf("--------------------------\n");
}

void displayX(){
    printf("--------------------------\n");
    printf("X matrix: \n");
    for(int i=0;i<rx;i++){
        printf("%f\n",x[i][0]);
    }
    printf("--------------------------\n");
}

void displayY(){
    printf("--------------------------\n");
    printf("Y matrix: \n");
    for(int i=0;i<ry;i++){
        printf("%f\n",y[i]);
    }
    printf("--------------------------\n");
}

void displayZ(){
    printf("--------------------------\n");
    printf("Z matrix: \n");
    for(int i=0;i<rz;i++){
        printf("%f\n",z[i]);
    }
    printf("--------------------------\n");
}

void displayA(){
    printf("--------------------------\n");
    printf("A matrix: \n");
    for(int i=0;i<rz;i++){
        printf("%f\n",a[i]);
    }
    printf("--------------------------\n");
}


void setMatrices(){
    printf("Setting Matrix W: \n");
    for(int i=0;i<rw;i++){
        for(int j=0;j<cw;j++){
            float val=0;
            printf("Enter Value of W for row %d col %d: ",i,j);
            scanf("%f",&val);
            w[i][j]=val;
        }
    }
    printf("Setting Matrix X: \n");
    for(int i=0;i<rx;i++){
        float val2=0;
        printf("Enter Value of X for row %d col %d: ",i,0);
        scanf("%f",&val2);
        x[i][0]=val2;
    }
}

void* multiplyMatrices(void* args){
    int* iptr=(int*)args;
    int i=*iptr;
    for(int k=0;k<rx;k++){
        y[i]+=w[i][k]*x[k][0];
    }
    pthread_exit(NULL);
}

void setBiasMatrix(){
    printf("Setting Bias Matrix: \n");
    for(int i=0;i<rb;i++){
        float val=0;
        printf("Enter Value of B for row %d col %d: ",i,0);
        scanf("%f",&val);
        b[i]=val;
    }
}

void* calculateZ(void* args){
    int* iptr=(int*)args;
    int i=*iptr;
    z[i]=y2[i]+b[i];
}

void* calculateA(void* args){
    int* iptr=(int*)args;
    int i=*iptr;
    a[i]=(1/(1+(exp(-1*(double)z2[i]))));
}

int main(){
    printf("Enter no of rows of W : ");
    scanf("%d",&rw);
    printf("Enter no of cols of W : ");
    scanf("%d",&cw);
    rx=cw;cx=1;
    rb=rw;cb=1;
    ry=rw;cy=1;
    rz=rw;cz=1;
    ra=rw;ca=1;
    setMatrices();
    displayW();
    displayX();
    int fd1[2];
    pipe(fd1); // pipe to connect P3 and P2
    int fd3[2];
    pipe(fd3);
    pid_t p3=fork();
    if(p3==0){
        //child of p3
        int fd2[2];
        pipe(fd2); // pipe to connect P2 and P1
        pid_t p2=fork();
        if(p2==0){
            //p1 code to execute
            pthread_t p1threads[rw];
            for(int i=0;i<rw;i++){
                pthread_create(&p1threads[i],NULL,multiplyMatrices,&i);
                pthread_join(p1threads[i],NULL);
            }
            displayY();
            close(fd2[0]);
            write(fd2[1],y,sizeof(int)*20);
            exit(0);
        }
        else{
            //p2 code to execute
            wait(NULL);
            close(fd2[1]);
            read(fd2[0],y2,sizeof(int)*20);
            setBiasMatrix();
            pthread_t p2threads[rb];
            for(int i=0;i<rb;i++){
                pthread_create(&p2threads[i],NULL,calculateZ,&i);
                pthread_join(p2threads[i],NULL);
            }
            displayZ();
            close(fd1[0]);
            write(fd1[1],z,sizeof(int)*20);
            exit(0);
        }
    }
    else{
        //p3 code to execute
        wait(NULL);
        close(fd1[1]);
        read(fd1[0],z2,sizeof(int)*20);
        pthread_t p3threads[rz];
        for(int i=0;i<rb;i++){
            pthread_create(&p3threads[i],NULL,calculateA,&i);
            pthread_join(p3threads[i],NULL);
        }
    }
    displayA();
}