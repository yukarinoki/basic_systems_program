#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>


int main(int argc, char *argv[])
{
  int client_sock ;
  socklen_t client_len ;
  int port =53;
  char host[20]="";
  struct sockaddr_in client_address ;
  if(argv[1]==NULL||argv[2]==NULL){printf("not recieve host or port.\n"); return 0;}
  strcpy(host, argv[1]);
  port=atoi(argv[2]);

  printf("port:%d host:%s\n", port, host);
  client_sock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
  client_address.sin_family = AF_INET ;
  inet_aton(host, &(client_address.sin_addr));
  client_address.sin_port = htons(port);

  client_len = sizeof(client_address);

    char buf[2048]="";
    char in[2048]="";
    /*
    fd_set readok;
    FD_ZERO(&readok);
    FD_SET(client_sock
    */
    while(scanf("%s", in)!=EOF){
    sendto(client_sock, in, sizeof(in),0 ,(struct sockaddr *)&client_address, client_len);
    recvfrom(client_sock, buf, sizeof(buf), 0, (struct sockaddr *)&client_address, &client_len);
    printf("recv: %s\n", buf);
    }  

  close(client_sock);
  
  return 0;
}

