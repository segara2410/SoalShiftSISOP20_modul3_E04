#include <stdio.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int* shmpokemon;
int* shmlp;
int* shmpb;
int* shmb;

void killTraizone(int intpid)
{
  if(fork() == 0)
  {
    if(fork() == 0)
    {
      char * argv[] = {"killall", "soal1_traizone", NULL};
      execv("/usr/bin/killall", argv);
    }
    wait(NULL);
    char pid[64];
    sprintf(pid, "%d", intpid);
    char * argv[] = {"kill", "-9", pid, NULL};
    execv("/bin/kill", argv);
  }
}

void* randomPokemon() 
{
  int pokemon;
	srand(time(NULL));
  while (1) 
  {
    int r = rand() % 100;
    if (r < 5) 
      pokemon = (rand() % 5) + 10;
    else if (r < 20)
      pokemon = (rand() % 5) + 5;
    else
      pokemon = (rand() % 5);

    if (rand() % 8000 == 0)
      pokemon += 15;
    
    *shmpokemon = pokemon;
    sleep(1);
  }
}

void* restockItem() 
{
  while (1)
  {
    sleep(10);
    *shmlp += 10;
    *shmpb += 10;
    *shmb += 10;
    
    if (*shmlp > 200) 
      *shmlp = 200;
    if (*shmpb > 200) 
      *shmpb = 200;
    if (*shmb > 200) 
      *shmb = 200;
  }
}

int main() 
{
	key_t key1 = 2501;
  int shmidpokemon = shmget(key1, sizeof(int), IPC_CREAT | 0666);
  shmpokemon = shmat(shmidpokemon, NULL, 0);
  *shmpokemon = 100;

	key_t key2 = 2502;
  int shmidlp = shmget(key2, sizeof(int), IPC_CREAT | 0666);
	shmlp = shmat(shmidlp, NULL, 0);
  *shmlp = 100;
  
	key_t key3 = 2503;
  int shmidpb = shmget(key3, sizeof(int), IPC_CREAT | 0666);
	shmpb = shmat(shmidpb, NULL, 0);
  *shmpb = 100;
  
	key_t key4 = 2504;  
  int shmidb = shmget(key4, sizeof(int), IPC_CREAT | 0666);
	shmb = shmat(shmidb, NULL, 0);
  *shmb = 100;

  pthread_t thread[2];
  pthread_create(&thread[0], NULL, restockItem, NULL);
  pthread_create(&thread[1], NULL, randomPokemon, NULL);
  
  while (1)
  {
    printf("1. Shutdown\nInput: ");
    int x;
    scanf("%d", &x);
    
    int pid = getpid();

    if (x == 1)
    {
      shmdt(shmpokemon);
      shmdt(shmlp);
      shmdt(shmpb);
      shmdt(shmb);

      shmctl(shmidpokemon, IPC_RMID, NULL);
      shmctl(shmidlp, IPC_RMID, NULL);
      shmctl(shmidpb, IPC_RMID, NULL);
      shmctl(shmidb, IPC_RMID, NULL);

      killTraizone(pid);
      sleep(1);
    }
    else
      puts ("invalid command");
  }
}