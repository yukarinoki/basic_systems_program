#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>
#include<string.h>    
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include<pthread.h> 
#include<signal.h>
#include<stdlib.h>
#include<time.h>
 

void *server_handler(void *);

struct handler_arg{
  int sock;
  char *root;
}; 
int main(int argc , char *argv[])
{
  int socket_desc , client_sock , len;
  struct sockaddr_in server , client;
     
  //socket
  socket_desc = socket(PF_INET , SOCK_STREAM , IPPROTO_TCP);
  if (socket_desc == -1)
    {
      printf("socket_create_fail\n");
      return 0;
    }
  puts("Socket created");
 
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( atoi(argv[1]) );
  printf("port:%d\n", atoi(argv[1]));
     
 
  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
    perror("bind failed");
    return 1;
  }
  puts("binded\n");
     
  listen(socket_desc , 5);
     
  puts("listening...");
  len = sizeof(struct sockaddr_in);
    
 
  int i=0;
  while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&len)) ){
    puts("Connection accepted");

    
    pthread_t thread;  
    struct handler_arg a;
    a.sock = client_sock;
    a.root = argv[2];
    if( pthread_create( &thread , NULL ,  server_handler , (void*) &a) < 0){
      perror("thhead creat failiur");
      return 1;
    }
    puts("go handler");
    printf("conection_number:%d\n", i);
    i++;    
  }
  
  if (client_sock < 0){
    perror("accept failed");
    return 1;
  }
  
  return 0;
}

void *server_handler(void *argument){
  struct handler_arg *a = (struct handler_arg *) argument;
  int sock = a->sock;
  char *root = a->root;

  int read_size;
  char client_message[2000];
    
  
  while((read_size = recv(sock , client_message , 2000 , 0)) > 0){
    printf("MESSAGE:%s\n", client_message);
    char path[100]="",*saveptr, *token, buf[2000]="",head[1000]="",*request_file=NULL, *ver=NULL, date[30];
    int co_c=0; // Conection: Close なら1;
    if(NULL!=strstr(client_message, "Connection: Close")){co_c=1;} 
    int http_ver=0; //0なら1.0,1なら1.1  
    //切り出し
    token = strtok_r(client_message," \n",&saveptr);
    printf("method:%s\n", token);
    request_file=strtok_r(request_file," \n",&saveptr);
    ver = strtok_r(ver, " \n", &saveptr);
    if(0==strncmp(ver,"HTTP/1.1",8)){
      http_ver = 1;
    }
      
    printf("request_file:%s\nhttp_version:%s\n", request_file,ver);
    FILE *fp;
    fpos_t fsize=0;
    
   
    char *home_env;
    home_env = getenv("HOME");
    strcat(path, home_env);
    strcat(path,"/");
    strcat(path, root);
    //strcat(path,"/");
    strcat(path,request_file);
    printf("path:%s\n", path);
    if((fp = fopen(path,"rb")) == NULL){
      printf("not exist request file..\n");
      char notf[200] ="";
      if(http_ver==0){strcat(notf,"http/1.0 404 Not found\nContent-Length: 13\nServer: kusoserver\n\n404 Not found");
      } else{strcat(notf,"http/1.1 404 Not found\nContent-Length: 13\nServer: kusoserver\n\n404 Not found"); }     
      write(sock , notf, strlen(notf));
      printf("return\n");
    }else{// ファイルが存在する。
      
      
      fseek(fp,0L,SEEK_END);
      fgetpos(fp, &fsize); //get filesize
      char file_size[30];
      sprintf(file_size, "%d", (int)fsize);
      time_t now = time(0);
      struct tm tm = *gmtime(&now);
      strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S %Z", &tm);
      fclose(fp);
    
      // strcat(head,ver);
      if(http_ver==0){strcat(head,"HTTP/1.0 ");}
      else{strcat(head,"HTTP/1.1 ");}
      strcat(head,"200 OK\n");
      strcat(head,"Date: ");
      strcat(head,date);
      strcat(head,"\n");
      strcat(head,"Server: mytcpserver_in_c\n");
      strcat(head,"Content-Length: ");
      strcat(head,file_size);
      strcat(head,"\n");
      strcat(head,"Content-Type: text/html");
      if(co_c) strcat(head, "Connection: Close");
      strcat(head, "\n\n");
      //head完成

      printf("HEAD:\n%s" ,head);

      if(0==strcmp("HEAD", token)){
	write(sock , head , strlen(head));
      }else if(0==strcmp("GET", token)){
	printf("150\n");
	write(sock , head , strlen(head));
	int file_des;
	file_des = open(path, O_RDONLY);
	printf("154\n");
	int reads;
	//      while((
	reads=read(file_des, buf, sizeof(buf)-1) ;
	//> 0){
	printf("%d", reads);
	buf[reads]='\0';
	write(sock , buf, strlen(buf));
	memset(buf, 0, 2000);
	// }//ファイルの内容を書く          
	close(file_des);
	
      }else{
	char notq[200]="";
	if(http_ver==0){strcat(notq,"http/1.0 501 Not Implemented\nContent-Length: 20\n\n501 Not ImpleMented");}
	else{strcat(notq,"http/1.1 501 Not Implemented\nContent-Length: 20\n\n501 Not ImpleMented");}
	write(sock ,notq , strlen(notq));
      }
    }
    
    memset(client_message,0,2000);
    if(http_ver==0||co_c) break;
  } 
  
  if(read_size == 0){
    puts("Client disconnected");
    fflush(stdout);
  }else if(read_size == -1){
    perror("recv failed");
  }
  close(sock);  
  return 0; 
}
