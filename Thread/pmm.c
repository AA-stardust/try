#include <common.h>
//#include<pthread.h>
//#define DEBUG
/*intptr_t atomic_xchg(volatile intptr_t *addr,
                               intptr_t newval) {
  // swap(*addr, newval);
  intptr_t result;
  asm volatile ("lock xchg %0, %1":
    "+m"(*addr), "=a"(result) : "1"(newval) : "cc");
  return result;
}*/
intptr_t locked = 0;

static inline void lock() {
  while (1) {
    intptr_t value = _atomic_xchg(&locked, 1);
    if (value == 0) {
      break;
    }
  }
}

static inline void unlock() {
  _atomic_xchg(&locked, 0);
}
typedef struct header{
  intptr_t addr;
  size_t size;
  struct header* next;
}header_t;
header_t *info;

typedef struct node{
  uintptr_t start;
  size_t size;
  struct node* next;
}node_t;
node_t *head;

uintptr_t Align(uintptr_t start,size_t size){
  size_t temp=1;
  while(temp<size){
    temp=temp<<1;
  }
  if(start%temp==0)return start;
  else{
    size_t n=start/temp;
    uintptr_t result=temp*(n+1);
    return result;
  }
}

void insert(node_t *p,uintptr_t(start),size_t size){
  node_t *q=(node_t*)malloc(sizeof(node_t));
  q->start=start;
  q->size=size;
  q->next=p->next;
  p->next=q;
}

void delete(node_t *p){
  #ifdef DEBUG
  assert(p!=head);
  #endif
  node_t *q=head;
  while(q->next!=p)q=q->next;
  q->next=p->next;
  free(p);
}

node_t *search(size_t size){
  node_t *p=head;
  for(;p!=NULL;p=p->next){
    if(p->size>=size){
      uintptr_t temp=Align(p->start,size)-p->start;
      size_t actual_size=p->size-(size_t)temp;
      if(actual_size>=size)return p;
    }
  }
  return NULL;
}

uintptr_t split(node_t *p,size_t size){
    uintptr_t start=Align(p->start,size);
    if(start==p->start){
      p->start=p->start+size;
      p->size=p->size-size;
      return start;
    }
    size_t new_size=p->size-(start-p->start)-size;
    uintptr_t new_start=start+(uintptr_t)size;
    p->size=start-p->start;
    insert(p,new_start,new_size);
    return start;
}
void print(){
  node_t *p=head;
  for(;p!=NULL;p=p->next){
    printf("[%p,%p)->",p->start,p->start+p->size);
  }
  printf("\n");
}
void print2(){
  header_t *p=info;
  for(;p!=NULL;p=p->next){
    printf("[%p,%p)->",p->addr,p->addr+(uintptr_t)p->size);
  }
  printf("\n");
}
void info_insert(intptr_t addr,size_t size,header_t *p);
static void *my_klloc(size_t size){
  uintptr_t brk=0;
  node_t *memory=search(size);
  if(memory==NULL){
    #ifdef DEBUG
    printf("Not enough memory for 0x%x\n",size);
    #endif 
    return NULL;
  }
  brk=split(memory,size);
  info_insert(brk,size,info);
  #ifdef DEBUG
  printf("result of malloc: \n");
  print();
  printf("info_list: \n");
  print2();
  //printf("addr: %p\n",brk);
  #endif
  return (void*)brk;
}
void info_insert(intptr_t addr,size_t size,header_t *p){
  header_t *new=(header_t*)malloc(size);
  new->addr=addr;
  new->size=size;
  new->next=p->next;
  p->next=new;
}
void info_delete(header_t *p){
  #ifdef DEBUG
  assert(p!=info);
  #endif
  header_t *q=info;
  while(q->next!=p)q=q->next;
  q->next=p->next;
  free(p);
}
header_t* info_search(void *ptr){
  intptr_t temp=(intptr_t)ptr;
  header_t *q=info;
  //printf("temp: %p\n",temp);
  for(;q!=NULL;q=q->next){
    if(q->addr==temp){
      return q;
    }
  }
  #ifdef DEBUG
  assert(0);
  #endif // DEBUG
 
  return NULL;
}
void merge_or_insert(intptr_t start,size_t size){
  node_t* p=head;
  node_t* q;
  intptr_t end=start+(size_t)size;
  if(end<=head->start){
    if(end==head->start){
      head->start=start;
      head->size+=size;
      return;
    }
    else{
      node_t *new=(node_t*)malloc(size);
      new->size=size;
      new->start=start;
      new->next=head;
      head=new;
      return;
    }
  }
  
  for(;p->next!=NULL;p=p->next){
    q=p->next;
    if(start>=(p->start+p->size)&&start<=q->start){
      #ifdef DEBUG
      assert(start+size<=q->start);//将free的空间不能与已有的空间重叠
      #endif
      if((start==p->start+(intptr_t)p->size)&&end==q->start){
        p->size=p->size+size+q->size;
        delete(q);
        return;
      }
      else if(start==p->start+(intptr_t)p->size){
        p->size+=size;
        return;
      }
      else if(end==q->start){
        q->start=start;
        q->size+=size;
        return;
      }
      else{
        insert(p,start,size);
        return;
      }
    }
  }
  if(start>=(p->start+p->size)){
    if(start==(p->start+p->size)){
      p->size+=size;
      return;
    }
    else{
      insert(p,start,size);
      return;
    }
  }
  printf("should not reach here\n");
  #ifdef DEBUG
  assert(0);
  #endif
}
static void my_kfree(void *ptr){
  header_t *free_node=info_search(ptr);
  merge_or_insert(free_node->addr,free_node->size);//维护node_t类型的链表
  info_delete(free_node);
  #ifdef DEBUG
  printf("result of free: \n");
  print();
  printf("info_list: \n");
  print2();
  #endif
}

static void *kalloc(size_t size) {
  lock();
  void* brk=my_klloc(size);
  unlock();
  return (void*)brk;
}
static void kfree(void *ptr) {
  lock();
  my_kfree(ptr);
  unlock();
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)_heap.end - (uintptr_t)_heap.start);
  //printf("here2\n");
  //printf("header: %d\n",sizeof(header_t));
  //printf("int : %d, size_t: %d\n",sizeof(int),sizeof(size_t));
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, _heap.start, _heap.end);

  head=(node_t*)malloc(sizeof(node_t));
  head->start=(uintptr_t)_heap.start;
  head->size=(size_t)pmsize;
  head->next=NULL;

  info=(header_t*)malloc(sizeof(header_t));
  info->size=0;
  info->next=NULL;
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
