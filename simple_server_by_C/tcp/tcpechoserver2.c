#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


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
  fd_set rd_fds, wr_fds, ex_fds;
  FD_ZERO(&rd_fds); FD_ZERO(&wr_fds); FD_ZERO(&ex_fds);
  FD_SET(server_sock, &rd_fds); 
    int i=0;
    int read_size;
char buf[2048];
struct timeval out;

while(1){

fd_set rd_fds, wr_fds, ex_fds;
FD_ZERO(&rd_fds); FD_ZERO(&wr_fds); FD_ZERO (&ex_fds);
FD_SET(server_sock, &rd_fds); 
int j=0;
for(j=0;j<i;j++){
FD_SET(client_sock[j], &rd_fds);
}
out.tv_sec =1;
out.tv_usec =0;

select(102,&rd_fds, &wr_fds, &ex_fds, &out);
printf("timeout\n");

if(FD_ISSET(server_sock, &rd_fds)){
client_sock[i] = accept(server_sock, (struct sockaddr *)&client_address, &client_len);
i++;
printf("accept:%d\n", i);
}

for(j=0;j<i;j++){
	  if(FD_ISSET(client_sock[j], &rd_fds)){
	    read_size=read(client_sock[j], buf, 2048);  
	    write(client_sock[j], buf , read_size);
memset(buf,0,sizeof(buf));
	  }    
	}
      }
    }
 
