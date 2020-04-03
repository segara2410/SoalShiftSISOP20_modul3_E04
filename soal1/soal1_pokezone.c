#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
int new_socket;

int lullaby_powder_count = 100;
int pokeball_count = 100;
int berry_count = 100;

int escape_rate; 
int capture_rate;
int pokedollar;

char current_pokemon[20];
char normal_pokemon[5][20] = { "Bulbasaur", "Charmander", "Squirtle", "Rattata", "Caterpie" };
char rare_pokemon[5][20] = { "Pikachu", "Eevee", "Jigglypuff", "Snorlax", "Dragonite" };
char legendary_pokemon[5][20] = { "Mew", "Mewtwo", "Moltres", "Zapdos", "Articuno" };
char pokemon_message[50];

void *handleRequest()
{
  while(1)
  {
    char buffer[1024] = {0};

    memset(buffer, 0, sizeof(buffer)); 
    int valread = read(new_socket, buffer, 1024);
    if (strcmp(buffer, "request pokemon") == 0)
      send(new_socket, pokemon_message, strlen(pokemon_message), 0);
  }
}

void *randomPokemon()
{
  while(1)
  {
    int encounter_rate = rand() % 100 + 1;
    int pokemon = rand() % 5;
    int shiny = rand() % 8000 + 1;
    
    if (encounter_rate <= 80)
    {
      strcpy(current_pokemon, normal_pokemon[pokemon]);
      escape_rate = 5;
      capture_rate = 70;
      pokedollar = 80;
    }
    else if (encounter_rate <= 95)
    {
      strcpy(current_pokemon, rare_pokemon[pokemon]);
      escape_rate = 10;
      capture_rate = 50;
      pokedollar = 100;
    }
    else if (encounter_rate <= 100)
    {
      strcpy(current_pokemon, legendary_pokemon[pokemon]);
      escape_rate = 20;
      capture_rate = 30;
      pokedollar = 200;
    }

    if (shiny == 1)
    {
      escape_rate += 5;
      capture_rate -= 20;
      pokedollar += 5000;
    }

    sprintf(pokemon_message, "%s %d %d %d", current_pokemon, escape_rate, capture_rate, pokedollar);
    sleep(1);
  }
}

void killTraizone(int intpid)
{
  if(fork() == 0)
  {
    if(fork() == 0)
    {
      char * argv[] = {"killall", "soal1_traizone", NULL};
      execv("/usr/bin/killall", argv);
    }
    char pid[64];
    sprintf(pid, "%d", intpid);
    char * argv[] = {"kill", "-9", pid, NULL};
    execv("/bin/kill", argv);
  }
}

int main()
{
  int server_fd, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
    
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

  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
      perror("accept");
      exit(EXIT_FAILURE);
  }

  char cmd[1024];
  printf("1. Shutdown Game\n");
  int pid = getpid();

  pthread_t threads[2];
  pthread_create(&threads[0], NULL, randomPokemon, NULL);
  pthread_create(&threads[1], NULL, handleRequest, NULL);

  while (1)
  {
    scanf("%s", cmd);
    if (strcmp(cmd, "shutdown") == 0)
      killTraizone(pid);
    else
      puts("invalid command");
  }
}