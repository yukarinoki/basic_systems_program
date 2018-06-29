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

void *recv_h(void*);
void *send_h(void*);

int main(int argc,char *argv[])
{
  int server_sock , client_sock, client_sock1;
  socklen_t server_len , client_len, client_len1;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;

  int port =53;
  
  port=atoi(argv[1]);
  printf("port:%d\n", port);
  server_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  server_address.sin_family = AF_INET ;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);
  
  //here to
  port = atoi(argv[3]);
  char *host = argv[2];
  struct sockaddr_in client_address1;
  client_sock1 = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  client_address1.sin_family= AF_INET;
  inet_aton(host, &(client_address1.sin_addr));
  client_address1.sin_port = htons(port);

  client_len1 = sizeof(client_address1);
  //here client_sock1
  int result;

  server_len = sizeof(server_address);
  bind(server_sock , (struct sockaddr *)&server_address , server_len);
  listen(server_sock , 1);
  pthread_t thread_id[2];
  
  
  result = connect(client_sock1,(struct sockaddr *)&client_address1, client_len1);
  if(result == -1){perror("繋がらない\n"); return 0;}
  printf("s:connect\n");
 


  client_sock = (accept(server_sock , (struct sockaddr *)&client_address , &client_len));
  printf("accept\n");
  
  int *cc = malloc(sizeof(int));
  *cc = 1;
  sleep(1);
  if(read(client_sock1,cc,4)==0){
    printf("c:read:error\n");
    return 0;
  }
  printf("c:read:OK\n");
  if(write(client_sock,cc,4)<0){
    perror("c:write: ");
    return 0;
  }
  printf("s:write:OK\n");
  
  while(read(client_sock1,cc,4)==0){}
  
 
  if( pthread_create(&thread_id[0],NULL,recv_h,(void*) &client_sock1) < 0){
    perror("s:recv:スレッド作れませんでした\n");
    return 0;
  }
  

  if( pthread_create(&thread_id[1],NULL,send_h,(void*) &client_sock) < 0){
    perror("s:send:スレッド作れませんでした\n");
    return 0;
  }
  pthread_join(thread_id[0],NULL);
  pthread_join(thread_id[1],NULL);


  return 0;
}




void *recv_h(void *socket_desc)
{
  struct timeval t1, t2;
  int sock = *(int*)socket_desc;
  int read_size=0;
  char buf[2048];
  gettimeofday(&t2,NULL);
  while(read_size<300000*2048){
      read_size+=read(sock, buf, 2048);
      //   printf("%d\n", read_size);
    }
  gettimeofday(&t1,NULL); 
  printf("s:all recieve\n");
  write(sock, &read_size, 4);
  double ans1, ans2;
  ans1 = (t1.tv_sec-t2.tv_sec)+ (t1.tv_usec-t2.tv_usec) /1000.0 /1000.0;
  ans2 = 300000*2048;
  ans2 = ans2 * 8.0 / ans1 / 1000000.0;
  printf("client_recv:%d/%lf/%lf\n",300000,ans1,ans2);
  close(sock);

  return 0;
} 
void *send_h(void *socket_desc)
{
  struct timeval t1, t2;
  int sock = *(int*)socket_desc;
  int write_size=0,che=0;
  char buf[2048];
  gettimeofday(&t2,NULL);
  while(write_size<300000*2048){
    write_size+=write(sock, buf, 2048);
  }
  gettimeofday(&t1,NULL);
  while(1){
  che=read(sock, &write_size, 4);
  if(che!=0){
    gettimeofday(&t1,NULL);
    break;
  }
  }
  printf("s:all send\n");
  double ans1, ans2;
  ans1 = (t1.tv_sec-t2.tv_sec)+ (t1.tv_usec-t2.tv_usec) /1000.0 /1000.0;
  ans2 = 300000*2048;
  ans2 = ans2 * 8.0 / ans1 / 1000000.0;
  printf("client_send:%d/%lf/%lf\n",300000,ans1,ans2);

  sleep(2);
  close(sock);

  return 0;
}

 
