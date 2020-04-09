#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>
#include <pthread.h>

#define PORT 8080
  
int space_scanner = 0;
int space_pressed = 0;
int sock = 0, valread;
int running_game = 0;
int screen = 1;

static struct termios old, new;

void initTermios(int echo)
{
  tcgetattr(0, &old);
  new = old;
  new.c_lflag &= ~ICANON;
  new.c_lflag &= echo ? ECHO : ~ECHO;
  tcsetattr(0, TCSANOW, &new);
}
 
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

char getch()
{
  char ch;
  initTermios(0);
  ch = getchar();
  resetTermios();
  return ch;
}

void *spaceScanner()
{
  while(running_game)
    while(space_scanner)
    {
      if (getch() == 32)
      {
        space_pressed = 1;
        space_scanner = 0;
      }
    }
  pthread_exit(0);
}

void *runGame()
{
  running_game = 1;
  while(1)
  {
    space_scanner = 1;
    char buffer[1024] = {0};
    char new_buffer[64] = "kosong";

    if (space_pressed)
    {
      char hit[10] = "hit !!";
      fflush(stdin);
      strcpy(new_buffer, hit);
      space_pressed = 0;
    }

    send(sock, new_buffer, strlen(new_buffer), 0 );
    memset(new_buffer, 0, sizeof(new_buffer));
    valread = read(sock, buffer, 1024);
    if (strcmp(buffer, "kosongs") != 0 && strcmp(buffer, "Game berakhir kamu kalah") != 0 && strcmp(buffer, "Game berakhir kamu menang") != 0)
      puts(buffer);

    if (strcmp(buffer, "Game berakhir kamu kalah") == 0 || strcmp(buffer, "Game berakhir kamu menang") == 0)
    {
      puts(buffer);
      screen = 2;
      break;
    }
  }
  space_scanner = 0;
  running_game = 0;
  pthread_exit(0);
}

int main(int argc, char const *argv[]) {
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1)
    {
      if (screen == 1)
      {
        char cmd[100];
        memset(cmd, 0, sizeof(cmd)); 
        printf("1. Login\n");
        printf("2. Register\n");
        printf("   Choices : ");
        fflush(stdout);
        scanf("%s", cmd);
        
        send(sock, cmd , strlen(cmd), 0 );
        valread = read(sock, buffer, 1024);

        if (strcmp(buffer, "login") == 0 || strcmp(buffer, "register") == 0)
        {
          char userpass[1024] = {0};
          char username[128];
          char password[128];
          int f;
          printf("   Username : ");
          scanf("%d", &f);
          memset(username, 0, sizeof(username));
          scanf("%[^\n]", username);

          printf("   Password : ");
          memset(password, 0, sizeof(password));
          scanf("%d", &f);
          scanf("%[^\n]", password);

          sprintf(userpass, "%s - %s\n", username, password);
          
          send(sock, userpass, strlen(userpass), 0 );
          
          read(sock, buffer, 1024);

          if(strcmp(buffer, "Auth success") == 0)
          {
            printf("   Login success\n");
            screen = 2;
          }
          else if(strcmp(buffer, "Auth failed") == 0)
            printf("   Login failed\n");
          else 
            printf("   Register success\n");
            
          memset(buffer, 0, sizeof(buffer));
        }      
        else
        {
          printf("   %s\n", buffer);
          memset(buffer, 0, sizeof(buffer));
        }


      }
      
      if (screen == 2)
      {
        char cmd[100];
        printf("1. Find Match\n2. Logout\n   Choices : ");
        scanf("%s", cmd);
        if (strcmp(cmd, "find") == 0)
        {
          send(sock, cmd , strlen(cmd), 0 );
      
          memset(buffer, 0, sizeof(buffer)); 
          valread = read(sock, buffer, 1024);

          if(strcmp(buffer, "Waiting for player ...") == 0)
            printf("   %s\n", buffer);
          
          while(1)
          {
            if(strcmp(buffer, "Game dimulai silahkan tap tap secepat mungkin !!") == 0)
             break;
            else
              read(sock, buffer, 1024);
          }
          screen = 3;
        }
        else if (strcmp(cmd, "logout") == 0)
        {
          send(sock, cmd , strlen(cmd), 0 );
          screen = 1;
        }
        else
          puts("invalid command");
      }

      if (screen == 3)
      {
        int health = 100;
        printf("   %s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
        
        space_scanner = 1;
        pthread_t threads[2]; 
        pthread_create(&threads[0], NULL, runGame, NULL);
        pthread_create(&threads[1], NULL, spaceScanner, NULL);

        pthread_join(threads[0], NULL);
        pthread_cancel(threads[1]);
        resetTermios();
        read(sock, buffer, 1024);
        memset(buffer, 0, sizeof(buffer));
      }
    }
}