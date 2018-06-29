#include "parse.c"
#include "job_control.h"
#include "path_env.h"
#include <string.h>
#include "parse.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>



static volatile int sigintr_flag;

void sigchld_handler(int sig,siginfo_t *siginfo, void *context){
  //  printf("L18\n");
  if(siginfo->si_code == (SI_USER|CLD_STOPPED)){
    sigintr_flag = 1;
    //printf("L20\n");
  }else{sigintr_flag=0;}
  
}

int main(int argc, char* argv[], char *envp[]){

  pid_t pid,wpid,pid1,pid2,piped_pgid, pid_mp[512];
  char s[LINELEN];
  job *curr_job;

  int fg_flag=0, fg_jobnum=0;

  job_control jobc = job_control_init();
  all_proc *alpr = all_proc_init();

  struct sigaction sig_ish;
  sig_ish.sa_flags = 0;
  sig_ish.sa_handler = SIG_IGN;
  sigemptyset(&sig_ish.sa_mask);
  sigaction(SIGINT, &sig_ish, NULL); // ish is made to ignore SIGINT
  sigaction(SIGTTOU, &sig_ish, NULL);
  sigaction(SIGTSTP, &sig_ish,NULL);
  
  while(fg_flag==1||get_line(s, LINELEN)){
    //main loop

    while(1&&fg_flag!=1){
      if((wpid=waitpid(-1,NULL,WNOHANG))==0) break;
      if(wpid==-1&&errno!=EINTR) break;	
      if(wpid>0){
	int ind = find_pro_job(wpid, alpr);
	(alpr->pr)[ind] = -1;
	(alpr->jb)[ind] = -1;
	if(((alpr->np)-1) == ind) (alpr->np)--;
	jobc[ind]->nf++;
	if(jobc[ind]->np==jobc[ind]->nf){
	  printf("[%d] is done           %s",ind+1,jobc[ind]->inst);
	  job_dat_destroy(jobc[ind]);
	  jobc[ind]=NULL;
	}
      }
    }// wait Back Ground job
    int special_cmd_flag = 0;    

    //fg is implemented in this section
    if(fg_flag==1&&special_cmd_flag==0){
      special_cmd_flag=1;
      fg_flag=0;

      int jn;
      pid_t tgp;
      jn=fg_jobnum;
      printf("fg_jobnum: %d\n", fg_jobnum);
      if(jn==0) printf("There is no back ground process.\n");
      if(jn<1||jobc[jn-1]==NULL){if (jn!=0) printf("There is no job which has this job number %d. \n", jn);}
      else {
	//here
	printf("L80");
	tgp = jobc[jn-1]->gpid;
	kill(-1*tgp,18);
 	
 	/* if(tcsetpgrp(0,getpid())==-1) perror("fore ground set L180 0 Error :"); */
	/* if(tcsetpgrp(1,getpid())==-1) perror("fore ground set L181 1 Error :"); */
	/* if(tcsetpgrp(2,getpid())==-1) perror("fore ground set L182 2 Error :"); */
	  
	
	/* while(1){ */
	/*   if((wpid=waitpid(-1,NULL,WNOHANG))==0) break; */
	/*   if(wpid==-1&&errno!=EINTR) break; */
	/*   if(wpid>0){ */
	/*     int ind = find_pro_job(wpid, alpr); */
	/*     (alpr->pr)[ind] = -1; */
	/*     (alpr->jb)[ind] = -1; */
	/*     if(((alpr->np)-1) == ind) (alpr->np)--; */
	/*     jobc[ind]->nf++; */
	/*     if(jobc[ind]->np==jobc[ind]->nf){ */
	/*       printf("[%d] is done           %s",ind+1,jobc[ind]->inst); */
	/*       job_dat_destroy(jobc[ind]); */
	/*       jobc[ind]=NULL; */
	/*     } */
	/*   } */
	/* }// wait Back Ground job    	 */
	
	//	if(jobc[jn-1]!=NULL){
	  
	  if(tcsetpgrp(0,tgp)==-1) perror("fore ground set L73 0 Error :");
	  if(tcsetpgrp(1,tgp)==-1) perror("fore ground set L74 1 Error :");
	  if(tcsetpgrp(2,tgp)==-1) perror("fore ground set L75 2 Error :");
	
	  sig_ish.sa_flags = SA_SIGINFO;
	  sig_ish.sa_handler = &sigchld_handler;
	  sigemptyset(&sig_ish.sa_mask);
	  sigaction(SIGCHLD, &sig_ish, NULL);
	  printf("L103:%d:", tgp);
	  int flaga=0;
	  int i;
	  sigintr_flag=0;
	  for(i=0;(i<jobc[jn-1]->np)&&flaga==0;i++){
	    int res;
	    
	    if((res=waitpid((jobc[jn-1]->a_pid)[i],NULL,0))==-1&&errno==EINTR&&sigintr_flag==1){flaga=1;sigintr_flag=0;break;}
	    else if(res==-1&&errno!=ECHILD){(jobc[jn-1]->nf)++; job_del_proc(alpr,(jobc[jn-1]->a_pid)[i]);}
	  }
	  if(flaga==0){
	    job_dat_destroy(jobc[jn-1]);
	    jobc[jn-1]=NULL;
	  }else{
	    sig_ish.sa_handler = SIG_DFL;
	    sigaction(SIGCHLD, &sig_ish, NULL);
	    jobc[jn-1]->gr=BG;
	    printf("[%d] is stopped.  %s", jn, jobc[jn-1]->inst);
	  }
	  //	job_wait(jobc[jn-1]);
	  //	jobc[jn-1]=NULL;
	  //	}
      }
    }
 
  

    if(!strcmp(s, "exit\n")&&special_cmd_flag==0){
      int nn;
      while((nn=next_job_num(jobc))!=0){
	job_wait(jobc[nn-1]);
	jobc[nn-1]=NULL;
      }
      free(alpr);
      free(jobc);
      break;
    }

 
    if(!strcmp(s, "debug\n")&&special_cmd_flag==0){
      special_cmd_flag = 1;
      printf("%d", jobc[0]==NULL?1:0);
    }
        

    if(!strcmp(s, "DEBUG\n")&&special_cmd_flag==0){
      special_cmd_flag=1;
      int jj;
      scanf("%d", &jj);
      printf("np:%d,nf:%d, \n", jobc[jj-1]->np,jobc[jj-1]->nf);
    }

   
    if(!strcmp(s, "jobs\n")&&special_cmd_flag==0){

      int j=0;
      printf("jobs:\n");
      for(j=0;j<100;j++){
	special_cmd_flag=1;
	if(jobc[j]!=NULL){
	  if(jobc[j]->gr == BG) printf("[%d] Background :%s", j+1, jobc[j]->inst);
	  if(jobc[j]->gr== FG) printf("[%d] Foreground :%s", j+1, jobc[j]->inst);
	}
      }
    }
    //bg is implemented in this section. 
    if((s[0]=='b'||s[0]=='f')&&s[1]=='g'&&special_cmd_flag==0){
      special_cmd_flag=1;
      int jn;      
      jn=next_job_num(jobc);
      if(atoi(s+2)!=0) jn = atoi(s+2);
      if(jn==0)  printf("There is no back ground process.\n"); 
      else if(jn<1||jobc[jn-1]==NULL){if(jn!=0) printf("There is no job which has this job number %d. \n", jn);}
      else { 
	if(s[0]=='f'){
	  fg_flag = 1;
	  fg_jobnum = jn;
	  printf("fg:%d", fg_jobnum);
	}
	kill(-(jobc[jn-1]->gpid),18);
      }
    }
    
    
    process *pr;
    int sta;
    int fd[2];
    int in_fd1,in_fd2,fdd,fds=0;
    
    curr_job = parse_line(s);
    
    if(curr_job!=NULL&&special_cmd_flag==0){
      //If input is only new line or control character, do nothing. 
      int jbn = next_job_num(jobc);
      printf("L196:");
      jobc[jbn] = job_dat_init();
      printf("L198:");
      if(curr_job->mode==BACKGROUND) jobc[jbn]->gr = BG; // set job_mode
      
      pr = curr_job->process_list;
      fd[0]=0;fd[1]=0;
      
      if(pr->next!=NULL){
	/*Execution use pipe*/

	int num_proc = 0;

	if(pipe(fd)==-1){
	  perror(NULL);
	  break;
	}	
	if((pid1 = fork() ) == 0){
	  /*Execution of first piped process */
	
	  setpgrp(); //set pgid.
	
	  sig_ish.sa_flags = 0;
	  sig_ish.sa_handler = SIG_DFL;
	  sigemptyset(&sig_ish.sa_mask);
	  sigaction(SIGINT, &sig_ish, NULL); // ish job is made to recognize SIGINT.
	  sigaction(SIGTTOU, &sig_ish, NULL);
	  sigaction(SIGTSTP, &sig_ish,NULL);
	  
	  dup2(fd[1], 1);
	  close(fd[0]);
	  close(fd[1]);
	  
	  if(pr->input_redirection!=NULL){
	    in_fd1 = open(pr->input_redirection,O_RDONLY);
	    close(0);
	    dup2(in_fd1,0);
	    close(in_fd1);
	  }

	  if(-1==execve(make_path(pr->program_name), pr->argument_list,envp)){
	    perror(NULL);
	    exit(-1);
	  }
	}else{
	  /*parent process*/
	  piped_pgid=pid1;
	  jobc[jbn]->gpid = piped_pgid;
	  if(curr_job->mode!=BACKGROUND){
	    if(tcsetpgrp(0,pid1)==-1) perror("fore ground set L73 0 Error :");
	    if(tcsetpgrp(1,pid1)==-1) perror("fore ground set L74 1 Error :");
	    if(tcsetpgrp(2,pid1)==-1) perror("fore ground set L75 2 Error :");
	  }
	  close(fd[1]);
	  /* if(waitpid(pid1,&sta,0) < 0){ */
	  /*   perror("waitpid L72 Error :"); */
	  /* } */
	  pr=pr->next;
	}
      
	  
	while(pr->next!=NULL){
	  num_proc++; //the number of piped processes.
	  fds=fd[0];
	  if(pipe(fd)==-1){
	    perror(NULL);
	    break;
	  }
	    
	  if((pid_mp[num_proc-1] = fork() ) == 0){
	    /*Execution of intermidiate pipe process*/
	    setpgid(0,piped_pgid); //set pgid.
	    
	    sig_ish.sa_flags = 0;
	    sig_ish.sa_handler = SIG_DFL;
	    sigemptyset(&sig_ish.sa_mask);
	    sigaction(SIGINT, &sig_ish, NULL); // ish job is made to recognize SIGINT.
	    sigaction(SIGTTOU, &sig_ish, NULL);
	    sigaction(SIGTSTP, &sig_ish,NULL);	    

	    //  printf("L91 : %d\n", piped_pgid);
	    if(fds!=0){dup2(fds,0); close(fds);}
	    dup2(fd[1], 1);
	    close(fd[0]);
	    close(fd[1]);
 
	    if(-1==execve(make_path(pr->program_name), pr->argument_list,envp)){
	      perror(NULL);
	      exit(-1);
	    }

	  }else{
	    /*parent process*/
	    close(fd[1]);
	    //  waitpid(pid1,&sta,0);
	    pr=pr->next;
	  }
	}

	if((pid2 = fork() ) == 0){
	  /*Execution of last piped process*/
	  // printf("L112 : %d\n", piped_pgid);
	  setpgid(0,piped_pgid); //set pgid.

	  sig_ish.sa_flags = 0;
	  sig_ish.sa_handler = SIG_DFL;
	  sigemptyset(&sig_ish.sa_mask);
	  sigaction(SIGINT, &sig_ish, NULL); // ish job is made to recognize SIGINT.
	  sigaction(SIGTTOU, &sig_ish, NULL);
	  sigaction(SIGTSTP, &sig_ish,NULL);

	  dup2(fd[0], 0);
	  close(fd[0]);  
	 
	  if(pr->output_redirection!=NULL){
	    fdd = open(pr->output_redirection, O_CREAT | O_WRONLY, 0666);
	    dup2(fdd, 1);
	    close(fdd);
	  }
	  if(-1==execve(make_path(pr->program_name), pr->argument_list,envp)){
	    perror(NULL);
	  }
	}else{
	  close(fd[0]);

	  jobc[jbn]->np=num_proc+2;
	  int i=0;
	  (jobc[jbn]->a_pid)[0]=pid1;
	  (jobc[jbn]->a_pid)[num_proc+1]=pid2;
	  job_add_proc(alpr,jbn,pid1);
	  for(i=0;i<num_proc;i++){
	    (jobc[jbn]->a_pid)[i+1] = pid_mp[i];
	    job_add_proc(alpr,jbn,pid_mp[i]);
	  }
	  job_add_proc(alpr,jbn,pid2);
	  job_set_inst(jobc[jbn],s);
	  
	  if(curr_job->mode!=BACKGROUND){
	    //if forground
	    jobc[jbn]->gr=FG;
	    
	    sig_ish.sa_flags = SA_SIGINFO;
	    sig_ish.sa_handler = &sigchld_handler;
	    sigemptyset(&sig_ish.sa_mask);
	    sigaction(SIGCHLD, &sig_ish, NULL);
	    int flaga=0;
	    if(waitpid(pid1,&sta,0)==-1&&sigintr_flag==1){flaga=1; sigintr_flag=0;}
	    else {(jobc[jbn]->nf)++;job_del_proc(alpr,pid1);}
	    int i;
	    for(i=0;i<num_proc&&flaga==0;i++){
	      if(waitpid(pid_mp[i],&sta,0)==-1&&sigintr_flag==1){flaga=1;sigintr_flag=0;break;}
	      else{(jobc[jbn]->nf)++; job_del_proc(alpr,pid_mp[i]);}
	    }
	    
	    if(flaga==1||(waitpid(pid2,&sta,0)==-1&&sigintr_flag==1)){flaga=1; sigintr_flag=0;}
	    else {(jobc[jbn]->nf)++;job_del_proc(alpr,pid2);}
	    
	    if(flaga==0){
	      job_dat_destroy(jobc[jbn]);
	      jobc[jbn]=NULL;
	    }else{
	      sig_ish.sa_handler = SIG_DFL;
	      sigaction(SIGCHLD, &sig_ish, NULL);
	      jobc[jbn]->gr=BG;
	      printf("[%d] is stopped.  %s", jbn+1, jobc[jbn]->inst);
	    }
	  }else{
	    //if background
	    jobc[jbn]->gr=FG;
	    printf("[%d]  %d\n", jbn+1, pid2);
	  }
	}
 
      }else if(pr->next==NULL){
	/*Execution doesn't use pipe*/
 	 
	if((pid = fork() ) == 0){
	  /*child process*/
	  setpgrp(); //set this prosess pid to pgid.

	  sig_ish.sa_flags = 0;
	  sig_ish.sa_handler = SIG_DFL;
	  sigemptyset(&sig_ish.sa_mask);
	  sigaction(SIGINT, &sig_ish, NULL); // ish job is made to recognize SIGINT.
	  sigaction(SIGTTOU, &sig_ish, NULL);	  
	  sigaction(SIGTSTP, &sig_ish,NULL);

	  if(pr->input_redirection!=NULL){
	    in_fd2 = open(pr->input_redirection,O_RDONLY);
	    close(0);
	    dup2(in_fd2,0);
	    close(in_fd2);
	  }
	  
	  if(pr->output_redirection!=NULL){
	    if(pr->output_option == TRUNC){
	      fdd = open(pr->output_redirection, O_CREAT | O_WRONLY, 0666);
	      dup2(fdd, 1);
	      close(fdd);
	    }
	    else if(pr->output_option == APPEND){
	      if((fdd = open(pr->output_redirection, O_APPEND|O_CREAT|O_WRONLY,S_IRWXU))<0){
		perror("output_redirection_append:");
	      }
	      dup2(fdd, 1);
	      close(fdd);
	    }
	  }
	  
	  execve(make_path(pr->program_name), pr->argument_list,envp);
	  perror("error :");
	  exit(-1);
	}else{
	  /*parent process*/
	  if(curr_job->mode!=BACKGROUND){
	    if(tcsetpgrp(0,pid)==-1) perror("fore ground set L173 0 Error :");
	    if(tcsetpgrp(1,pid)==-1) perror("fore ground set L174 1 Error :");
	    if(tcsetpgrp(2,pid)==-1) perror("fore ground set L175 2 Error :");
	  }
	 
	  job_add_proc(alpr,jbn,pid);
	  jobc[jbn]->np=1;
	  jobc[jbn]->gpid=pid;
	  (jobc[jbn]->a_pid)[0]=pid;
	  job_set_inst(jobc[jbn],s);

	  if(curr_job->mode!=BACKGROUND){
	    jobc[jbn]->gr = FG;
	    
	    sig_ish.sa_flags = SA_SIGINFO;
	    sig_ish.sa_handler = &sigchld_handler;
	    sigemptyset(&sig_ish.sa_mask);
	    sigaction(SIGCHLD, &sig_ish, NULL);
	    	    
	    if(waitpid(pid,&sta,0)!=-1||sigintr_flag!=1){
	      job_dat_destroy(jobc[jbn]);
	      jobc[jbn]=NULL;
	      job_del_proc(alpr,pid);
	    }else{
	      printf("[%d] is stopped.  %d:  %s", jbn+1, pid, jobc[jbn]->inst);
	      sigintr_flag=0;
	      jobc[jbn]->gr = BG;
	    }
	    sig_ish.sa_handler = SIG_DFL;
	    sigaction(SIGCHLD, &sig_ish, NULL);
	  }if(curr_job->mode==BACKGROUND){
	    jobc[jbn]->gr = BG;
	    printf("[%d]  %d\n", jbn+1, pid);
	  }
	}
      }    
    }
    
    if(tcsetpgrp(0,getpid())==-1) perror("fore ground set L180 0 Error :");
    if(tcsetpgrp(1,getpid())==-1) perror("fore ground set L181 1 Error :");
    if(tcsetpgrp(2,getpid())==-1) perror("fore ground set L182 2 Error :");
  } 
  
  return 0;
}
