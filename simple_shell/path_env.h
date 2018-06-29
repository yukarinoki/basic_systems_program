#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int file_exist(char *path){
  struct stat st;   
  if (stat(path, &st) == 0) return 1;
  else return 0;
}
  

char* make_path(char *cmd){
   if(cmd[0] == '/'){
     //Exection by absolute path
     // return cmd;
   }else{
     //Execution by command name
     char *pathvar = getenv("PATH");
     char *saveptr;
     char *sub_tok;
     char _path[512];
     sub_tok=strtok_r(pathvar, ":", &saveptr);
     strcpy(_path,sub_tok);
     strcat(_path,"/");
     strcat(_path,cmd);
     //     printf("%s\n", _path);
     if(file_exist(_path)) strcpy(cmd,_path);
     else{
       while((sub_tok = strtok_r(NULL, ":", &saveptr) )!= NULL){
	 strcpy(_path,sub_tok);
	 strcat(_path,"/");
	 strcat(_path,cmd);
	 //	 printf("%s\n", _path);
	 if(file_exist(_path)){strcpy(cmd, _path); break;}
       }
     }
   }
   return cmd;
}
