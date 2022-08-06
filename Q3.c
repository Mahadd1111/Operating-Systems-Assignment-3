#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<stdio.h>
#include<pthread.h>
#include<math.h>
#include<time.h>
#include<inttypes.h>

#define BILLION 1000000000L;

int S=5000;
int numtasks=0;
int unitval=0;

struct task{
    int id;
    int task_value;
    int arrival_time;
    int unit_count;
    int* unitIdList;
    double completionTime;
    int* outputstages;
}tasks[10];



struct node{
    struct task data;
    struct node* next;
};

struct node* front;
struct node* rear;

void enQueue(struct task val){
    struct node* temp=(struct node*)malloc(sizeof(struct node));
    temp->data=val;
    temp->next=NULL;
    if(rear==NULL){
        front=rear=temp;
    }
    else{
        rear->next=temp;
        rear=temp;
    }
}

struct task dequeue(){
    struct node* temp;
    temp=front;
    if(front==NULL){
        printf("Queue is Empty\n");
        front=rear=NULL;
    }
    else{
        struct task returnval=front->data;
        front=front->next;
        free(temp);
        return returnval;
    }
}

void displayQueue(){
    struct node* temp=front;
    int count=0;
    if(front==NULL){
        printf("Nothing in Queue to display\n");
    }
    else{
        printf("Queue elements are: \n");
        while(temp!=NULL){
            printf("%d\n",temp->data.id);
            temp=temp->next;
            count++;
        }
    }
}


void createNewTask(int tnum,int argnum,char a[100]){
    int value=atoi(a);
    if(argnum==1){tasks[tnum].id=value;}
    else if(argnum==2){tasks[tnum].task_value=value;}
    else if(argnum==3){
        tasks[tnum].unit_count=value;
        tasks[tnum].outputstages=malloc((tasks[tnum].unit_count)*sizeof(int));
    }
    else if(argnum==4){
        tasks[tnum].unitIdList=malloc((tasks[tnum].unit_count)*sizeof(int));
        tasks[tnum].unitIdList[argnum-4]=value;
    }
    if(argnum>4){
        tasks[tnum].unitIdList[argnum-4]=value;
    }
}

void seperateFunction(char a[100],int tnum){
    int count=0;
    int j=0;
    int parameter=0;
    for(int i=0;a[i]!='\0',count<=strlen(a);i++){
        char temp[100]="";
        while(a[i]!=' '){
            temp[j]=a[i];
            j++;
            i++;
            count++;
        }
        while(a[i+1]==' '){
            i++;
            count++;
        }
        j=0;
        parameter++;
        //printf("Word is: %s\n",temp);
        createNewTask(tnum,parameter,temp);
    }
}

int receptionRoom(){
    int tnum=-1;
    FILE* file=fopen("input.txt","r");
    if(!file){printf("Unable to open file\n");return -1;}
    char line[100];
    fgets(line,sizeof(line),file);
    while(fgets(line,sizeof(line),file)){
        tnum++;
        seperateFunction(line,tnum);
    }
    fclose(file);
    return tnum;
}

void displayTasks(){
    for(int i=0;i<=numtasks;i++){
        printf("For Task %d : \n",i+1);
        printf("Id: %d\n",tasks[i].id);
        printf("task_value: %d\n",tasks[i].task_value);
        printf("Arrival: %d\n",tasks[i].arrival_time);
        printf("UnitsCount: %d\n",tasks[i].unit_count);
        for(int j=0;j<tasks[i].unit_count;j++){
            printf("Unit %d : %d \n",j+1,tasks[i].unitIdList[j]);
        }
        printf("\n");
    }
}

void putTasksInQueue(){
    for(int i=0;i<=numtasks;i++){
        enQueue(tasks[i]);
    }
}

void* executeUnit(void* arg){
    struct task* temp=(struct task*)arg;
    //printf("Curr Task: %d with unitval %d and taskval %d\n",temp->id,temp->unitIdList[unitval],temp->task_value);
    int unitfunction=temp->unitIdList[unitval];
    if(unitfunction==0){
        temp->task_value=temp->task_value+5;
        temp->task_value=temp->task_value%S;
    }
    else if(unitfunction==1){
        temp->task_value=temp->task_value*7;
        temp->task_value=temp->task_value%S;
    }
    else if(unitfunction==2){
        temp->task_value=pow(temp->task_value,5);
        temp->task_value=temp->task_value%S;
    }
    else if(unitfunction==3){
        temp->task_value=temp->task_value-100;
    }
    else if(unitfunction==4){
        temp->task_value=pow(temp->task_value,2);
    }
    //printf("New Task val is: %d\n",temp->task_value);
    temp->outputstages[unitval]=temp->task_value;
    //printf("Current task value is: %d\n",temp->outputstages[unitval]);
}

void dispatcher(){
    putTasksInQueue();
    for(int i=numtasks;i>=0;i--){
        struct timespec start,end;
        clock_gettime(CLOCK_REALTIME,&start);
        unitval=0;
        struct task nextTask=dequeue();
        struct task* currTask;
        for(int i=0;i<=numtasks;i++){ // searches dequeued id which matched id in tasks[]
            if(tasks[i].id==nextTask.id){
                currTask=&tasks[i];
            }
        }
        int numThreads=currTask->unit_count;
        pthread_t threads[numThreads];
        for(;unitval<numThreads;unitval++){
            pthread_create(&threads[unitval],NULL,executeUnit,currTask);
            pthread_join(threads[unitval],NULL);
        }
        clock_gettime(CLOCK_REALTIME,&end);
        double accum=/*((end.tv_sec-start.tv_sec)*1000)+*/((end.tv_nsec - start.tv_nsec)/1e6);
        tasks[i].completionTime=accum;
    }
}

void printroom(){
    printf("TaskId\tCompletionTime\tUnitIds\t\tTaskValue1\tTaskValue2\tTaskValue3\tTaskValue4\tTaskValue5\n");
    for(int i=0;i<=numtasks;i++){
        printf("%d\t",tasks[i].id);
        printf("%f\t",tasks[i].completionTime);
        for(int j=0;j<tasks[i].unit_count;j++){
            printf("%d,",tasks[i].unitIdList[j]);
        }
        if(tasks[i].unit_count>=4){
            printf("\t");
        }
        else{printf("\t\t");}
        for(int x=0;x<tasks[i].unit_count;x++){
            printf("%d_%d\t\t",tasks[i].unitIdList[x],tasks[i].outputstages[x]);
        }
        printf("\n");
    }
}

int main(){
    numtasks=receptionRoom();
    dispatcher();
    printroom();
}