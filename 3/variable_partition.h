#include <unistd.h>
#include <curses.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>

#define MAX_THREAD 3
#define BF_initialize_require_memory_list FF_initialize_require_memory_list
#define WF_initialize_require_memory_list FF_initialize_require_memory_list
#define BF_initialize_thread_residence_memory_list FF_initialize_thread_residence_memory_list
#define WF_initialize_thread_residence_memory_list FF_initialize_thread_residence_memory_list
#define WF_delete_freearea_list FF_delete_freearea_list
#define BF_delete_freearea_list FF_delete_freearea_list
#define WF_delete_require_memory_list FF_delete_require_memory_list
#define BF_delete_require_memory_list FF_delete_require_memory_list
#define WF_delete_thread_residence_memory_list FF_delete_thread_residence_memory_list
#define BF_delete_thread_residence_memory_list FF_delete_thread_residence_memory_list



typedef struct freearea{
	struct freearea *next;
	int start_address;
	int size;
}FREEAREA;

typedef struct require_memory{
    struct require_memory *next;
	char thread_name[10];
	int size;
	int duration;   
}REQUIRE_MEMORY;

typedef struct thread_residence_memory{
    struct thread_residence_memory *next;
	char thread_name[10];
	int start_address;
	int size;
}THREAD_RESIDENCE_MEMORY;

FREEAREA init_free_area_table[5]={
	{NULL,10,10},
	{NULL,40,30},
	{NULL,80,5},
	{NULL,145,15},
	{NULL,180,20}
};

REQUIRE_MEMORY init_thread_require_memory_table[3]={
	{NULL,"thread_1",20,4},
	{NULL,"thread_2",10,5},
	{NULL,"thread_3",5,6}
};


THREAD_RESIDENCE_MEMORY init_thread_residence_memory_table[5]={
	{NULL,"a",0,10},
	{NULL,"b",20,20},
	{NULL,"c",70,10},
	{NULL,"d",85,60},
	{NULL,"e",160,20}
};


FREEAREA *p_free_area_list=NULL;

REQUIRE_MEMORY *p_thread_require_memory_queue=NULL;

THREAD_RESIDENCE_MEMORY *p_thread_residence_memory_list=NULL;

THREAD_RESIDENCE_MEMORY *tail_thread_residence_memory_list=NULL;

pthread_mutex_t CS_THREAD_MEMORY_LIST;

pthread_mutex_t CS_SCREEN;

pthread_mutex_t CS_FREEAREA_LIST;

pthread_mutex_t WF_RELEASE;


pthread_t h_thread[MAX_THREAD];

sem_t thread_end[MAX_THREAD];


void display_thread_residence_memory_list();
void display_freearea_list(int x, int y);



void FF_delete_freearea_list();

REQUIRE_MEMORY *FF_initialize_require_memory_list(REQUIRE_MEMORY *init_table,int num);

void FF_delete_require_memory_list();

THREAD_RESIDENCE_MEMORY *FF_initialize_thread_residence_memory_list(THREAD_RESIDENCE_MEMORY *init_table,int num);

void  FF_delete_thread_residence_memory_list();

void  FF_release_memory(int start_address,int size);


void* WF_thread(void *data);
void  WF_insert_freearea(FREEAREA *free_node);
void  change_freearea_list(FREEAREA *p);
void  WF_initialize_freearea_list(FREEAREA *init_table,int num);
int   WF_require_memory(int size);
void  WF_release_memory(int start_address,int size);
void  WF();