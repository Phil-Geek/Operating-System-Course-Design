#include "variable_partition.h"
int l_x = 30;
int l_y = 0;
int l_y2 = 0;
int main(int argc,char *argv[]){
	initscr();
	WF();
	endwin();
	return 0;
}

void display_thread_residence_memory_list(){
	THREAD_RESIDENCE_MEMORY *p;
   	p=p_thread_residence_memory_list;
	int i=13;
	move(10,0);
	printw("|-------------------|--------------------|------------------|\n");
	printw("| thread_name       | start_address(kB)  | size(KB)         |\n");
	printw("|-------------------|--------------------|------------------|\n");
	while(p!=NULL){
	   move(i,0);
       printw("| %s",p->thread_name);
	   move(i,20);
	   printw("| %d",p->start_address);
	   move(i,41);
	   printw("| %d",p->size);
       move(i,60);
       printw("|\n");
	   p=p->next;
	   i++;
	};
	move(i,0);
        printw("|-------------------|--------------------|------------------|\n\n");
	refresh();
}

void display_freearea_list(int x, int y){
	FREEAREA *p;
   	p=p_free_area_list;
    move(x-3,y);
	printw("|-------------------|--------------------|\n");
    move(x-2,y);
	printw("| start_address(kB) | size(KB)           |\n");
    move(x-1,y);
	printw("|-------------------|--------------------|\n");
    while(p!=NULL){
	   move(x,y);
       printw("| %d",p->start_address);
	   move(x,y+20);
	   printw("| %d",p->size);
       move(x,y+41);
       printw("|\n");
	   p=p->next;
	   x++;
	};
	move(x,0);
    printw("|-------------------|--------------------|\n\n");
	refresh();
}

void FF_delete_freearea_list(){
	FREEAREA *temp;
	temp=p_free_area_list;
	while(temp!=NULL){
		temp=p_free_area_list->next;
		free(p_free_area_list);
		p_free_area_list=temp;
	}
	p_free_area_list=NULL;
}


REQUIRE_MEMORY * FF_initialize_require_memory_list(REQUIRE_MEMORY *init_table,int num)
{
  REQUIRE_MEMORY *temp;
  REQUIRE_MEMORY *head=NULL;
  REQUIRE_MEMORY *tail=NULL;
  int i;
  for(i=0;i<num;i++){
     temp=(REQUIRE_MEMORY *)malloc(sizeof(REQUIRE_MEMORY));
     strcpy(temp->thread_name,init_table[i].thread_name);
	 temp->size=init_table[i].size;
	 temp->duration=init_table[i].duration;
	 temp->next=NULL;
	 if(head==NULL)
		 head=tail=temp;
	 else{
		 tail->next=temp;
		 tail=tail->next;
	 }
  };
  return head;
}

void FF_delete_require_memory_list(){
	REQUIRE_MEMORY *temp;
	temp=p_thread_require_memory_queue;
	while(temp!=NULL){
		temp=p_thread_require_memory_queue->next;
		free(p_thread_require_memory_queue);
		p_thread_require_memory_queue=temp;
	}
	p_thread_require_memory_queue=NULL;
}

THREAD_RESIDENCE_MEMORY * FF_initialize_thread_residence_memory_list(THREAD_RESIDENCE_MEMORY *init_table,int num)
{
  THREAD_RESIDENCE_MEMORY *temp;
  THREAD_RESIDENCE_MEMORY *head=NULL;
  THREAD_RESIDENCE_MEMORY *tail=NULL;
  int i;
  for(i=0;i<num;i++){
     temp=(THREAD_RESIDENCE_MEMORY *)malloc(sizeof(THREAD_RESIDENCE_MEMORY));
     strcpy(temp->thread_name,init_table[i].thread_name);
	 temp->start_address=init_table[i].start_address;
	 temp->size=init_table[i].size;
	 temp->next=NULL;
	 if(head==NULL)
		 head=tail=temp;
	 else{
		 tail->next=temp;
		 tail=tail->next;
	 }
  };
  tail_thread_residence_memory_list=tail;
  return head;
}


void FF_delete_thread_residence_memory_list(){
	THREAD_RESIDENCE_MEMORY *temp=p_thread_residence_memory_list;
	
	temp=p_thread_residence_memory_list;
	while(temp!=NULL){
		temp=p_thread_residence_memory_list->next;
		free(p_thread_residence_memory_list);
		p_thread_residence_memory_list=temp;
	}
	p_thread_residence_memory_list=NULL;
}

void* WF_thread(void *data){
    int start_address=-1;
	int i=(((REQUIRE_MEMORY *)(data))->thread_name)[7]-49;
	THREAD_RESIDENCE_MEMORY *temp;
	pthread_mutex_lock(&CS_SCREEN);
	//printw("create thread:%s\n",((REQUIRE_MEMORY *)(data))->thread_name);
	refresh();
	pthread_mutex_unlock(&CS_SCREEN);
    
    while(1){
		start_address=WF_require_memory(((REQUIRE_MEMORY *)(data))->size);
        
        pthread_mutex_lock(&CS_SCREEN);
        refresh();
        display_freearea_list(17,l_y);
        l_y+=45;
        pthread_mutex_unlock(&CS_SCREEN);
        
		if(start_address>=0)
			break;
		else
			sleep(1);
	}
    
    pthread_mutex_lock(&CS_SCREEN);
    move(23,0);
    printw("After Matching:");
    pthread_mutex_unlock(&CS_SCREEN);
	temp=(THREAD_RESIDENCE_MEMORY *)malloc(sizeof(THREAD_RESIDENCE_MEMORY));
    strcpy(temp->thread_name,((REQUIRE_MEMORY *)(data))->thread_name);
	temp->start_address=start_address;
	temp->size=((REQUIRE_MEMORY *)(data))->size;
	temp->next=NULL;
	pthread_mutex_lock(&CS_THREAD_MEMORY_LIST);
	tail_thread_residence_memory_list->next=temp;
	tail_thread_residence_memory_list=tail_thread_residence_memory_list->next;
    pthread_mutex_unlock(&CS_THREAD_MEMORY_LIST);
    
	sleep(((REQUIRE_MEMORY *)(data))->duration);
    pthread_mutex_lock(&WF_RELEASE);
	WF_release_memory(start_address,((REQUIRE_MEMORY *)(data))->size);
    pthread_mutex_unlock(&WF_RELEASE);
    
	sem_post(&thread_end[i]);
	return 0;
}

void WF_insert_freearea(FREEAREA *free_node){
    FREEAREA *p;
    FREEAREA *p_next;
	 
	 if(p_free_area_list==NULL)
		 p_free_area_list=free_node;
	 else{
        p=p_next=p_free_area_list;
	    while(p_next!=NULL&&free_node->size<p_next->size){
            p=p_next;
            p_next=p_next->next;
	    }
	    if(p_next==NULL)      
            p->next=free_node;
        else if(p==p_next){  
		    free_node->next=p;
		    p_free_area_list=free_node;
		}
		else{           
			 free_node->next=p_next;
			 p->next=free_node;
		}
	 }
}

void WF_initialize_freearea_list(FREEAREA *init_table,int num){
  FREEAREA *temp;
  int i;
  for(i=0;i<num;i++){
     temp=(FREEAREA *)malloc(sizeof(FREEAREA));
     temp->start_address=init_table[i].start_address;
	 temp->size=init_table[i].size;
	 temp->next=NULL;
	 WF_insert_freearea(temp);
  }
}


int WF_require_memory(int size){
	int start_address=-1;
	FREEAREA *p_next;
	pthread_mutex_lock(&CS_FREEAREA_LIST);
	p_next=p_free_area_list;
	if(size==p_free_area_list->size){
		start_address=p_next->start_address;
		p_free_area_list=p_free_area_list->next;
		free(p_next);
	}
	else{
		if(size<p_next->size){
			start_address=p_next->start_address;
			p_next->start_address+=size;
			p_next->size-=size;
		    p_free_area_list=p_free_area_list->next;
			WF_insert_freearea(p_next);
		}
	}
	pthread_mutex_unlock(&CS_FREEAREA_LIST);
	return (start_address);

}

void change_freearea_list(FREEAREA *p){
    FREEAREA *p_before = NULL;
    FREEAREA *p_next = NULL;
    FREEAREA *temp = p_free_area_list;
    while(temp!=NULL){
	    if (temp -> next == p){
            p_before = temp;
            break;
        }
        temp=temp->next;
	};
    p_next = p->next;
    if(p_next!=NULL && p_before!=NULL){
        p_before -> next = p_next;
        return;
    }
    if(p_next!=NULL){
        p_free_area_list = p_next;
        return;
    }
    if(p_before!=NULL){
        p_before -> next=NULL;
        return;
    }
    p_free_area_list = NULL;
    return;
}
void WF_release_memory(int start_address,int size){
    int end_address=start_address + size;
    int fend_address=-1;
    int fstart_address=-1;
    FREEAREA *temp2=NULL;
    FREEAREA *p=NULL;
    FREEAREA *p_before=NULL;
    FREEAREA *p_next=NULL;
    FREEAREA *p_before_before=NULL;
    FREEAREA *p_next_before=NULL;
    
    p=p_free_area_list;
    temp2=p_free_area_list;
    
    FREEAREA *temp=NULL;
    temp=(FREEAREA *)malloc(sizeof(FREEAREA));
    temp->next=NULL;
    
    while(p!=NULL){
        fstart_address = p->start_address;
        fend_address=p->start_address + p->size;
        if(fend_address == start_address){
            p_before = p;
        }
        if(fstart_address==(start_address+size)){
            p_next = p;
        }
        p = p -> next;
    };
 
    if(p_next!=NULL && p_before!=NULL){
        temp->start_address = p_before->start_address;
        temp->size=  p_before->size + size + p_next->size;
        
        while(temp2!=NULL){
            if (temp2 -> next == p_before){
                p_before_before = temp2;
                break;
            }
            temp2=temp2 -> next;
        };
        temp2=p_free_area_list;
        
        while(temp2!=NULL){
            if (temp2 -> next == p_next){
                p_next_before = temp2;
                break;
            }
            temp2=temp2 -> next;
        };
        if(p_before->next == p_next){
            if(p_before_before !=NULL && p_next -> next != NULL){
                p_before_before -> next = p_next -> next;
            }
            if(p_before_before != NULL){
                p_before_before -> next = NULL;
            }
            if(p_next -> next != NULL){
                p_free_area_list = p_next -> next;
            }
        }
        if(p_before -> next != NULL && p_before -> next  == p_next_before){
            if(p_before_before != NULL && p_next -> next != NULL){
                p_before_before -> next = p_before -> next;
                p_next_before -> next = p_next -> next;
            }
            if(p_before_before != NULL){
                p_before_before -> next = p_before -> next;
                p_next_before -> next=NULL;
            }
            if(p_next -> next != NULL){
                p_next_before -> next = p_next -> next;
                p_free_area_list = p_before -> next;
            }
            
        }else{
            if(p_before_before!=NULL && p_next_before!= NULL){
                p_before_before -> next = p_before -> next;
                p_next_before -> next = p_next -> next;
            }
            if(p_before_before!=NULL){
                p_before_before -> next = p_before -> next;
                p_next_before -> next = NULL;
            }
            if(p_next_before!= NULL){
                p_next_before -> next = p_next -> next;
                p_free_area_list = p_next_before -> next;
            }
        }
        WF_insert_freearea(temp);
        return;
        
    }
    if(p_next != NULL){
        temp->start_address = start_address;
        temp->size= size + p_next->size;
        
        change_freearea_list(p_next);
        
        WF_insert_freearea(temp);
        
        pthread_mutex_lock(&CS_SCREEN);
        refresh();
        display_freearea_list(27,l_y2);
        l_y2 += 45;
        pthread_mutex_unlock(&CS_SCREEN);
        
        return;
    }
    if(p_before != NULL){
        temp->start_address = p_before->start_address;
        temp->size= p_before->size + size;
        
        change_freearea_list(p_before);
        
        WF_insert_freearea(temp);
        
        pthread_mutex_lock(&CS_SCREEN);
        refresh();
        display_freearea_list(27,l_y2);
        l_y2 += 45;
        pthread_mutex_unlock(&CS_SCREEN);
        return;
    }
    temp->start_address = start_address;
    temp->size= size;
    WF_insert_freearea(temp);
    
    pthread_mutex_lock(&CS_SCREEN);
    refresh();
    display_freearea_list(27,l_y2);
    l_y2 += 45;
    pthread_mutex_unlock(&CS_SCREEN);
    return;
}



void WF(){
    int i=0;
	int j=0;
    REQUIRE_MEMORY *p;
    
	for(j=0;j<MAX_THREAD;j++){
	   sem_init(&thread_end[j],0,0);
    }
    
	pthread_mutex_init(&CS_THREAD_MEMORY_LIST,NULL);
    pthread_mutex_init(&CS_FREEAREA_LIST,NULL);
	pthread_mutex_init(&CS_SCREEN,NULL);
    pthread_mutex_init(&WF_RELEASE,NULL);
 
	printw("Wirst Fit\n");
  	refresh();
    
    WF_initialize_freearea_list(init_free_area_table,5);
    
    
    p_thread_require_memory_queue=FF_initialize_require_memory_list(init_thread_require_memory_table,3);
  
    p_thread_residence_memory_list=FF_initialize_thread_residence_memory_list(init_thread_residence_memory_table,5);
    
    p=p_thread_require_memory_queue;
    
    pthread_mutex_lock(&CS_SCREEN);
	printw("Before allocation:\n");
	refresh();
	display_freearea_list(6,0);
    printw("After allocation:\n");
	pthread_mutex_unlock(&CS_SCREEN);
    
    while(p!=NULL){
        pthread_create(&h_thread[i],NULL,WF_thread,p);
        i++;
	    p=p->next;
    };
    
    for(j=0;j<MAX_THREAD;j++){
	   sem_wait(&thread_end[j]);
    }
	
    
	FF_delete_freearea_list();
	FF_delete_require_memory_list();
	FF_delete_thread_residence_memory_list();
	
	for(j=0;j<MAX_THREAD;j++){
	   sem_destroy(&thread_end[j]);
   }

	getch();
    printw("\n");	
	refresh();
}