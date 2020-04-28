#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sched.h>
#include<stdlib.h>
typedef struct process{
    pid_t pid;
    int ready_time, rest_time;
    int next_stop;
    struct process *next , *previous;
} P;
P *head = NULL, *tail = NULL;

char policy[5];
int num_of_process;
char process_name[105][33];
pid_t pid_child[105];
int rc_process[105][2];
int time_of_main = 0;
int index_process = 0 , next_index_process = 0;


void timeunit(){
    volatile unsigned long i; for(i=0;i<1000000UL;i++);
}

int idle_it(pid_t pid){
    struct sched_param param;
    param.sched_priority = 0;
    return sched_setscheduler(pid , SCHED_IDLE , &param);
}

int run_it(pid_t pid){
    struct sched_param param;
    param.sched_priority = 0;
    return sched_setscheduler(pid , SCHED_OTHER , &param);
}

int start_head(int index){
    head = malloc(sizeof(P));
    head -> ready_time = rc_process[index][0];
    head -> rest_time = rc_process[index][1];
    head -> next = head -> previous = NULL;
    head -> pid = pid_child[index];
    head -> next_stop = (rc_process[index][1] > 500)?(rc_process[index][1]-500):0;
    tail = head;
}

int insert_new_job(int index){
    P *tmp , *add;
    for(tmp = head ; tmp != NULL ; tmp = tmp->next){
        if(!(tmp->next) && ((tmp == head && rc_process[index][0] != head->ready_time) || rc_process[index][1] >= tmp->rest_time)){
            add = malloc(sizeof(P));
            tmp -> next = add;
            add -> next = NULL;
            add -> previous = tmp;
            break;
        }
        if(tmp == head && rc_process[index][0] != head->ready_time) continue;
        if(rc_process[index][1] < tmp->rest_time){
            add = malloc(sizeof(P));
            if(!(tmp->previous)){
                head = add;
                struct sched_param param;
                param.sched_priority = 0;
                sched_setscheduler(tmp->pid , SCHED_IDLE , &param);
            }
            else (tmp -> previous) -> next = add;
            add -> previous = tmp -> previous;
            add -> next = tmp;
            tmp -> previous = add;
            break;
        }
        if(rc_process[index][1] >= tmp->rest_time) continue;
    }
    add -> pid = pid_child[index];
    add -> rest_time = rc_process[index][1];
    add -> ready_time = rc_process[index][0];
}

void result(){
    for(int i = 0 ; i < num_of_process ; i++){
        printf("%d %d\n" , i , pid_child[i]);
        fflush(stdout);
    }
}
