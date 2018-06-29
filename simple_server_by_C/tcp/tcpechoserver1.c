#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void *tcp_handler(void*);

int main(int argc,char *argv[])
{
  int server_sock , client_sock[100] ;
  socklen_t server_len , client_len ;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;

  int port =53;
  
  port=atoi(argv[1]);
  printf("port:%d\n", port);
  server_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  server_address.sin_family = AF_INET ;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);
  
  
  server_len = sizeof(server_address);
  bind(server_sock , (struct sockaddr *)&server_address , server_len);
  listen(server_sock , 5);
  pthread_t thread_id;
  int i=0;
  while(0!=(client_sock[i] = (accept(server_sock , (struct sockaddr *)&client_address , &client_len))))
    { 
      printf("accept:%d\n", i);
      if( pthread_create(&thread_id,NULL,tcp_handler,(void*) &client_sock[i]) < 0)
        {
	  perror("スレッド作れませんでした\n");
	  return 0;
        }
      if(100>=i) i++;
      else if(100==i){printf("クライアントの数の限界です\n"); return 0;}
    }
  return 0;
}




void *tcp_handler(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    int read_size;
    char buf[2048];
      
    while(1)
    {
      read_size=read(sock, buf, 2048);  
      if(read_size==0) break;
      write(sock , buf , read_size);
      memset(buf, 0, 2048);
    }
         
    return 0;
} 
