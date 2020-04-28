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
    qsort(in_ , num_of_process , sizeof(I) , cmp);
    for(int i = 0 ; i < num_of_process ; i++){
        strcpy(process_name[i] , in_[i].name);
        rc_process[i][0] = in_[i].ready_time;
        rc_process[i][1] = in_[i].exec_time;
    }
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(1 , &mask);
    sched_setaffinity(0, sizeof(cpu_set_t), &mask);
    
    if(strcmp(policy , "FIFO") == 0) FIFO();
    if(strcmp(policy , "RR") == 0) RR();
    if(strcmp(policy , "SJF") == 0) SJF();
    if(strcmp(policy , "PSJF") == 0) PSJF();
}


void RR(){
    while(1){
        //fprintf(stdout, "time : %d\n", time_of_main);
        if(next_index_process < num_of_process){
            for(int i = next_index_process ; i < num_of_process ; i++){
                if(rc_process[i][0] == time_of_main){
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
                        cpu_set_t mask;
                        CPU_ZERO(&mask);
                        CPU_SET(0 , &mask);
                        sched_setaffinity(pid_child[i], sizeof(cpu_set_t), &mask);
                        idle_it(pid_child[i]);
                        if(head == NULL) start_head(i);
                        else {
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
            if(head -> rest_time == 0) {
                run_it(head->pid);
                int ret = wait(NULL);
                head = head -> next;
            }
            else {
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
        //if(head != NULL)fprintf(stdout , "%d now!\n" , head -> pid);
        if(head == NULL && next_index_process >= num_of_process) break;
    }
    //result();
}


void FIFO(){

    while(1){
        /*struct sched_param param;
        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
        int set = sched_setscheduler(0 , SCHED_OTHER , &param);*/

        for(int i = next_index_process ; i < num_of_process ; i++){
            if(rc_process[i][0] == time_of_main){
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
                    cpu_set_t mask;
                    CPU_ZERO(&mask);
                    CPU_SET(0 , &mask);
                    sched_setaffinity(pid_child[i], sizeof(cpu_set_t), &mask);
                    if(head == NULL) {
                        start_head(i);
                        run_it(head->pid);
                    }
                    else {
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
        if (head != NULL && head -> rest_time == 0){
            int ret = wait(NULL);
            head = head -> next;
            if(head) run_it(head->pid);
        }

        timeunit();
        time_of_main ++;
        if (head) head -> rest_time --;
        if (next_index_process >= num_of_process && !head) break;
    }
    //result();
}


void SJF(){
    while(1){
        //fprintf(stdout, "time : %d\n", time_of_main);
        if(next_index_process < num_of_process){
            for(int i = next_index_process ; i < num_of_process ; i++){
                if(rc_process[i][0] == time_of_main){
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
                        cpu_set_t mask;
                        CPU_ZERO(&mask);
                        CPU_SET(0 , &mask);
                        sched_setaffinity(pid_child[i], sizeof(cpu_set_t), &mask);
                        idle_it(pid_child[i]);
                        if(head == NULL) start_head(i);
                        else insert_new_job(i);
                        next_index_process ++ ;
                    }
                }
                else break;
            }
        }
        
        if(head != NULL && head -> rest_time == 0){
            run_it(head->pid);
            int ret = wait(NULL);
            head = head -> next;
        }
        if(head) run_it(head->pid);
        timeunit();
        time_of_main ++;
        if(head != NULL)head -> rest_time --;
        //if(head != NULL)fprintf(stdout , "%d now!\n" , head -> pid);
        if(head == NULL && next_index_process >= num_of_process) break;
    }
    //result();
}

void PSJF(){
    while(1){
        //fprintf(stdout, "time : %d\n", time_of_main);
        if(next_index_process < num_of_process){
            for(int i = next_index_process ; i < num_of_process ; i++){
                if(rc_process[i][0] == time_of_main){
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
                        cpu_set_t mask;
                        CPU_ZERO(&mask);
                        CPU_SET(0 , &mask);
                        sched_setaffinity(pid_child[i], sizeof(cpu_set_t), &mask);
                        idle_it(pid_child[i]);
                        if(head == NULL) start_head(i);
                        else if(rc_process[i][1] >= head -> rest_time) insert_new_job(i);
                        else {
                            idle_it(head -> pid);
                            P *tmp = head;
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
        
        if(head != NULL && head -> rest_time == 0){
            run_it(head->pid);
            int ret = wait(NULL);
            head = head -> next;
        }
        if(head) run_it(head->pid);
        timeunit();
        time_of_main ++;
        if(head != NULL)head -> rest_time --;
        //if(head != NULL)fprintf(stdout , "%d now!\n" , head -> pid);
        if(head == NULL && next_index_process >= num_of_process) break;
    }
    //result();
}


