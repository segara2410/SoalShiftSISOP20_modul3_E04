#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define PORT 8080

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

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    FILE *fp;
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

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    int value = 0;
    while(1)
    {
      char buffer[1024] = {0};
      char userpass[1024] = {0};

      memset(buffer, 0, 99); 
      valread = read(new_socket, buffer, 1024);
      
      char value_now[1024];

      int state_login = -1;

      if(strcmp(buffer, "login") == 0)
      {
        send(new_socket, buffer, strlen(buffer), 0);
        read(new_socket, buffer, 1024);
        
        char login_message[100];
        if(accountExist(buffer))
          strcpy(login_message, "Auth success\n");
        else
          strcpy(login_message, "Auth failed\n");

        send(new_socket, login_message, strlen(login_message), 0); 
        memset(buffer, 0, 99); 
        state_login = 1;
      }
      else if(strcmp(buffer, "register") == 0)
      {
        fp = fopen ("akun.txt", "a");
        
        send(new_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, 99); 
        
        read(new_socket, buffer, 1024);
        fputs(buffer, fp);
        fclose(fp);

        char register_success[100] = "register success\n";
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
      else
      {
        char nothing[1] = "";
        send(new_socket, nothing, sizeof(nothing), 0);        
      }
    }
    return 0;
}