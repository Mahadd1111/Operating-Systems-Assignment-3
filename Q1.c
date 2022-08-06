#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

void menu(){
    printf("--------Displaying menu----------\n");
    printf("1- Eggs: 15rs\n2- Bread: 60rs\n3- Chocolate: 50rs\n");
    printf("---------------------------------\n");
}

void* getItems(int array[4]){
    int p_eggs=0,p_bread=0,p_choc=0;
    menu();
    printf("Randomly generating number of items......\n\n");
    sleep(2);
    srand(100);
    int numeggs=(rand()%5)+1;
    srand(150);
    int numbread=(rand()%5)+1;
    srand(200);
    int numchoc=(rand()%5)+1;
    array[0]=numeggs+numbread+numchoc;
    p_eggs=numeggs*15;
    p_bread=numbread*60;
    p_choc=numchoc*50;
    array[1]=p_eggs;
    array[2]=p_bread;
    array[3]=p_choc;
    int sum=p_eggs+p_bread+p_choc;
    printf("Thread 1 : Items Purchased = eggs, bread, chocolate\n");
    printf("eggs=%dx%d=%d,bread=%dx%d=%d,chocolate=%dx%d=%d, sum=%d+%d+%d=%d\n",numeggs,15,p_eggs,numbread,60,p_bread,numchoc,50,p_choc,p_eggs,p_choc,p_bread,sum);
    pthread_exit((void*)sum);
}

void* calculateTax(int total){
    float a=total/1.8;
    float* tax=&a;
    float b=total+*tax;
    float* taxSum=&b;
    printf("Thread 2: %d/(1+0.8)=%.02f ,taxed sum=%.02f+%d=%.02f\n",total,*tax,*tax,total,*taxSum);
    pthread_exit((void*)taxSum);
}

void* calculateSale(int total){
    if(total>250){
        float a=total*0.1;
        float* sale=&a;
        float b=total-*sale;
        float* saleTotal=&b;
        printf("\nThread 3: price=%d,sale=0.1,%d*0.1=%.02f,%d-%.02f=%.02f\n",total,total,*sale,total,*sale,*saleTotal);
        pthread_exit((void*)saleTotal);
    }
    else{
        printf("\nThread 3: No sale on it as total is less than 250\n");
    }
}

void* sortedDisplay(int a[4]){
    printf("\nThread 4: \n");
    if(a[1]>=a[2]){
        if(a[1]>=a[3]){
            if(a[2]>=a[3]){
                printf("Eggs %d\nBread %d\nChocolates %d\n",a[1],a[2],a[3]);
            }
            else{
                printf("Eggs %d\nChocolates %d\nBread %d\n",a[1],a[3],a[2]);
            }
        }
        else{
            printf("Chocolates %d\nEggs %d\nBread %d\n",a[3],a[1],a[2]);
        }
    }
    else if(a[2]>=a[1]){
        if(a[2]>=a[3]){
            if(a[1]>=a[3]){
                printf("Bread %d\nEggs %d\nChocolate %d\n",a[2],a[1],a[3]);
            }
            else{
                printf("Bread %d\nChocolates %d\nEggs %d\n",a[2],a[3],a[1]);
            }
        }
        else{
            printf("Chocolates %d\nBread %d\n Eggs %d\n",a[3],a[2],a[1]);
        }
    }
    pthread_exit(NULL);
}

int main(){
    int array[4]={0,0,0,0};
    pthread_t thread1,thread2,thread3,thread4;
    void* total1;
    void* total2;
    void* total3;
    void* total4;
    pthread_create(&thread1,NULL,(void*)getItems,array);
    pthread_join(thread1,&total1);
    printf("\nValue of total is %d\n",(int)total1);
    pthread_create(&thread2,NULL,calculateTax,total1);
    pthread_join(thread2,&total2);
    pthread_create(&thread3,NULL,calculateSale,total1);
    pthread_join(thread3,&total3);
    pthread_create(&thread4,NULL,sortedDisplay,array);
    pthread_join(thread4,&total4);
}