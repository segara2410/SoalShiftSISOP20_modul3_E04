#include <stdio.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* pokemon_list[30] = {
	"Bulbasaur",
	"Charmander",
	"Squirtle",
	"Rattata",
	"Caterpie",
	"Pikachu",
	"Eevee",
	"Jigglypuff",
	"Snorlax",
	"Dragonite",
	"Mew",
	"Mewtwo",
	"Moltres",
	"Zapdos",
	"Articuno",
	"Shiny Bulbasaur",
	"Shiny Charmander",
	"Shiny Squirtle",
	"Shiny Rattata",
	"Shiny Caterpie",
	"Shiny Pikachu",
	"Shiny Eevee",
	"Shiny Jigglypuff",
	"Shiny Snorlax",
	"Shiny Dragonite",
	"Shiny Mew",
	"Shiny Mewtwo",
	"Shiny Moltres",
	"Shiny Zapdos",
	"Shiny Articuno"
};

int mode;
int mencari;
pthread_t threadcari;

int input;
pthread_t inputthread;

int shmidpokemon;
int shmidlp;
int shmidpb;
int shmidb;
int* shmpokemon;
int* shmlp;
int* shmpb;
int* shmb;


pthread_mutex_t pokeslotwrite;
int pokeslot;

pthread_mutex_t pokemoncapturewrite;
int pokemon_capture;
pthread_t pokemoncapturethread;

pthread_mutex_t pokemonwrite;
int pokemon[7];
int pokemon_AP[7];
pthread_t pokemon_thread[7];

int pokedollar;
int lullaby;
int pokeball;
int berry;

int effect;
pthread_t lullabythread;

void* lullabyHandler() 
{
	sleep(10);
	effect = 0;
	printf("Efek lullaby powder habis.\n");
	pthread_exit(0);
}

void* pokemonHandler() 
{
	int slot = pokeslot;
	pthread_mutex_unlock(&pokeslotwrite);
	while (1) 
  {
		sleep(10);
		if (mode == 1) 
      continue;
    
		pthread_mutex_lock(&pokemonwrite);
		pokemon_AP[slot] -= 10;
		if (pokemon_AP[slot] == 0) 
    {
			if (rand() % 100 < 90) 
      {
				printf("Ada pokemon yang terlepas.\n");
				pokemon[slot] = -1;
				pokemon_AP[slot] = -1;
				pthread_exit(0);
			} 
      else
				pokemon_AP[slot] = 50;
		}
		pthread_mutex_unlock(&pokemonwrite);
	}
}

void* capturePokemon() 
{
	int chance = (pokemon_capture % 15) / 5;
	
  if (chance == 0) 
    chance = 5;
	else if (chance == 1) 
    chance = 10;
	else if (chance == 2) 
    chance = 20;
	
  if (pokemon_capture >= 15) 
    chance += 5;
    
	while (1) 
  {
		sleep(20);
		if (effect) 
      continue;
		if (rand() % 100 < chance) 
    {
			printf("Pokemon telah escape dari pencarian pokemon.\n");
			pthread_mutex_lock(&pokemoncapturewrite);
			pokemon_capture = -1;
			pthread_mutex_unlock(&pokemoncapturewrite);
			pthread_exit(0);
		}
	}
}

void* cariHandler() 
{
	while (1) 
  {
		sleep(10);
		if (rand() % 100 < 60)
    {
			mencari = 0;
			pthread_mutex_lock(&pokemoncapturewrite);
			pokemon_capture = *shmpokemon;
			pthread_mutex_unlock(&pokemoncapturewrite);
			pthread_create(&pokemoncapturethread, NULL, capturePokemon, NULL);
			mencari = 0;
			mode = 1;
			input = -1;
			pthread_cancel(inputthread);
			pthread_exit(0);
		}
	}
}

void cariPokemon() 
{
	mencari = !mencari;
	if (mencari)
		pthread_create(&threadcari, NULL, cariHandler, NULL);
	else
		pthread_cancel(threadcari);
}

void pokedex() 
{
	printf("Slot  Pokemon           Affection Points\n");
	
  for (int i = 0; i < 7; i++) 
  {
		printf("%d     ", i+1);
		if (pokemon[i] == -1)
			printf("-                 -\n");
		else
			printf("%-18s%d\n", pokemon_list[pokemon[i]], pokemon_AP[i]);
	}

	printf("1. Lepas\n2. Beri semua pokemon Berry\n3. Keluar\nInput: ");
	int choice;
	scanf("%d", &choice);

	if (choice == 1) 
  {
		printf("Pilih pokemon (1-7): ");
		int poke = -1;
		while (poke == -1) 
    {
			scanf("%d", &poke);
		
    	if (poke >= 1 && poke <= 7 && pokemon[poke-1] != -1)
      {
				pthread_mutex_lock(&pokemonwrite);
				pthread_cancel(pokemon_thread[poke-1]);
				if (pokemon[poke-1] >= 15) 
        {
					pokedollar += 5000;
					pokemon[poke-1] -= 15;
				}

				if (pokemon[poke-1] < 5)
					pokedollar += 80;
				else if (pokemon[poke-1] < 10)
					pokedollar += 100;
				else if (pokemon[poke-1] < 15)
					pokedollar += 200;
				
				pokemon[poke-1] = -1;
				pokemon_AP[poke-1] = 0;
				pthread_mutex_unlock(&pokemonwrite);
				break;
			}
			printf("invalid command\n");
		}
	} 
  else if (choice == 2) 
  {
		if (berry > 0) 
    {
			berry--;
			pthread_mutex_lock(&pokemonwrite);
			for (int i = 0; i < 7; i++) {
				if (pokemon_AP[i] != -1) {
					pokemon_AP[i] += 10;
				}
			}
			pthread_mutex_unlock(&pokemonwrite);
			printf("Berhasil memberi berry.\n");
		} 
    else
			printf("Gagal memberi berry.\n");
  }
	else if (choice == 3)
		return;
}

void shop() 
{
	printf("Pokedollar: %d\n", pokedollar);
	printf("    Item            Stock  Shop  Harga\n");
	printf("1.  Lullaby Powder  %-3d    %-3d   60\n", lullaby, *shmlp);
	printf("2.  Pokeball        %-3d    %-3d   5 \n", pokeball, *shmpb);
	printf("3.  Berry           %-3d    %-3d   15\n", berry, *shmb);
	printf("4.  Keluar\n");
	printf("Choice : ");
	
  int choice;
	scanf("%d", &choice);
	
  if (choice == 4) 
    return;
	
  printf("Jumlah: ");
	
  int jml;
	scanf("%d", &jml);
	
  if (jml > 99)
  {
		printf("Gagal membeli.\n");
		return;
	}
	if (choice == 1) 
  {
		if (jml <= *shmlp && lullaby + jml <= 99 && pokedollar >= jml * 60) 
    {
			*shmlp -= jml;
			pokedollar -= jml * 60;
			lullaby += jml;
			printf("Berhasil membeli %d Lullaby Powder.\n", jml);
			return;
		}
	} 
  else if (choice == 2) 
  {
		if (jml <= *shmpb && pokeball + jml <= 99 && pokedollar >= jml * 5)
    {
			*shmpb -= jml;
			pokedollar -= jml * 5;
			pokeball += jml;
			printf("Berhasil membeli %d Pokeball.\n", jml);
			return;
		}
	}
  else if (choice == 3) 
  {
		if (jml <= *shmb && berry + jml <= 99 && pokedollar >= jml * 15) 
    {
			*shmb -= jml;
			pokedollar -= jml * 15;
			berry += jml;
			printf("Berhasil membeli %d Berry.\n", jml);
			return;
		}
	}
  else if (choice == 4)
		return;
	
	printf("Gagal membeli.\n");
}

void capturePokemon() 
{
	if (pokemon_capture == -1)
  {
		printf("Tidak ada pokemon.\n");
		return;
	}
	if (pokeball == 0) 
  {
		printf("Tidak ada pokeball.\n");
		return;
	}
	pokeball--;
	int chance = (pokemon_capture % 15) / 5;

	if (chance == 0)
    chance = 70;
	else if (chance == 1)
    chance = 50;
	else if (chance == 2)
    chance = 30;

	if (pokemon_capture >= 15) 
    chance -= 20;

	if (effect)
    chance += 20;
	
  if (rand() % 100 < chance) 
  {
		printf("Berhasil menangkap %s!\n", pokemon_list[pokemon_capture]);
		int slot = -1;
		for (int i = 0; i < 7; i++) 
    {
			if (pokemon[i] == -1)
      {
				slot = i;
				break;
			}
		}
		if (slot == -1) 
    {
			int money = (pokemon_capture % 15) / 5;
			
      if (money == 0)
        money = 80;
			else if (money == 1) 
        money = 100;
			else if (money == 2)
        money = 200;
			if (pokemon_capture >= 15)
        money += 5000;
			
      pokedollar += money;
			printf("Slot pokemon penuh! Anda mendapatkan %d.\n", money);
		} 
    else 
    {
			pthread_mutex_lock(&pokemonwrite);
			pokemon[slot] = pokemon_capture;
			pokemon_AP[slot] = 100;
			pthread_mutex_unlock(&pokemonwrite);
			pthread_mutex_lock(&pokeslotwrite);
			pokeslot = slot;
			pthread_create(&pokemon_thread[slot], NULL, pokemonHandler, NULL);
			pthread_cancel(pokemoncapturethread);
			pthread_mutex_lock(&pokemoncapturewrite);
			pokemon_capture = -1;
			pthread_mutex_unlock(&pokemoncapturewrite);
		}
	} 
  else 
		printf("Tidak berhasil menangkap %s.\n", pokemon_list[pokemon_capture]);
}

void useItem()
{
	if (lullaby <= 0) 
  {
		printf("Lullaby Powder tidak cukup.\n");
		return;
	}
	if (effect)
  {
		printf("Efek lullaby powder masih ada.\n");
		return;
	}
	puts("Gunakan lullaby powder?");
  puts("1. Ya");
  puts("2. Tidak");
  printf("Choice : ");

	int choice;
	scanf("%d", &choice);
	if (choice == 1) 
  {
		lullaby--;
		effect = 1;
		pthread_create(&lullabythread, NULL, lullabyHandler, NULL);
	}
}

void cleanup()
{
	shmdt(shmpokemon);
	shmdt(shmlp);
	shmdt(shmpb);
	shmdt(shmb);

	shmctl(shmidpokemon, IPC_RMID, NULL);
	shmctl(shmidlp, IPC_RMID, NULL);
	shmctl(shmidpb, IPC_RMID, NULL);
	shmctl(shmidb, IPC_RMID, NULL);

	pthread_mutex_destroy(&pokemonwrite);
	pthread_mutex_destroy(&pokemoncapturewrite);
	pthread_mutex_destroy(&pokeslotwrite);

	exit(EXIT_SUCCESS);
}

void* inputHandler()
{
	scanf("%d", &input);
}

int main() 
{
	srand(time(NULL));
	signal(SIGINT, cleanup);

	key_t key1 = 2501;
	key_t key2 = 2502;
	key_t key3 = 2503;
	key_t key4 = 2504;

  int shmidpokemon = shmget(key1, sizeof(int), IPC_CREAT | 0666);
  int shmidlp = shmget(key2, sizeof(int), IPC_CREAT | 0666);
  int shmidpb = shmget(key3, sizeof(int), IPC_CREAT | 0666);
  int shmidb = shmget(key4, sizeof(int), IPC_CREAT | 0666);

  shmpokemon = shmat(shmidpokemon, NULL, 0);
	shmlp = shmat(shmidlp, NULL, 0);
	shmpb = shmat(shmidpb, NULL, 0);
	shmb = shmat(shmidb, NULL, 0);
	
  memset(pokemon, -1, sizeof(pokemon));
	memset(pokemon_AP, -1, sizeof(pokemon_AP));
	memset(pokemon_thread, 0, sizeof(pokemon_thread));

	pthread_mutex_init(&pokemonwrite, NULL);
	pthread_mutex_init(&pokemoncapturewrite, NULL);
	pthread_mutex_init(&pokeslotwrite, NULL);

	pokemon_capture = -1;
	mode = 0;
	mencari = 0;
	lullaby = 0;
	pokeball = 0;
	berry = 0;
	effect = 0;
	pokedollar = 500;
	pokeslot = -1;

	while (1) 
  {
    printf("\n");
    if (mode == 0) 
    {
      if (mencari) 
        printf("Sedang Mencari...\n1. Berhenti Mencari\n2. Pokedex\n3. Shop\n4. Go to capture mode\nInput: ");
      else 
        printf("Normal Mode\n1. Cari Pokemon\n2. Pokedex\n3. Shop\n4. Go to capture mode\nInput: ");
    } 
    else if (mode == 1) 
    {
      if (pokemon_capture == -1) 
        printf("Capture Mode\n1. Tangkap\n2. Item\n3. Keluar\nInput: ");
      else 
        printf("Capture Mode\nMenemukan pokemon: %s\n1. Tangkap\n2. Item\n3. Keluar\nInput: ", 
          pokemon_list[pokemon_capture]);
    }

		pthread_create(&inputthread, NULL, inputHandler, NULL);
		pthread_join(inputthread, NULL);

		if (input == -1) 
    {
			printf("\n");
			continue;
		}

		int choice = input;
		
    if (mode == 0) 
    {
			if (choice == 1)
				cariPokemon();
			else if (choice == 2)
				pokedex();
			else if (choice == 3)
				shop();
			else if (choice == 4)
				mode = 1;			
		}
    else if (mode == 1)
    {
			if (choice == 1)
				capturePokemon();
			else if (choice == 2)
				useItem();
			else if (choice == 3)
				mode = 0;
		}
	}
}