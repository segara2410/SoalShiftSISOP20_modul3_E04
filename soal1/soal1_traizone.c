#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h> 

#define PORT 8080

int sock = 0, valread;
struct sockaddr_in serv_addr;
char buffer[1024];

int mode = -1;
int menu = -1;
int pokemon_found = -1;

char current_pokemon[20];
int escape_rate;
int capture_rate; 
int pokedollar;

void *cariPokemon()
{
  while(mode == 1 && menu == 1)
  {
    int chance = rand() % 100 + 1;
    printf("%d\n", chance);
    if(chance > 40)
    {
      strcpy(buffer, "request pokemon");
      send(sock, buffer, strlen(buffer), 0);

      valread = read(sock, buffer, 1024);
      char * token = strtok(buffer, " ");
      int i = 0;
      while( token != NULL ) 
      {
        if (i == 0)
          strcpy(current_pokemon, token);
        else if (i == 1)
          escape_rate = atoi(token);
        else if (i == 2)
          capture_rate = atoi(token);
        else if (i == 3)
          pokedollar = atoi(token);

        token = strtok(NULL, " ");
        i++;
      }
      printf ("%s ditemukan !!\n", current_pokemon);
      pokemon_found = 1;
      pthread_exit(0);
    }
    sleep(10);
  }
}

int main()
{
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
      printf("\nCan't connect to Pokezone\n");
      return -1;
  }

  while (1)
  {
    srand(time(0));
    char cmd[1024];

    mode = 1;

    while (mode == 1)
    {
      printf("Normal Mode\n");
      printf("1. Cari Pokemon\n");
      printf("2. Pokedex\n");
      printf("3. Shop\n");
      scanf("%s", cmd);

      if (strcmp(cmd, "cari") == 0)
        menu = 1;
      else if (strcmp(cmd, "pokedex") == 0)
        menu = 2;
      else if (strcmp(cmd, "shop") == 0)
        menu = 3;
      else
        puts("invalid command");

      if (menu == 1)
      {
        puts("\nSedang Mencari ...");
        printf("1. Berhenti\n");
        printf("2. Capture\n");

        pthread_t thread; 
        pthread_create(&thread, NULL, cariPokemon, NULL);

        pokemon_found = 0;
        while(!pokemon_found)
        {
          scanf("%s", cmd);
          if (strcmp(cmd, "berhenti") == 0)
          {
            pthread_cancel(thread);
            menu = -1;
            break;
          }
          else if (strcmp(cmd, "capture") == 0)
          {
            if (pokemon_found)
            {
              mode = 2;
              menu = -1;

              break;
            }
            else 
              puts("pokemon belum ditemukan!");
          }
          else
          {
            puts("invalid command");
          }
        }
      }
      else if (menu == 2)
      {
        puts("Pokedex");
      }
      else if (menu == 3)
      {
        puts("Shop");
      }
    }

    while (mode == 2)
    {
      printf("\nCapture Mode\n");
      printf("1. Tangkap\n");
      printf("2. Item\n");
      printf("3. Keluar\n");
      scanf("%s", cmd);

      if (strcmp(cmd, "tangkap") == 0)
        menu = 1;
      else if (strcmp(cmd, "item") == 0)
        menu = 2;
      else if (strcmp(cmd, "keluar") == 0)
        menu = 3;
      else
        puts("invalid command");

      if (menu == 1)
      {
        puts("\nTangkap");
      }
      else if (menu == 2)
      {
        puts("item");
      }
      else if (menu == 3)
      {
        mode = 1;
      }
      puts("");
    }
  }
}