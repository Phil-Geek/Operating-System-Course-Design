#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>
#define MAX_PHILOSOPHERS 5
#define ZERO 48  
#define DELAY (rand()%25)/1000

pthread_mutex_t h_mutex_chopsticks[MAX_PHILOSOPHERS];

int thread_number[MAX_PHILOSOPHERS]={0,1,2,3,4};

void* ordered_allocation_philosopher(void* data){
	int philosopher_number=*(int *)(data);
	int i=0;
	for(;;)
	{
		srand( (unsigned)time( NULL ) * ( philosopher_number+ 1) );
                sleep(DELAY);
		if(i>=5){
			i=0;
			clear();
			refresh();
		}else{
			i++;
	        if(philosopher_number==MAX_PHILOSOPHERS-1){
                printw("%s%c%s%c\n","Philosopher ",ZERO+philosopher_number," is waiting chopstick ",ZERO);
                refresh();
                pthread_mutex_lock(&h_mutex_chopsticks[ZERO]);
                sleep(DELAY/4);
                printw("%s%c%s%c\n","Philosopher ",ZERO+philosopher_number," is waiting chopstick ",ZERO+(philosopher_number));
                refresh();
                
                pthread_mutex_lock(&h_mutex_chopsticks[philosopher_number]);
            }else{
                
                printw("%s%c%s%c\n","Philosopher ",ZERO+philosopher_number," is waiting chopstick ",ZERO+philosopher_number);
                refresh();
                
                pthread_mutex_lock(&h_mutex_chopsticks[philosopher_number]);
                sleep(DELAY/4);
                
                printw("%s%c%s%c\n","Philosopher ",ZERO+philosopher_number," is waiting chopstick ",ZERO+(1+philosopher_number));
                refresh();
                
                pthread_mutex_lock(&h_mutex_chopsticks[1+philosopher_number]);
            }
            printw("%s%c%s\n","Philosopher ",ZERO+philosopher_number," is eating.");
            refresh();
            sleep(DELAY);
            if(philosopher_number==MAX_PHILOSOPHERS-1){
                printw("%s%c%s%c\n","Philosopher ",ZERO+philosopher_number," is releasing chopstick ",ZERO);
                refresh();
                
                pthread_mutex_unlock(&h_mutex_chopsticks[ZERO]);
                
                printw("%s%c%s%c\n","Philosopher ",ZERO+philosopher_number," is releasing chopstick ",ZERO+philosopher_number);
                refresh();
                
                pthread_mutex_unlock(&h_mutex_chopsticks[philosopher_number]);
                sleep(DELAY);
            }else{
                printw("%s%c%s\n","Philosopher ",ZERO+philosopher_number," is eating.");
                refresh();
                sleep(DELAY);
                
                printw("%s%c%s%c\n","Philosopher ",ZERO+philosopher_number," is releasing chopstick ",ZERO+philosopher_number);
                refresh();
                
                pthread_mutex_unlock(&h_mutex_chopsticks[philosopher_number]);
                
                printw("%s%c%s%c\n","Philosopher ",ZERO+philosopher_number," is releasing chopstick ",ZERO+(1+philosopher_number)%MAX_PHILOSOPHERS);
                refresh();
                
                pthread_mutex_unlock(&h_mutex_chopsticks[(1+philosopher_number)%MAX_PHILOSOPHERS]);
                sleep(DELAY);
            }
        }
    }
	return 0;
    
}

void* ordered_allocation(){
	int i=0;
	pthread_t h_thread[MAX_PHILOSOPHERS];

	printw("orderded allocation:deadlock impossible.\n");
	refresh();
	for(i=0;i<MAX_PHILOSOPHERS;i++){
		pthread_mutex_init(&h_mutex_chopsticks[i],NULL);
	};
	for(i=0;i<MAX_PHILOSOPHERS;i++){
		pthread_create(&h_thread[i],NULL,ordered_allocation_philosopher,&thread_number[i]);
	};

	for(i=0;i<MAX_PHILOSOPHERS;i++){
		pthread_join(h_thread[i],NULL);
	}
}

int main(int argc,char *argv[]){
    initscr();
	ordered_allocation();
	endwin();
	return 0;
}
