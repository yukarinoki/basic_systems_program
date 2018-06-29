#include <sys/types.h>
#include <netinet/ip.h> 
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
  int server_sock ;
  int port =53;
  struct sockaddr_in server_address, from;
  if(argv[1]==NULL){printf("not recive port number.\n"); return 0;}
  port=atoi(argv[1]);
  printf("port:%d\n", port);
  server_sock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
  server_address.sin_family = AF_INET ;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);

  socklen_t server_len = sizeof(server_address);
  socklen_t from_len = sizeof(from);
  bind(server_sock , (struct sockaddr *)&server_address , server_len);

    char buf[2048]="";
    int read_len=0;
    
    while(1){
    read_len = recvfrom(server_sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, &from_len);
    printf("receive\n");
    printf("recv: %s\n", buf);
    if(sendto(server_sock, buf, read_len, 0,(struct sockaddr *)&from, from_len) < 0) perror("sendto: ");
    printf("send\n");
    //    memset(buf,0,sizeof(buf));
}  

  close(server_sock);  
}
