#include <stdlib.h>
#include <stdio.h>

#include "execution.h"




void *myPipe(void *mas){
	int* p = (int*) mas;
	char* buf = malloc (sizeof *buf * 512);
	int len;
	while((len = read(p[0], buf, 511))>0){
		write(p[1], buf,len);
	}
	fsync(p[1]);
//	close(p[0]);
	//close(p[1]);
	return NULL;
}

int runTheCommand(struct codeBreaking *cb){
	pthread_attr_t attr[3];// could be done with one but for future expansion was than with 3 :)
	pthread_t pthreads[3];
	int pi[2], po[2], pe[2];
	int status;
	pid_t ch_pid;
	if (pipe(pi) || pipe(po) || pipe(pe)) return 22;
	ch_pid = fork();
	if (ch_pid==0) { /* child */
		close(po[1]);
		close(pi[0]);
		close(pe[0]);
		
		dup2(po[0], 0); close(po[0]);
		dup2(pi[1], 1); close(pi[1]);
		dup2(pe[1], 2); close(pe[1]); 
		char** argv = malloc(sizeof *argv * (cb->argv->size +1));
		*argv = cb->command;
		int i =0;
		struct node * n = cb->argv->first;
		for(i=0; i < cb->argv->size && n != NULL; i++, n = n->next){
			*(argv+(1+i)) = n->c;
		}
		//printCommand(cb); //debuging
		char *env[] = {NULL};
		int ret = execve(*argv, argv, env);
		
		//TODO: use the ret to determine something useful :)
		_exit(127);
	} else {
		close(po[0]);
		close(pi[1]);
		close(pe[1]);
		po[0]=0;
		pi[1]=1;
		pe[1]=2;
		int i;
		for (i =0 ; i<3;i++){
			pthread_attr_init(&attr[i]);
			pthread_attr_setdetachstate(&attr[i], PTHREAD_CREATE_JOINABLE);
		}
		if(pthread_create(&pthreads[0], &attr[0],myPipe, po) != 0 ) exit (100);
		if(pthread_create(&pthreads[1], &attr[0],myPipe, pi) !=0) exit(101);
		if(pthread_create(&pthreads[2], &attr[0],myPipe, pe) !=0) exit(102);
		
		//char* buf = malloc (sizeof *buf * 128);
		//int len =0;
		//while((len =read(pi[0], buf, 127)) >0)
		//	write(1,buf, len);
		//write(1,"\n",1);
		//dup2(po[1], 0); close(po[1]);
		//dup2(pi[0], 1); close(pi[0]);
		
		pid_t ret;
		ret = waitpid(ch_pid,&status , 0);
		// WAIT FOR PTHREADS
		void* status;
		int rc ;
		for (i=1; i < 3; i++){
			pthread_attr_destroy(&attr[i]);
			
			rc = pthread_join(pthreads[i],&status); // due to input never ending ;Д
			//TODO : do something with rc and status 
		}
		pthread_attr_destroy(&attr[0]);
		rc = pthread_cancel(pthreads[0]);
		/*
		status+='#';
		write(1,&status,1);
		if (ret == -1)
			write(1,"\nCHILD ERROR\n",13);
		if (ret == ch_pid )
			write(1,"\nCHILD NORMAL\n",14);
		*/
	}
	return status;
}

