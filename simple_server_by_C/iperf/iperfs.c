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


int main(int argc,char *argv[])
{
  int server_sock , client_sock ;
  socklen_t server_len , client_len ;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  struct timeval t1, t2;


  int port =53;
  if(argv[1]==NULL){printf("no port.\n"); return 0;}
  port=atoi(argv[1]);
  
  printf("port:%d\n", port);
  server_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  server_address.sin_family = AF_INET ;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);
  
  
  server_len = sizeof(server_address);
  bind(server_sock , (struct sockaddr *)&server_address , server_len);
  listen(server_sock , 5);
  //  int i=0;
  int *re;
  re = malloc(sizeof(int));
  
  int che=0;
  client_sock = accept(server_sock , (struct sockaddr *)&client_address , &client_len);
  printf("accept\n");
  char buf[2048];
  int ww=0;
    memset(buf, 1, sizeof(buf));
    gettimeofday(&t2,NULL);
    while(ww!=2048*600000){
      ww+=write(client_sock, buf , 2048);
    }

    //    printf("aut\n");
    while(1){
    che = read(client_sock, re, 4);
    if(che!=0){
      gettimeofday(&t1,NULL); 
      break;
    }
    }

    printf("sended\n");
    double ans1, ans2;
    ans1 = (t1.tv_sec-t2.tv_sec)+ (t1.tv_usec-t2.tv_usec) /1000.0 /1000.0;
    ans2 = *re;
    ans2 = ans2 * 8.0 / ans1 / 1000000.0;
    printf("%d/%lf/%lf\n",(*re/2048),ans1,ans2);
    return 0;
}
    



