#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>
#include <unistd.h>

pthread_mutex_t mutex_work_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_work_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_work_3 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_work_1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_work_2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_work_3 = PTHREAD_COND_INITIALIZER;

int work1_cond_ready = 0;
int work2_cond_ready = 0;
int work3_cond_ready = 0;
int exit_cond_ready = 0;

long int global_index = 0;

void *
work_thread_1(void * arg){
  while(1){
    pthread_mutex_lock(&mutex_work_1);
    if(1 != work1_cond_ready){
     pthread_cond_wait(&cond_work_1, &mutex_work_1); 
    }
    if(9999 == global_index){
      exit_cond_ready = 1;
    }
    if(exit_cond_ready != 0){
      work2_cond_ready = 1;
      pthread_cond_signal(&cond_work_2);
      usleep(5);
      pthread_exit(0);
    }
    printf("Start work thread 1.\n");
    work1_cond_ready = 0;
    
    /* DO SOMETHING IN THREAD ONE */
    global_index++;
    printf("global_index = %ld\n", global_index);

    work2_cond_ready = 1;
    usleep(500);
    
    pthread_mutex_unlock(&mutex_work_1);
    pthread_cond_signal(&cond_work_2);
    printf("End of thread 1.\n");
  }
}

void *
work_thread_2(void * arg){
  while(1){
    pthread_mutex_lock(&mutex_work_2);
    if(1 != work2_cond_ready){
     pthread_cond_wait(&cond_work_2, &mutex_work_2); 
    }
    if(exit_cond_ready != 0){
      work3_cond_ready = 1;
      pthread_cond_signal(&cond_work_3);
      usleep(5);
      pthread_exit(0);
    }
    printf("Start work thread 2.\n");
    work2_cond_ready = 0;
    
    /* DO SOMETHING IN THREAD TWO */
    work3_cond_ready = 1;
    usleep(500);
 
    pthread_mutex_unlock(&mutex_work_2);
    pthread_cond_signal(&cond_work_3);
    printf("End of thread 2.\n");
 }
}

void *
work_thread_3(void * arg){
  while(1){
    pthread_mutex_lock(&mutex_work_3);
    if(1 != work3_cond_ready){
     pthread_cond_wait(&cond_work_3, &mutex_work_3); 
    }
    if(exit_cond_ready != 0){
      usleep(5);
      pthread_exit(0);
    }
    printf("Start work thread 3.\n");
    work3_cond_ready = 0;
    
    /* DO SOMETHING IN THREAD THREE */
    work1_cond_ready = 1;
    usleep(500);
    
    pthread_mutex_unlock(&mutex_work_3);
    pthread_cond_signal(&cond_work_1);
    printf("End of thread 3.\n");
  }
}


int
main(){
  float *workspace_thread_1;
  float *workspace_thread_2; 
  float *workspace_thread_3;

  workspace_thread_1 = (float *)malloc(1000 * sizeof(float));
  workspace_thread_2 = (float *)malloc(1000 * sizeof(float));
  workspace_thread_3 = (float *)malloc(1000 * sizeof(float));

  pthread_t thread_1;
  pthread_create(&thread_1, NULL, work_thread_1, NULL);
  printf("Thread 1 lanched!\n");

  pthread_t thread_2;
  pthread_create(&thread_2, NULL, work_thread_2, NULL);
  printf("Thread 2 lanched!\n");
  
  pthread_t thread_3;
  pthread_create(&thread_3, NULL, work_thread_3, NULL);
  printf("Thread 3 lanched!\n"); 
 
  work1_cond_ready = 1;
  pthread_cond_signal(&cond_work_1);

  pthread_join(thread_1, NULL);

  pthread_mutex_destroy(&mutex_work_1);
  pthread_cond_destroy(&cond_work_1);
  
  pthread_mutex_destroy(&mutex_work_2);
  pthread_cond_destroy(&cond_work_2);

  pthread_mutex_destroy(&mutex_work_3);
  pthread_cond_destroy(&cond_work_3);
    
  return 0;
}
