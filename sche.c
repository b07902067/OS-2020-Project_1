#define _GNU_SOURCE
#include<sys/syscall.h>
#include<sys/time.h>
#include"sche.h"
void RR();
void FIFO();
void SJF();
void PSJF();

int main(){
    scanf("%s" , policy);
    scanf("%d" , &num_of_process);
    for(int i = 0 ; i < num_of_process ; i++) scanf("%s%d%d" , process_name[i] , &rc_process[i][0] , &rc_process[i][1]);
    
    if(strcmp(policy , "FIFO") == 0) FIFO();
    if(strcmp(policy , "RR") == 0) RR();
    if(strcmp(policy , "SJF") == 0) SJF();
    if(strcmp(policy , "PSJF") == 0) PSJF();
}


void RR(){
    while(1){
        //fprintf(stderr, "time : %d\n", time_of_main);
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
                        fprintf(stderr , "%s %d\n" , process_name[i] , getpid());
                        exit(0);
                    }
                    else {
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
        if(head) idle_it(head -> pid);
        time_of_main ++;
        if(head != NULL)head -> rest_time --;
        //if(head != NULL)fprintf(stdout , "%d now!\n" , head -> pid);
        if(head == NULL && next_index_process >= num_of_process) break;
    }
    //result();
}


void FIFO(){
    while(1){
        for(int i = next_index_process ; i < num_of_process ; i++){
            if(rc_process[i][0] == time_of_main){
                struct timespec start, end;
                syscall(333 , &start.tv_sec , &start.tv_nsec);
                if((pid_child[i] = fork()) == 0){
                    for(int j = 0 ; j < rc_process[i][1] ; j++) timeunit();
                    syscall(333 , &end.tv_sec , &end.tv_nsec);
                    pid_t this = getpid();
                    syscall(334 , start.tv_sec , start.tv_nsec , end.tv_sec , end.tv_nsec , this);
                    fprintf(stderr , "%s %d\n" , process_name[i] , getpid());
                    exit(0);
                }
                else {
                    struct sched_param param;
                    param.sched_priority = ;
                    sched_setscheduler(pid , SCHED_FIFO , &param);
                    if(head == NULL) start_head(i);
                    else {
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
        //fprintf(stderr, "time : %d\n", time_of_main);
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
                        fprintf(stderr , "%s %d\n" , process_name[i] , getpid());
                        exit(0);
                    }
                    else {
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
        if(head) idle_it(head -> pid);
        time_of_main ++;
        if(head != NULL)head -> rest_time --;
        //if(head != NULL)fprintf(stdout , "%d now!\n" , head -> pid);
        if(head == NULL && next_index_process >= num_of_process) break;
    }
    //result();
}

void PSJF(){
    while(1){
        //fprintf(stderr, "time : %d\n", time_of_main);
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
                        fprintf(stderr , "%s %d\n" , process_name[i] , getpid());
                        exit(0);
                    }
                    else {
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
        if(head) idle_it(head -> pid);
        time_of_main ++;
        if(head != NULL)head -> rest_time --;
        //if(head != NULL)fprintf(stdout , "%d now!\n" , head -> pid);
        if(head == NULL && next_index_process >= num_of_process) break;
    }
    //result();
}

