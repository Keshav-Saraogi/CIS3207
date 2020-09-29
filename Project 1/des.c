#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Event Types:
#define CPU_ARRIVAL 0
#define CPU_FINISH 1
#define DISK_ARRIVAL 2
#define DISK_FINISH 3
#define SIMULATION_END 4

//States:
#define IDLE 0
#define BUSY 1


typedef struct event 
{
    int taskID;
    int eType;
    int time;           
} event;           //event type like CPU_ARRIVAL, SIMULATION_END, etc. Used to determine servicing when event occurs to store in event queue.

typedef struct node
{
    struct event task;
    struct node* next;
} node;

event evnt;

node* CPU_Q = NULL;
node* D1_Q = NULL;
node* D2_Q = NULL;
node* E_Q = NULL;

int SEED = 0;
int INIT_TIME = 0;      //Beginning of simulation
int FIN_TIME = 0;       //End of Simulation
int ARRIVE_MIN = 0;     //New process will enter sys at rand interval between min & max
int ARRIVE_MAX = 0;
float QUIT_PROB = 0.0;  //Prob process will exit sys after done executing on CPU
int CPU_MIN = 0;        //Time a process spends executing on CPU rand interval between min & max
int CPU_MAX = 0;
int DISK1_MIN = 0;      //Time a process spends doing I/O on Disk rand interval between min & max
int DISK1_MAX = 0;
int DISK2_MIN = 0;
int DISK2_MAX = 0;

int Time_CURRENT = 0;
char CPU_STAT = IDLE;
char D1_STAT = IDLE;
char D2_STAT = IDLE;

//NUM in QUEUES
int cpu_number_of_tasks = 0;
int cpu_number_of_tasks_max = 0;

int disk1_number_of_tasks = 0;
int disk1_number_of_tasks_max = 0;

int disk2_number_of_tasks = 0;
int disk2_number_of_tasks_max = 0;

int event_number_of_tasks = 0;
int event_number_of_tasks_max = 0;

int TOTAL_NoT = 1;

//TIME arrive-finish, idle-busy & MAX
int CPU_IDLE_TIME = 0;
int CPU_BUSY_TIME = 0;
float CPU_AVG_RESP_TIME = 0.0;
int CPU_MAX_TIME = 0;
int CPU_TOTAL_TIME = 0;

int D1_IDLE_TIME = 0;
int D1_BUSY_TIME = 0;
float D1_AVG_RESP_TIME = 0.0;
int D1_MAX_TIME = 0;
int D1_TOTAL_TIME = 0;

int D2_IDLE_TIME = 0;
int D2_BUSY_TIME = 0;
float D2_AVG_RESP_TIME = 0.0;
int D2_MAX_TIME = 0;
int D2_TOTAL_TIME = 0;


//PROTOTYPE FUNCTIONS-----------------------------------------------------

int randTime(int max, int min);
event startEvent(int tID, int type, int tme);
void printNodeContents(node a);
void printEventContents(event a);

float throughPut(int numJobsTotal);
int findMax(int newData, int oldMax);
float findAvg(int a, int b, float oldAvg);
int findTotal(int newData, int oldTotal);
float findUtil(int total);

//Priority / Event Queue fNs:
void push(event tsk, node** eQ, int* eTotal);
event pop(int* eTotal, node** eQ);

//FIFO Queue fNs:
void enQueue(event tsk, node** fQ, int* tTotal);
event deQueue(int* tTotal, node** fQ);

//Handler fNs:
void CPU_Handler(event);
void DISK_Handler(event);


int main() 
{
    printf("Output on log.txt\n");
    freopen("log.txt","w",stdout);
    FILE *fptr;
    char varStrValues[12];
    int varValues[12];
    int i = 0, j = 0;

    if ((fptr = fopen("config.txt","r")) == NULL)
    {
       printf("Error! opening file");
       exit(1);
    }
    while(fscanf(fptr, "%*s %s", varStrValues) == 1)
    {
        if(i==5)
        {
            sscanf(varStrValues, "%f", &QUIT_PROB);
        }
        sscanf(varStrValues, "%d", &varValues[i++]);
    }

    SEED = varValues[0];
    INIT_TIME = varValues[1];
    FIN_TIME = varValues[2];
    ARRIVE_MIN = varValues[3];
    ARRIVE_MAX = varValues[4];
    CPU_MIN = varValues[6];
    CPU_MAX = varValues[7];
    DISK1_MIN = varValues[8];
    DISK1_MAX = varValues[9];
    DISK2_MIN = varValues[10];
    DISK2_MAX = varValues[11];

    printf("INITIAL VARIABLES:");
    printf("\nSEED = %d", SEED);
    printf("\nINIT_TIME = %d", INIT_TIME);
    printf("\nFIN_TIME = %d", FIN_TIME);
    printf("\nARRIVE_MIN = %d", ARRIVE_MIN);
    printf("\nARRIVE_MAX = %d", ARRIVE_MAX);
    printf("\nQUIT_PROB = %2.1f", QUIT_PROB);
    printf("\nCPU_MIN = %d", CPU_MIN);
    printf("\nCPU_MAX = %d", CPU_MAX);
    printf("\nDISK1_MIN = %d", DISK1_MIN);
    printf("\nDISK1_MAX = %d", DISK1_MAX);
    printf("\nDISK2_MIN = %d", DISK2_MIN);
    printf("\nDISK2_MAX = %d\n\n", DISK2_MAX);

    fclose(fptr);
    
    Time_CURRENT = INIT_TIME;
    srand(SEED);

    push(startEvent(event_number_of_tasks, SIMULATION_END, FIN_TIME), &E_Q, &event_number_of_tasks);
    event_number_of_tasks_max++;
    push(startEvent(event_number_of_tasks, CPU_ARRIVAL, INIT_TIME), &E_Q, &event_number_of_tasks);
    event_number_of_tasks_max++;

    printf("\t   Welcome to DESimulator!\n");
    printf("\nEVENT ID: \tEVENT TYPE: \t    EVENT TIME: ");

    //Loop to execute EventQueue
    while (event_number_of_tasks != 0 && Time_CURRENT < FIN_TIME) {

        //pop event from top of EventQ to be serviced
        evnt = pop(&event_number_of_tasks, &E_Q);
        event_number_of_tasks_max--;

        //update current time
        Time_CURRENT = evnt.time;

        //service task according to its event type
        switch (evnt.eType){

            case CPU_ARRIVAL:
                CPU_Handler(evnt);
                break;

            case CPU_FINISH:
                CPU_Handler(evnt);
                break;

            case DISK_ARRIVAL:
                DISK_Handler(evnt);
                break;

            case DISK_FINISH:
                DISK_Handler(evnt);
                break;

            case SIMULATION_END:
                printf("\n    %d \t\tSIMULATION_END \t\t%d\n", evnt.taskID, Time_CURRENT);

                printf("\ntotalN: %d", TOTAL_NoT);
                printf("\nCPU_NoT: %d", cpu_number_of_tasks);
                printf("\nD1_NoT: %d", disk1_number_of_tasks_max);
                printf("\nD2_NoT: %d", disk2_number_of_tasks_max);
                printf("\nE_NoT: %d", event_number_of_tasks);

                printf("\nMaximum Size of CPU Queue: %d", cpu_number_of_tasks_max);
                float CPU_NoT_AVG = cpu_number_of_tasks / TOTAL_NoT;
                printf("\nAverage Size of CPU Queue: %2.1f", CPU_NoT_AVG);
                printf("\nMaximum Size of Disk1 Queue: %d", disk1_number_of_tasks_max);
                float D1_NoT_AVG = disk1_number_of_tasks_max / TOTAL_NoT;
                printf("\nAverage Size of Disk1 Queue: %2.1f", D1_NoT_AVG);
                printf("\nMaximum Size of Disk2 Queue: %d", disk2_number_of_tasks_max);
                float D2_NoT_AVG = disk2_number_of_tasks_max / TOTAL_NoT;
                printf("\nAverage Size of Disk2 Queue: %2.1f", D2_NoT_AVG);
                printf("\nMaximum Size of Event Queue: %d", event_number_of_tasks_max);
                float E_NoT_AVG = event_number_of_tasks / TOTAL_NoT;
                printf("\nAverage Size of Event Queue: %2.1f\n", E_NoT_AVG);

                int cpuUtil = findUtil(cpu_number_of_tasks);
                printf("\nUtilization of CPU: %2.1d", cpuUtil);
                int d1Util = findUtil(disk1_number_of_tasks_max);
                printf("\nUtilization of D1: %2.1d", d1Util);
                int d2Util = findUtil(disk2_number_of_tasks_max);
                printf("\nUtilization of D2: %2.1d\n", d2Util);

                printf("\nMaximum Response Time of CPU: %d", CPU_MAX_TIME);
                printf("\nAverage Response Time of CPU: %2.1f", CPU_AVG_RESP_TIME);
                printf("\nMaximum Response Time of D1: %d", D1_MAX_TIME);
                printf("\nAverage Response Time of D1: %2.1f", D1_AVG_RESP_TIME);
                printf("\nMaximum Response Time of D2: %d", D2_MAX_TIME);
                printf("\nAverage Response Time of D2: %2.1f\n", D2_AVG_RESP_TIME);

                float cpuPut = throughPut(cpu_number_of_tasks);
                printf("\nThroughput of CPU: %2.1f", cpuPut);
                float d1Put = throughPut(cpu_number_of_tasks);
                printf("\nThroughput of D1: %2.1f", d1Put);
                float d2Put = throughPut(cpu_number_of_tasks);
                printf("\nThroughput of D2: %2.1f\n", d2Put);

                printf("\n\t  Thank you for using DESimulator!\n");
                break;
        }
    }
    return 0;
}

int randTime(int max, int min)
{
    int rTime = ((rand() % (max - min + 1)) + min);
    return rTime;
}

//START EVENT: given data, return event----------------------------------
event startEvent(int tID, int type, int tme)
{
    event task;
    task.taskID = tID;
    task.eType = type;
    task.time = tme;
    return task;
}

//PRINT NODE CONTENTS: given node, print details of node-----------------
void printNodeContents(node a)
{
    printf("\n---------Contents of Node------------");
    printf("\nNode task taskID: \t%d", a.task.taskID);
    printf("\nNode task event type: \t%d", a.task.eType);
    printf("\nNode task event time: \t%d", a.task.time);
    printf("\nNode next: \t\t%d\n", a.next);
}

//PRINT EVENT CONTENTS: given event, print details of event--------------
void printEventContents(event a)
{
    printf("\n---------Contents of Event------------");
    printf("\nEvent task taskID: \t%d", a.taskID);
    printf("\nEvent task event type: \t%d", a.eType);
    printf("\nEvent task event time: \t%d\n", a.time);
}

float throughPut(int numJobsTotal)
{
    int totalTime = FIN_TIME-INIT_TIME;
    return (numJobsTotal / totalTime);
}

int findMax(int newData, int oldMax)
{
    if(newData > oldMax)
        return newData;

    return oldMax;
}

float findAvg(int a, int b, float oldAvg)
{
    float newAvg = (((b - a) + oldAvg) / 2);
    return newAvg;
}

int findTotal(int newData, int oldTotal)
{
    int newTotal = oldTotal + newData;
    return newTotal;
}

float findUtil(int total)
{
    float util = (total / (FIN_TIME - INIT_TIME));
    return util;
}


///PRIORITY QUEUE:_______________________________________________________

//PUSH: given event, push a node with that event onto Event Queue--------
void push(event tsk, node** eQ, int* eTotal)
{
    //variable to help organize priority
    node* oldNode;
    oldNode = *eQ;

    //build node to hold event
    node* newNode = (node*)malloc(sizeof(node));
    newNode->task = tsk;
    newNode->next = NULL;

    //increment events total
    (*eTotal)++;

    //only applicable on first run to start empty Q
    if(*eQ == NULL)
    {
        *eQ = newNode;
        return;
    }

    //check for lowest time to prioritize Q
    if(newNode->task.time < oldNode->task.time)
    {
        newNode->next = oldNode;
        *eQ = newNode;
        return;
    }

    //reorganize linked list based on priority
    while(oldNode->next != NULL && newNode->task.time >= oldNode->next->task.time)
    {
        oldNode = oldNode->next;
    }
    newNode->next = oldNode->next;
    oldNode->next = newNode;

    return;
}

//POP: given event Q, pop an event from top of eQ node-------------------
event pop(int* eTotal, node** eQ) 
{

    //pull top priority node data
    node *newNode = *eQ;

    //change second priority in eQ to top priority
    *eQ = (*eQ)->next;

    //pull event from node holding it
    event task = newNode->task;

    //deallocate the memory previously allocated to node
    free(newNode);

    //decrement event number of events
    //(*eTotal)--;

    return task;
}


///FIFO QUEUEs:__________________________________________________________

//ENQUEUE: given event, enqueue a node with that event onto FIFO queue---
void enQueue(event tsk, node** fQ, int* tTotal)
{

    //variable to help link list
    node* oldNode;
    oldNode = *fQ;

    //build node to hold event
    node* newNode = (node*)malloc(sizeof(node));
    newNode->task = tsk;
    newNode->next = NULL;
    //increment task total
    (*tTotal)++;

    //check to see if fifo Q is empty
    if(*fQ == NULL)
    {
        *fQ = newNode;
        return;
    }

    //if Q is not empty place node in line and connect list together
    while(oldNode->next != NULL)
    {
        oldNode = oldNode->next;
    }
    oldNode->next = newNode;
    return;
}

//DEQUEUE: given fifo Q, dequeue an event from front of fQ node----------
event deQueue(int* tTotal, node** fQ)
{
    //pull front of fQ node data
    node *newNode = *fQ;
    //change second in fQ to first
    *fQ = (*fQ)->next;
    //pull event from node holding it
    event task = newNode->task;
    //deallocate the memory previously allocated to node
    free(newNode);
    //decrement event number of events
    (*tTotal)++;
    return task;
}

//CPU HANDLER: given an event, handle it---------------------------------
void CPU_Handler(event tsk)
{
    //create rand num between 0-1
    float r, randNum;
    r = rand();
    randNum = r / RAND_MAX;
    //check if event type is CPU_FINISH
    if(tsk.eType == CPU_FINISH)
    {
        printf("\n  %d \t\tCPU_FINISH \t\t%d", tsk.taskID, Time_CURRENT);
        CPU_STAT = IDLE;
        printf("\n     \t\tCPU_IDLE \t\t%d", Time_CURRENT);
        //check if process will exit the system
        if(randNum < QUIT_PROB)
        {
            printf("\n  %d \t\tEXITS SYSTEM \t\t%d", tsk.taskID, Time_CURRENT);
        }
        else
        {

            //enter process back into eQ for disk I/O
            push(startEvent(tsk.taskID, DISK_ARRIVAL, randTime(ARRIVE_MAX, ARRIVE_MIN)+Time_CURRENT), &E_Q, &event_number_of_tasks);
            event_number_of_tasks_max++;
        }
    }

    //check is event type is CPU_ARRIVAL
    if(tsk.eType == CPU_ARRIVAL)
    {
        printf("\n  %d \t\tCPU_ARRIVAL \t\t%d" , tsk.taskID, Time_CURRENT);

        //enter process into fQ for CPU
        enQueue(tsk, &CPU_Q, &cpu_number_of_tasks);
        cpu_number_of_tasks_max++;
        cpu_number_of_tasks++;
        TOTAL_NoT++;
        push(startEvent(TOTAL_NoT, CPU_ARRIVAL, randTime(ARRIVE_MAX, ARRIVE_MIN)+Time_CURRENT), &E_Q, &event_number_of_tasks);
        event_number_of_tasks_max++;

        //check if CPU is available
        if (CPU_STAT == IDLE)
        {
            printf("\n  %d \t\tSERVICED ON CPU \t%d" , tsk.taskID, Time_CURRENT);
            //pull task from front of CPU fQ
            tsk = deQueue(&cpu_number_of_tasks, &CPU_Q);
            cpu_number_of_tasks_max--;
            //enter process back into eQ to finish on CPU
            push(startEvent(tsk.taskID, CPU_FINISH, randTime(CPU_MAX, CPU_MIN)+Time_CURRENT), &E_Q, &event_number_of_tasks);
            event_number_of_tasks_max++;
            CPU_STAT = BUSY;
            printf("\n     \t\tCPU_BUSY \t\t%d", Time_CURRENT);
        }
    }
    return;
}

//DISK HANDLER: given an event, handle it--------------------------------
void DISK_Handler(event tsk)
{
    //check if event type is DISK_FINISH
    if(tsk.eType == DISK_FINISH)
    {
        //check to see which Disk has a smaller Q, finish task at smaller Q
        if(disk1_number_of_tasks_max <= disk2_number_of_tasks_max)
        {
            printf("\n  %d \t\tDISK1_FINISH \t\t%d", tsk.taskID, Time_CURRENT);
            D1_STAT = IDLE;
            printf("\n     \t\tD1_IDLE \t\t%d", Time_CURRENT);
        }
        else
        {
            printf("\n  %d \t\tDISK2_FINISH \t\t%d", tsk.taskID, Time_CURRENT);
            D2_STAT = IDLE;
            printf("\n     \t\tD2_IDLE \t\t%d", Time_CURRENT);
        }
    }

    //check is event type is DISK_ARRIVAL
    if(tsk.eType == DISK_ARRIVAL)
    {
        //check to see which Disk has a smaller Q, service task at smaller Q
        if(disk1_number_of_tasks_max <= disk2_number_of_tasks_max)
        {
            //enter process into fQ for D1
            enQueue(tsk, &D1_Q, &disk1_number_of_tasks_max);
            disk1_number_of_tasks++;
            disk1_number_of_tasks_max++;
            printf("\n  %d \t\tDISK1_ARRIVAL \t\t%d", tsk.taskID, Time_CURRENT);
            //check is D1 is available
            if (D1_STAT == IDLE)
            {
                printf("\n  %d \t\tSERVICED ON D1 \t\t%d" , tsk.taskID, Time_CURRENT);
                //pull task from front of D1 fQ
                tsk = deQueue(&disk1_number_of_tasks_max, &D1_Q);
                disk1_number_of_tasks_max--;
                //enter process back into eQ to finish on D1
                push(startEvent(tsk.taskID, DISK_FINISH, randTime(DISK1_MAX, DISK1_MIN)+Time_CURRENT), &E_Q, &event_number_of_tasks);
                event_number_of_tasks_max++;
                D1_STAT = BUSY;
                printf("\n     \t\tD1_BUSY \t\t%d", Time_CURRENT);
            }
        }
        else
        {
            //enter process into fQ for D2
            enQueue(tsk, &D2_Q, &disk2_number_of_tasks_max);
            disk2_number_of_tasks++;
            disk2_number_of_tasks_max++;
            printf("\n  %d \t\tDISK2_ARRIVAL \t\t%d", tsk.taskID, Time_CURRENT);
            //check is D2 is available
            if (D2_STAT == IDLE)
            {
                printf("\n  %d \t\tSERVICED ON D2 \t\t%d" , tsk.taskID, Time_CURRENT);
                //pull task from front of D2 fQ
                tsk = deQueue(&disk2_number_of_tasks_max, &D2_Q);
                disk2_number_of_tasks_max--;
                //enter process back into eQ to finish on D2
                push(startEvent(tsk.taskID, DISK_FINISH, randTime(DISK2_MAX, DISK2_MIN)+Time_CURRENT), &E_Q, &event_number_of_tasks);
                event_number_of_tasks_max++;
                D2_STAT = BUSY;
                printf("\n     \t\tD2_BUSY \t\t%d", Time_CURRENT);
            }
        }
    }
    return;
}