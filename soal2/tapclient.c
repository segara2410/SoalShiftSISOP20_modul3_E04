#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
  
int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int sock = 0, valread;
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
      char cmd[100];
      printf("1. Login\n2. Register\n   Choices : ");
      scanf("%s", cmd);
      send(sock, cmd , strlen(cmd), 0 );
  
      memset(buffer, 0, 99); 
      valread = read(sock, buffer, 1024);
      if (strcmp(buffer, "login") == 0 || strcmp(buffer, "register") == 0)
      {
        char userpass[1024] = {0};
        char username[1024] = {0};
        char password[1024] = {0};

        printf("   Username : ");
        scanf("%s", username);
        printf("   Password : ");
        scanf("%s", password);
        sprintf(userpass, "%s - %s\n", username, password);
        send(sock, userpass, strlen(userpass), 0 );
        
        read(sock, buffer, 1024);
        printf("   %s", buffer);
      }      
      else
        printf("   %s", buffer);
    }
}