#include "parse.h"
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

enum job_gr{
  BG,
  FG,
};

typedef struct {
  char* inst;
  pid_t gpid;
  int np;
  int nf; // number of finish process.
  pid_t* a_pid;
  enum job_gr gr;
}job_dat;

typedef job_dat** job_control; //the number of member is 100.

typedef struct { 
  int *pr;
  int *jb;
  int np;
}all_proc; //the number of member is 3000.


job_control job_control_init(){
  job_control t = (job_dat **) malloc(sizeof(job_dat *)*100);
  int i=0;
  for(i=0;i<100;i++){
    t[i]=NULL;
  }
  return t;
}

all_proc* all_proc_init(){
  all_proc* p = malloc(sizeof(all_proc));
  p->pr = (pid_t *)malloc(3000*sizeof(pid_t));
  p->jb = (int *)malloc(3000*sizeof(int));
  int i=0;
  for(i=0;i<3000;i++){
    (p->pr)[i] = -1;
    (p->jb)[i] = -1;
  }
  p->np = 0;
  return p;
}

job_dat* job_dat_init(){
  job_dat* t = (job_dat *)malloc(sizeof(job_dat));
  t->inst = NULL;
  t->gpid=0;
  t->np = 0;
  t->np = 0;
  (t->a_pid) = (pid_t *) malloc(30*sizeof(pid_t));
  t->gr = FG;
  return t;
}
job_dat* job_set_inst(job_dat *t, char *s){
  t->inst = (char *)malloc(strlen(s)+1);
  strcpy(t->inst, s);
  return t;
}
all_proc* job_add_proc(all_proc *pr,int jb_num, pid_t p){
  (pr->pr)[(pr->np)] = p;
  (pr->jb)[(pr->np)++] = jb_num;
  return pr;
}
all_proc* job_del_proc(all_proc *pr, pid_t p){
  int i,n=pr->np;
  for(i=0;i<n;i++){
    if((pr->pr)[i] == p) break;
  }
  (pr->pr)[i] = -1;
  (pr->jb)[i] = -1;
  if(i==(pr->np)) (pr->np)--;
  return pr;
}
job_dat* job_set_gpid(job_dat *t, pid_t g){
  t->gpid=g;
  return t;
}
job_dat* job_set_gr(job_dat *t,enum job_gr gr){
  t->gr = gr;
  return t;
}

job_dat* job_set_all(job_dat* t, char* s, pid_t g,enum job_gr gr){
  return job_set_gr(job_set_gpid(job_set_inst(t, s) , g) , gr);
} 

void job_dat_destroy(job_dat *t){
  if(t->inst != NULL) free(t->inst);
  if(t->a_pid != NULL)free(t->a_pid);
  if(t!=NULL) free(t);
}

void job_wait(job_dat *t){
  int n = (t->np);
  int i=0;
  for(i=0;i<n;i++){
    if(waitpid((t->a_pid)[i],NULL,0)==-1&&errno==EINTR){
      i--;
    }
  }
  job_dat_destroy(t);
} 

int find_pro_job(pid_t p,all_proc *pr){
  int i,n=pr->np;
  for(i=0;i<n;i++){
    if((pr->pr)[i] == p) break;
  } 
  return (pr->jb)[i];
}


int next_job_num(job_control t){
  int i=0;
  int n=-1;
  for(i=0;i<100;i++){
    if(t[i] == NULL){n=i; break;}
  }
  return n;
}
