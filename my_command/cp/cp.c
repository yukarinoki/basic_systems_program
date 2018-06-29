#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(){
  char path[100] ="";
  char path1[100] = "";
  printf("from?: ");
  scanf("%s", path);
  printf("to?: ");
  scanf("%s", path1);
  struct stat st,st1;
  stat(path,&st);
  stat(path1,&st1);
  if(st1.st_ino==st.st_ino){printf("cp: '%s' と `%s' は同じファイルです\n", path ,path1);}
  int fd = open(path, O_RDONLY);
  if(fd<0){perror("open error: "); return 0;}
  int fd1= open(path1,O_CREAT|O_RDWR|O_TRUNC,st.st_mode);
  if(fd == -1){perror("file copy error: "); return 0;}
  
  ssize_t rd=-1;
  char buf[1000];
  int wt=0; 
  while((rd=read(fd,buf,1000))!=0){
  if(rd<0){perror("error :"); break;}
	char *t=buf; 
  while(rd!=0){
   wt = write(fd1,t,rd);
	if(wt<0){perror("write error: "); return 0;}
	rd-=wt;
	t+=wt;
  }
 }
  close(fd);
  close(fd1);
  return 0;
}

/*
実行結果
基本部分とのことなので、コマンドライン引数はやってない。x

tansei@ispc2016:~/os2017/report2$ head -c 1000 /dev/urandom > test.tx
tansei@ispc2016:~/os2017/report2$ ./a
from?: test.txt
to?: to.txt
tansei@ispc2016:~/os2017/report2$ diff test.txt to.txt

あとpermissionを同じになるようにした。

tansei@ispc2016:~/os2017/report2$ stat test.txt
File: `test.txt'
Size: 1000      Blocks: 8          IO Block: 4096   通常ファイル
Device: 805h/2053dInode: 1709389     Links: 1
Access: (0664/-rw-rw-r--)  Uid: ( 1000/  tansei)   Gid: ( 1000/  tansei)
Access: 2017-04-20 11:42:15.892072669 +0900
Modify: 2017-04-20 11:42:14.768072677 +0900
Change: 2017-04-20 11:42:14.768072677 +0900
 Birth: -
tansei@ispc2016:~/os2017/report2$ stat to.txt
  File: `to.txt'
  Size: 1000      Blocks: 8          IO Block: 4096   通常ファイル
  Device: 805h/2053dInode: 1709391     Links: 1
Access: (0664/-rw-rw-r--)  Uid: ( 1000/  tansei)   Gid: ( 1000/  tansei)
Access: 2017-04-20 11:42:31.196072556 +0900
Modify: 2017-04-20 11:42:30.072072565 +0900
Change: 2017-04-20 11:42:30.072072565 +0900
 Birth: -

コピー先とコピー元が同じときの動作をおいたが、完璧ではない。
*/
