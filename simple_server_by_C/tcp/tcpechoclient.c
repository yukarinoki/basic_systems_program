#define _REENTRANT

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>


int main(int argc, char *argv[])
{
  int client_sock ;
  int client_len ;
  int port =53;
  char host[20]="";
  struct sockaddr_in client_address;
 
  strcpy(host, argv[1]);
  port=atoi(argv[2]);

  printf("port:%d host:%s\n", port, host);
  client_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  client_address.sin_family = AF_INET ;
  inet_aton(host, &(client_address.sin_addr));
  client_address.sin_port = htons(port);

  client_len = sizeof(client_address);
int result;
  result = connect(client_sock, (struct sockaddr *)&client_address , client_len);
  if(result==-1){perror("繋がらない\n"); return 0;}

char buf[2048]="";
char in[2048]="";

while(scanf("%s", in)!=EOF){
write(client_sock, in, strlen(in)+1);
read(client_sock, buf, 2048);
printf("%s\n", buf);
}  

  close(client_sock);


return 0;
}
