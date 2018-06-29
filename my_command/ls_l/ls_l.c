#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <sys/types.h>
#include <grp.h>

int main(int argc,char *argv[]){
DIR	*dir;
struct 	dirent *dp;
char	path[512],name[1024];
struct stat st;
	if(argc<=1){
	strcpy(path,".");
	}
	else{
	strcpy(path,argv[1]);
	}

	if((dir=opendir(path))==NULL){
		perror("opendir");
	return 0;
	}

	for(dp=readdir(dir);dp!=NULL;dp=readdir(dir)){
		if(dp->d_name[0]!='.'){
		
		if(DT_DIR==dp->d_type) printf("d");
		else if(DT_LNK==dp->d_type) printf("l");
		else printf("-");
		//file type
		strcpy(name,path);
		strcat(name,"/");
		strcat(name,dp->d_name);
		stat(name,&st);
		printf("%d ", st.st_mode);// permission
		printf("%lu ", (unsigned long) st.st_nlink); // the number of hard link

		struct passwd *pass = getpwuid(st.st_uid);
		printf("%s ", pass->pw_name); // user id
		struct group *gr = getgrgid(st.st_gid);
		printf("%s ", gr->gr_name); // grope id
		printf("%lld ", (unsigned long long)st.st_size); // file size
	   	struct tm *t_st;
   		/* tlansrate to natural notation */
   		t_st = localtime(&((st.st_ctim).tv_sec));
 		printf("%d月",t_st->tm_mon+1);
  		printf("  %d ",t_st->tm_mday);
  		printf("%d:",t_st->tm_hour);
   		printf("%d ",t_st->tm_min); //time stamp
		printf("%s\n",dp->d_name);
		}
	}
	closedir(dir);
	return 0;
}
