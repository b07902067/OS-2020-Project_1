#define _GNU_SOURCE
#include<sys/syscall.h>
#include<sys/time.h>
#include<stdlib.h>
#include<string.h>
#include"sche.h"
void RR();
void FIFO();
void SJF();
void PSJF();
typedef struct {
    char name[33];
    int ready_time, exec_time;
} I;

I in_[105];

int cmp(const void *a, const void *b){
    int fron = ((I*)a) -> ready_time;
    int back = ((I*)b) -> ready_time;
    return fron > back;
}

int main(){
    scanf("%s" , policy);
    scanf("%d" , &num_of_process);
    for(int i = 0 ; i < num_of_process ; i++) scanf("%s%d%d" , in_[i].name , &in_[i].ready_time , &in_[i].exec_time);
    // sort process by ready time
    qsort(in_ , num_of_process , sizeof(I) , cmp);
    for(int i = 0 ; i < num_of_process ; i++){
        strcpy(process_name[i] , in_[i].name);
        rc_process[i][0] = in_[i].ready_time;
        rc_process[i][1] = in_[i].exec_time;
    }
    
    // the main process will run on CPU with id 1
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(1 , &mask);
    sched_setaffinity(0, sizeof(cpu_set_t), &mask);
    
    // start scheduler
    if(strcmp(policy , "FIFO") == 0) FIFO();
    if(strcmp(policy , "RR") == 0) RR();
    if(strcmp(policy , "SJF") == 0) SJF();
    if(strcmp(policy , "PSJF") == 0) PSJF();
}


void RR(){
    while(1){
        if(next_index_process < num_of_process){
            for(int i = next_index_process ; i < num_of_process ; i++){
                if(rc_process[i][0] == time_of_main){ // there is a process ready to run
                    struct timespec start, end;
                    syscall(333 , &start.tv_sec , &start.tv_nsec);
                    if((pid_child[i] = fork()) == 0){
                        for(int j = 0 ; j < rc_process[i][1] ; j++) timeunit();
                        syscall(333 , &end.tv_sec , &end.tv_nsec);
                        pid_t this = getpid();
                        syscall(334 , start.tv_sec , start.tv_nsec , end.tv_sec , end.tv_nsec , this);
                        fprintf(stdout , "%s %d\n" , process_name[i] , getpid());
                        exit(0);
                    }
                    else {
                        // all child process will run on CPU with id 0
                        cpu_set_t mask;
                        CPU_ZERO(&mask);
                        CPU_SET(0 , &mask);
                        sched_setaffinity(pid_child[i], sizeof(cpu_set_t), &mask);
                        idle_it(pid_child[i]);
                        if(head == NULL) start_head(i); // if no one running or waiting, then run this process
                        else { // append to tail
                            tail -> next = malloc(sizeof(P));
                            (tail -> next) -> ready_time = rc_process[i][0];
                            (tail -> next) -> rest_time = rc_process[i][1];
                            (tail -> next) -> next = NULL;
                            (tail -> next) -> previous = tail;
                            (tail -> next) -> pid = pid_child[i];
                            (tail -> next) -> next_stop = (rc_process[i][1] > 500)?(rc_process[i][1]-500):0;
                            tail = tail -> next;
                        }
                        next_index_process ++ ;
                    }
                }
                else break;
            }
        }
        if(head != NULL && head -> rest_time == head -> next_stop){
            if(head -> rest_time == 0) { // a child process is end
                run_it(head->pid);
                int ret = wait(NULL);
                head = head -> next;
            }
            else { // a child process has met a time quantum
                idle_it(head -> pid);
                head -> next_stop = (head->rest_time > 500)?(head->rest_time-500):0;
                if(head -> next != NULL){
                    tail -> next = head;
                    head -> previous = tail;
                    tail = tail -> next;
                    head = head -> next;
                    tail -> next = NULL;
                    head -> previous = NULL;
                    idle_it(tail->pid);
                }
            }
        }
        if(head) run_it(head->pid);
        timeunit();
        time_of_main ++;
        if(head != NULL)head -> rest_time --;
        if(head == NULL && next_index_process >= num_of_process) break;
    }
}


void FIFO(){
    while(1){
        for(int i = next_index_process ; i < num_of_process ; i++){
            if(rc_process[i][0] == time_of_main){ // there is a process ready to run
                struct timespec start, end;
                syscall(333 , &start.tv_sec , &start.tv_nsec);
                if((pid_child[i] = fork()) == 0){
                    for(int j = 0 ; j < rc_process[i][1] ; j++) timeunit();
                    syscall(333 , &end.tv_sec , &end.tv_nsec);
                    pid_t this = getpid();
                    syscall(334 , start.tv_sec , start.tv_nsec , end.tv_sec , end.tv_nsec , this);
                    fprintf(stdout , "%s %d\n" , process_name[i] , getpid());
                    exit(0);
                }
                else {
                    // all child process will run on CPU with id 0
                    cpu_set_t mask;
                    CPU_ZERO(&mask);
                    CPU_SET(0 , &mask);
                    sched_setaffinity(pid_child[i], sizeof(cpu_set_t), &mask);
                    if(head == NULL) {
                        start_head(i);
                        run_it(head->pid); // if no one running or waiting, then run new forked process
                    }
                    else { // else append to tail
                        idle_it(pid_child[i]);
                        tail -> next = malloc(sizeof(P));
                        (tail -> next) -> previous = tail;
                        tail = tail -> next;
                        tail -> next = NULL;
                        tail -> pid = pid_child[i];
                        tail -> rest_time = rc_process[i][1];
                    }
                    next_index_process ++;
                }
            }
            else break;
        }
        if (head != NULL && head -> rest_time == 0){ // a child process end
            int ret = wait(NULL);
            head = head -> next;
            if(head) run_it(head->pid);
        }

        timeunit();
        time_of_main ++;
        if (head) head -> rest_time --;
        if (next_index_process >= num_of_process && !head) break;
    }
}


void SJF(){
    while(1){
        if(next_index_process < num_of_process){
            for(int i = next_index_process ; i < num_of_process ; i++){
                if(rc_process[i][0] == time_of_main){ // there is a process ready to run
                    struct timespec start, end;
                    syscall(333 , &start.tv_sec , &start.tv_nsec);
                    if((pid_child[i] = fork()) == 0){
                        for(int j = 0 ; j < rc_process[i][1] ; j++) timeunit();
                        syscall(333 , &end.tv_sec , &end.tv_nsec);
                        pid_t this = getpid();
                        syscall(334 , start.tv_sec , start.tv_nsec , end.tv_sec , end.tv_nsec , this);
                        fprintf(stdout , "%s %d\n" , process_name[i] , getpid());
                        exit(0);
                    }
                    else {
                        // all child process will run on CPU with id 0
                        cpu_set_t mask;
                        CPU_ZERO(&mask);
                        CPU_SET(0 , &mask);
                        sched_setaffinity(pid_child[i], sizeof(cpu_set_t), &mask);
                        idle_it(pid_child[i]);
                        if(head == NULL) start_head(i); // if no one running or waiting, then run new forked process
                        else insert_new_job(i); // insert the new forked process in the link list, but it won't be the head except it's ready time is the same as head's
                        next_index_process ++ ;
                    }
                }
                else break;
            }
        }
        
        if(head != NULL && head -> rest_time == 0){ // a child process end
            run_it(head->pid);
            int ret = wait(NULL);
            head = head -> next;
        }
        if(head) run_it(head->pid);
        timeunit();
        time_of_main ++;
        if(head != NULL)head -> rest_time --;
        if(head == NULL && next_index_process >= num_of_process) break;
    }
}

void PSJF(){
    while(1){
        if(next_index_process < num_of_process){
            for(int i = next_index_process ; i < num_of_process ; i++){
                if(rc_process[i][0] == time_of_main){ // there is a process ready to run
                    struct timespec start, end;
                    syscall(333 , &start.tv_sec , &start.tv_nsec);
                    if((pid_child[i] = fork()) == 0){
                        for(int j = 0 ; j < rc_process[i][1] ; j++) timeunit();
                        syscall(333 , &end.tv_sec , &end.tv_nsec);
                        pid_t this = getpid();
                        syscall(334 , start.tv_sec , start.tv_nsec , end.tv_sec , end.tv_nsec , this);
                        fprintf(stdout , "%s %d\n" , process_name[i] , getpid());
                        exit(0);
                    }
                    else {
                        // all child process will run on CPU with id 0
                        cpu_set_t mask;
                        CPU_ZERO(&mask);
                        CPU_SET(0 , &mask);
                        sched_setaffinity(pid_child[i], sizeof(cpu_set_t), &mask);
                        idle_it(pid_child[i]);
                        if(head == NULL) start_head(i); // if no one running or waiting, then run the new forked process
                        else if(rc_process[i][1] >= head -> rest_time) insert_new_job(i); // insert the new forked process into link list, it can be head if it's rest time is less than head
                        else { // the new forked process will be head
                            idle_it(head -> pid);
                            P *tmp = head;
                            // the following lines (241 ~ 260) move head to the correct place
                            for(P *index = head ; index != NULL ; index = index -> next){
                                if (index == head) continue;
                                if(!(index -> next) && (head -> rest_time >= index -> rest_time)){
                                    index -> next = head;
                                    head -> previous = index;
                                    head = head -> next;
                                    tmp -> next = NULL;
                                    break;
                                }
                                if(head -> rest_time < index -> rest_time){
                                    if(index == head -> next) break;
                                    index -> previous -> next = head;
                                    head -> previous = index -> previous;
                                    index -> previous = head;
                                    head = head -> next;
                                    tmp -> next = index;
                                }
                                else continue;
                            }
                            // set the new forked process to be head
                            P *new = malloc(sizeof(P));
                            new -> pid = pid_child[i];
                            new -> ready_time = rc_process[i][0];
                            new -> rest_time = rc_process[i][1];
                            new -> next = head;
                            new -> previous = NULL;
                            head -> previous = new;
                            head = new;
                        }
                        next_index_process ++ ;
                    }
                }
                else break;
            }
        }
        
        if(head != NULL && head -> rest_time == 0){ //a child process end
            run_it(head->pid);
            int ret = wait(NULL);
            head = head -> next;
        }
        if(head) run_it(head->pid);
        timeunit();
        time_of_main ++;
        if(head != NULL)head -> rest_time --;
        if(head == NULL && next_index_process >= num_of_process) break;
    }
}


