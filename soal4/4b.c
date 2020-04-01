#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

void *find_fact(void* arg) 
{ 
  int data = *(int*)arg;
  unsigned long long k = 1; 

  for (int i = 1; i <= data; i++)
    k = k * i;

  unsigned long long *p = (unsigned long long*)malloc(sizeof(unsigned long long)); 
  *p = k; 

  pthread_exit(p); 
}

int main()
{
  key_t key = 1234;
  int *value;
  int fact[20];

  int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
  value = shmat(shmid, NULL, 0);

  for (int i = 0; i < 20; i++)  
  { 
    printf("%-3d ", value[i]); 
    if ((i + 1) % 5 == 0) 
      printf("\n"); 
  }
  
  pthread_t threads[20]; 
  for (int i = 0; i < 20; i++)  
  {
    // int *new_val = &i;
    int *new_val = &value[i];
    pthread_create(&threads[i], NULL, find_fact, (void *)new_val);
  }
  
  for (int i = 0; i < 20; i++)  
  { 
    void *k; 

    pthread_join(threads[i], &k);
    
    unsigned long long *p = (unsigned long long *)k;
    printf("%-20llu ",*p);
    if ((i + 1) % 5 == 0)
      printf("\n"); 
  }

  shmdt(value);
  shmctl(shmid, IPC_RMID, NULL);
}