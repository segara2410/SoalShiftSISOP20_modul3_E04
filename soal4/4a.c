#include <stdio.h> 
#include <pthread.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 

void *mult(void* arg) 
{ 
  int *data = (int *)arg; 
  int k = 0, i = 0; 

  int x = data[0]; 
  for (i = 1; i <= x; i++) 
    k += data[i]*data[i+x]; 

  int *p = (int*)malloc(sizeof(int)); 
  *p = k; 

  pthread_exit(p); 
} 

int main() 
{ 
  int matA[4][2] = { {1,  20 },
                     {4,  17 },
                     {7,  8 },
                     {1,  12 } };
  int matB[2][5] = { { 1,  3,  2,  14,  9 },
                     {19,  2, 16,  13,  1 } };

  int r1=4;
  int c1=2;
  int r2=2;
  int c2=5;
  int i,j,k; 
  int val = 1;

  // for (i = 0; i < r1; i++)  
  //   for (j = 0; j < c1; j++)  
  //     matA[i][j] = rand() % 20 + 1;  
  //     // matA[i][j] = val++;  

  // for (i = 0; i < r2; i++)  
  //   for (j = 0; j < c2; j++)
  //   matB[i][j] = rand() % 20 + 1;  
  //   // matB[i][j] = val++;  

  printf("First Matrix:\n");
  for (i = 0; i < r1; i++){ 
    for(j = 0; j < c1; j++) 
     printf("%-2d ",matA[i][j]); 
    printf("\n"); 
  } 

  printf("Second Matrix:\n");
  for (i = 0; i < r2; i++){ 
    for(j = 0; j < c2; j++) 
      printf("%-2d ",matB[i][j]); 
    printf("\n");     
  } 

  int max = r1*c2; 

  pthread_t *threads; 
  threads = (pthread_t*)malloc(max*sizeof(pthread_t)); 

  int count = 0; 
  int* data = NULL; 
  for (i = 0; i < r1; i++) 
    for (j = 0; j < c2; j++) 
    { 
      data = (int *)malloc((20)*sizeof(int)); 
      data[0] = c1; 

      for (k = 0; k < c1; k++) 
      data[k+1] = matA[i][k]; 

      for (k = 0; k < r2; k++) 
      data[k+c1+1] = matB[k][j]; 

      pthread_create(&threads[count++], NULL,  
        mult, (void*)(data)); 
    } 

  key_t key = 1234;
  int *value;

  int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
  value = shmat(shmid, NULL, 0);

  printf("Result Matrix:\n"); 
  for (i = 0; i < max; i++)  
  { 
    void *k; 

    pthread_join(threads[i], &k); 
    
    int *p = (int *)k;
    value[i] = *p;
    printf("%-3d ", value[i]); 
    if ((i + 1) % c2 == 0) 
      printf("\n"); 
  }
  shmdt((void *) value);

  return 0; 
} 