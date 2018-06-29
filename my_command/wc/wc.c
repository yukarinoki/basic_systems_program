#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(){
  char buf[1000];
  int fd,n,w=0,l=0,b=0;
  char path[100];
  scanf("%s", path);
  fd = open(path, O_RDONLY);

  while(0!=(n=read(fd,buf,1000))){
    int i;
    b+=n;
    for(i=0;i<n;i++){
      if(buf[i]==' ')w++;
      else if(buf[i]=='\n'){l++;if(i>0&&(buf[i-1]!='\n'&&buf[i-1]!=' ')) w++;}
    }
  }
  printf("word: %d ,line: %d, byte: %d\n",w,l,b);

  


  return 0;
}

/*このプログラムは確かにうごく　実際
tansei@ispc2016:~/os2017/report2$ ./a 
eng.txt
word: 608 ,line: 15, byte: 3665
tansei@ispc2016:~/os2017/report2$ wc eng.txt
  15  608 3665 eng.txt

*/
