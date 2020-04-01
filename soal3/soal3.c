#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX 256

char cwd[MAX];

int isDirectory(const char *path)
{
  struct stat path_stat;
  stat(path, &path_stat);
  return S_ISDIR(path_stat.st_mode);
}

void *moveFile(void *filename) 
{
  char *source = (char *)filename;
  char new_source[MAX];
  strcpy(new_source, source);

  char *target = strrchr(new_source, '/');
  char new_filename[MAX];

  if(!target)
    strcpy(new_filename, new_source);
  else
    strcpy(new_filename, target + 1);

  const char *dot = strrchr(new_filename, '.');
  struct stat st = {0};
  char folder[MAX];
  if(!dot || dot == new_source) 
    strcpy(folder, "Unknown");
  else
  {
    strcpy(folder, dot + 1);
    int i = 0;
    while(folder[i]!='\0')
    {
      if(folder[i]>='A' && folder[i]<='Z')
        folder[i] += 32;
      i++;
    }
  }

  int ch;
  FILE *fp1, *fp2;

  if(target)
    target = target + 1;
  else
    target = new_source;

  char new_target[MAX];
  sprintf(new_target, "%s/%s/%s", cwd, folder, target);

  fp1 = fopen(new_source, "r");

  if (!fp1) 
  {
    printf("Unable to open %s!\n", new_source);
    pthread_exit(NULL); 
    return NULL;
  }

  if (stat(folder, &st) == -1)
    mkdir(folder, 0700);

  fp2 = fopen(new_target, "w");

  while ((ch = fgetc(fp1)) != EOF) 
    fputc(ch, fp2);

  fclose(fp1);
  fclose(fp2);

  remove(new_source);
  pthread_exit(NULL); 

}

int main(int argc, char **argv) 
{
  if (argc < 2)
  {
    puts("argument is not valid");
    return 0;
  }

  getcwd(cwd, sizeof(cwd));

  if (strcmp(argv[1], "-f") == 0)
  {
    pthread_t threads[argc - 2]; 
    for (int i = 2; i < argc; i++)
    {
      if(!isDirectory(argv[i]))
        pthread_create(&threads[i-2], NULL, moveFile, argv[i]);
      else
        printf("%s is a directory\n", argv[i]);
    }

    for (int i = 0; i < argc - 2; i++)
      pthread_join(threads[i], NULL);
  }
  else if (strcmp(argv[1], "-d") == 0 && argc == 3)
  {
    int file_count = 0;
    char files[MAX][MAX];

    DIR *d;
    struct dirent *dir;
    chdir(argv[2]);
    d = opendir(".");

    if (d)
    {
      while ((dir = readdir(d)) != NULL)
      {
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
          continue;
          
        if(!isDirectory(dir->d_name))
        {
          char tmp[100];
          strcpy(tmp, dir->d_name);
          sprintf(files[file_count], "%s/%s", argv[2], dir->d_name);
          file_count++;
        }
        else
        {
          printf("%s is a directory\n", dir->d_name);
        }
      }
      closedir(d);
    }

    chdir(cwd);
    pthread_t threads[file_count]; 
    for (int i = 0; i < file_count; i++)
      pthread_create(&threads[i], NULL, moveFile, files[i]);

    for (int i = 0; i < file_count; i++)
      pthread_join(threads[i], NULL);
  }
  else if (strcmp(argv[1], "*") == 0 && argc == 2)
  {
    int file_count = 0;
    char files[MAX][MAX];

    DIR *d;
    struct dirent *dir;
    d = opendir(".");

    if (d)
    {
      while ((dir = readdir(d)) != NULL)
      {
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
          continue;
          
        if(!isDirectory(dir->d_name))
        {
          char tmp[100];
          strcpy(tmp, dir->d_name);
          sprintf(files[file_count], "%s", dir->d_name);
          file_count++;
        }
        else
        {
          printf("%s is a directory\n", dir->d_name);
        }
      }
      closedir(d);

    }

    pthread_t threads[file_count]; 
    for (int i = 0; i < file_count; i++)
      pthread_create(&threads[i], NULL, moveFile, files[i]);

    for (int i = 0; i < file_count; i++)
      pthread_join(threads[i], NULL);
  }
  else
    puts("argument is not valid");
    
  return 0;
}
