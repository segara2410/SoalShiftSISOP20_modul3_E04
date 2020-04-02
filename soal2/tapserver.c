#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080

FILE *fp;
int server_fd, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
int player = 0;
int health[1000];
struct Queue* queue;
int matchup[1000];

struct Queue 
{ 
  int front, rear, size; 
  unsigned capacity; 
  int* array; 
};


struct Queue* createQueue(unsigned capacity) 
{ 
  struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue)); 
  queue->capacity = capacity; 
  queue->front = queue->size = 0;  
  queue->rear = capacity - 1;
  queue->array = (int*) malloc(queue->capacity * sizeof(int)); 
  return queue; 
} 
  
int isFull(struct Queue* queue) 
{  
  return (queue->size == queue->capacity); 
} 
  
int isEmpty(struct Queue* queue) 
{  
  return (queue->size == 0); 
}
  
void enqueue(struct Queue* queue, int item) 
{ 
  if (isFull(queue)) 
    return; 
  queue->rear = (queue->rear + 1) % queue->capacity; 
  queue->array[queue->rear] = item; 
  queue->size = queue->size + 1; 
} 
  
int dequeue(struct Queue* queue) 
{ 
  if (isEmpty(queue)) 
    return -1; 
  int item = queue->array[queue->front]; 
  queue->front = (queue->front + 1) % queue->capacity; 
  queue->size = queue->size - 1; 
  return item; 
}

int accountExist(char userpass[])
{
  FILE *file = fopen("akun.txt", "r");
  if ( file != NULL )
  {
    char line[256];
    while (fgets(line, sizeof line, file) != NULL)
    {
      if (strcmp(line, userpass) == 0)
      {
        fclose(file);
        return 1;
      }
    }
    fclose(file);
  }

  return 0;
}

void *createServer(void* arg)
{
  int player_id = *(int*)arg;

  int new_socket;

  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  int state_login = -1;

  char buffer[1024] = {0};
  char new_buffer[1024] = "login";
  while(1)
  {
    char userpass[1024] = {0};

    memset(buffer, 0, sizeof(buffer)); 
    valread = read(new_socket, buffer, 1024);
    char value_now[1024];

    if(strcmp(buffer, "login") == 0)
    {
      send(new_socket, new_buffer, strlen(new_buffer), 0);
      read(new_socket, buffer, 1024);
      
      char login_message[100];
      if(accountExist(buffer))
        strcpy(login_message, "Auth success");
      else
        strcpy(login_message, "Auth failed");

      send(new_socket, login_message, strlen(login_message), 0); 
      memset(buffer, 0, sizeof(buffer)); 
      state_login = 1;
    }
    else if(strcmp(buffer, "register") == 0)
    {
      fp = fopen ("akun.txt", "a");
      
      send(new_socket, buffer, strlen(buffer), 0);
      memset(buffer, 0, sizeof(buffer)); 
      
      read(new_socket, buffer, 1024);
      fputs(buffer, fp);
      fclose(fp);

      char register_success[100] = "register success";
      send(new_socket, register_success, strlen(register_success), 0); 

      fp = fopen ("akun.txt", "r");
      char c = fgetc(fp); 
      while (c != EOF) 
      { 
        printf ("%c", c); 
        c = fgetc(fp); 
      }
      fclose(fp);

      state_login = 0;
    }
    else if(strcmp(buffer, "find") == 0)
    {
      // puts(buffer);
      int enemy = -1;
      int waiting = 0;
      char status[100] = "Waiting for player ...";
      // puts(status);
      
      while((enemy == -1 || enemy == player_id) && matchup[player_id] == -1)
      {
        if(isEmpty(queue))
          enqueue(queue, player_id);
        else
          enemy = dequeue(queue);

        if(enemy != player_id && enemy != -1)
        {
          health[player_id] = 100;
          health[enemy] = 100;

          matchup[player_id] = enemy;
          matchup[enemy] = player_id;
          
          while(queue->front == player_id || queue->front == enemy)
            dequeue(queue);
          break;
        }
        else
        {
          if(waiting == 0)
          {
            send(new_socket, status, sizeof(status), 0);
            waiting = 1;
          }
        }
        sleep(1);
      }

      enemy = matchup[player_id];

      strcpy(status, "Game dimulai silahkan tap tap secepat mungkin !!");
      send(new_socket, status, sizeof(status), 0);

      memset(status, 0, sizeof(status));
      int current_health = health[player_id];
      while(health[player_id] && health[enemy])
      {
        char new_status[1024] = "kosongs";
        read(new_socket, buffer, 1024);

        if(strcmp(buffer, "hit !!") == 0)
        {
          // printf("%d vs %d\n", health[player_id], health[enemy]);
          health[enemy] -= 10;
          memset(buffer, 0, sizeof(buffer));
        }
        else
          strcpy(new_status, "kosongs");

        if(health[player_id] != current_health)
        {
          // printf("%d nyawa %d\n", player_id, health[player_id]);
          current_health = health[player_id];
          sprintf(new_status, "%d", current_health);
        }

        send(new_socket, new_status, sizeof(new_status), 0);

      }
      if (health[player_id])
        strcpy(status, "Game berakhir kamu menang");
      else
        strcpy(status, "Game berakhir kamu kalah");
      send(new_socket, status, sizeof(status), 0);
      matchup[player_id] = -1;
    }
    else if (strcmp(buffer, "logout") == 0)
    {
      state_login = -1;
    }
    else
    {
      strcpy(buffer, "buffer invalid command");
      send(new_socket, buffer, sizeof(buffer), 0);
      memset(buffer, 0, sizeof(buffer)); 
    }
  }
}


int main(int argc, char const *argv[]) 
{
  memset(matchup, -1, sizeof(matchup));
  queue = createQueue(1000); 
  fp = fopen ("akun.txt", "a");
  fclose(fp);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
    
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );
    
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  pthread_t threads[100]; 
  for (int i = 0; i < 100; i++)
  {
    int *new_val = &i;
    pthread_create(&threads[i], NULL, createServer,  (void *)new_val);
  }
  
  for (int i = 0; i < 100; i++)  
    pthread_join(threads[i], NULL);

  return 0;
}