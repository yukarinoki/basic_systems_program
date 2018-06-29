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
#include <sys/time.h>

int main(int argc, char *argv[])
{
  int client_sock ;
  int client_len ;
  struct timeval t1, t2;
  int port =atoi(argv[2]);
  int b=0;
  char *host=argv[1];
  struct sockaddr_in client_address;
  
  
  printf("port:%d host:%s\n", port, host);
  client_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  client_address.sin_family = AF_INET ;
  inet_aton(host, &(client_address.sin_addr));
  client_address.sin_port = htons(port);
  
  client_len = sizeof(client_address);
  int result;
  result = connect(client_sock, (struct sockaddr *)&client_address , client_len);
  
  if(result==-1){perror("繋がらない\n"); return 0;}
  printf("connect\n");
  gettimeofday(&t2,NULL);
  char buf[2048]="";
 
  while(b<600000*2048){
    b+=read(client_sock, buf, 2048);
    
    //  printf("%d\n", b);
  }
  gettimeofday(&t1,NULL); 
 printf("all recived\n");
 write(client_sock, &b, 4);
 double ans1, ans2;
 ans1 = (t1.tv_sec-t2.tv_sec)+ (t1.tv_usec-t2.tv_usec) /1000.0 /1000.0;
 ans2 = 600000*2048;
 ans2 = ans2 * 8.0 / ans1 / 1000000.0;
 printf("%d/%lf/%lf\n",600000,ans1,ans2);
 close(client_sock);

return 0;
}
