#include "co.h"
#include <stdlib.h>
#include<stdio.h>
#include<setjmp.h>
#include<stdbool.h>
#include<time.h>
typedef void (*func_t) (void* arg);
#define MAX_CO 100
typedef unsigned char uint8_t;
//uint8_t *__stack;
//void *__stack_backup;
#if defined(__i386__)
  #define SP "%%esp"
#elif defined(__x86_64__)
  #define SP "%%rsp"
#endif
#define STACK_SIZE 4096
uint8_t *__stack;
void* __stack_backup;
/*enum co_status{
  CO_NEW=1,
  CO_RUNNING,
  CO_WAITING,
  CO_DEAD,
};*/
struct co {
  char *name;
  int num;
  bool st;
  void *stack_backup;
  void (*func)(void *);
  void *arg;
  //enum co_status status;
  struct co* waiter;
  jmp_buf context;
  uint8_t stack[STACK_SIZE];
}__attribute__((aligned(16)));

struct co co_routines[MAX_CO];
struct co* current;
int count=0;//记录已使用的co的数量
void *cu_arg;
func_t cu_func;
void co_init(){
  count=0;
  co_routines[count].st=true;
  co_routines[count].num=count;
  srand((unsigned)time(NULL));
}

struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  ++count;
  co_routines[count].num = count;
  co_routines[count].st = true;
  cu_func = func;
  cu_arg = arg;
  int val = setjmp(co_routines[0].context);      //if we don't use it, will never return to this function.
  if(!val) {
    __stack = co_routines[count].stack + sizeof(co_routines[count].stack);
    asm volatile("mov " SP ", %0; mov %1, " SP :
                     "=g"(__stack_backup) :
                     "g"(__stack));  
    co_routines[count].stack_backup = __stack_backup;
    
    current = &co_routines[count];
    //printf("%d\n",cunt);
    /*char * temp=(char *)cu_arg;
    printf("%s\n", temp);*/
    cu_func(cu_arg);
     //printf("reach here\n");  //printf this sentence * cunt
    current->st = 0;        //current may change
    int temp = current->num;
    //printf("%d\n",temp);
    
    current = &co_routines[0];
    //longjmp(current->buf, 1);  
    __stack_backup = co_routines[temp].stack_backup;
    asm volatile("mov %0," SP : : "g"(__stack_backup)); 
	//printf("change esp\n");    
    longjmp(current->context, 1);  
    
  }
  /*else
    printf("return from co_yield\n");*/
  return &co_routines[count];
}


void co_wait(struct co *co) {
  setjmp(current->context);
  if(co->st){
    int next=rand()%count+1;
    while(next==current->num||!co_routines[count].st){
      next=rand()%count+1;
    }
    current=&co_routines[next];
    longjmp(current->context,1);
  }
}

void co_yield() {
  int val=setjmp(current->context);
  if(val==0){
    int next=rand()%(count+1);
    while(next==current->num||!co_routines[next].st){
      next=rand()%(count+1);
    }
    current=&co_routines[next];
    longjmp(current->context,1);
  }
  else{
    return;
  }
}
