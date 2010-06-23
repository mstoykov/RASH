//      path_misc.c
//      
//      Copyright 2010 M.Stoikov <mstoikov@lt3.home.say>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include "path_misc.h"

int fileIsExec(const char * name){
		struct stat statbuf;
		int res = (stat(name, &statbuf) == 0);  
		if (res != 0 ) {
			if (S_ISLNK(statbuf.st_mode)){ //it's a symlink
				char target_path[256];
				int len = readlink (name, target_path, sizeof (target_path));
				if (len == -1) {
					if (errno == EINVAL)
						exit(32);// It's not a symbolic link
					else
						exit (33);
				}
				else {
					target_path[len] = '\0';
					int p = fileIsExec(target_path);
					return p;
				}
			}else {
			 	res = (S_ISREG(statbuf.st_mode)   // the file is regular or symlink 
			 		&&
					( ((statbuf.st_mode & S_IXUSR) == S_IXUSR && getuid() == statbuf.st_uid) // we are the owner and we've got the rights
					|| ((statbuf.st_mode & S_IXGRP) == S_IXGRP && getgid() == statbuf.st_gid) // we are from the group and we've got the rights
					|| ((statbuf.st_mode & S_IXOTH) == S_IXOTH)));// we are nobody but we've got the rights
				}
		} else { // interesting :)
			char target_path[256];
			int len = readlink (name, target_path, sizeof (target_path));
			if (len == -1) {
				if (errno == EINVAL)
					;// It's not a symbolic link
				else
					exit (33); //something went terribly wrong
			} else {
				target_path[len] = '\0';
				int p = fileIsExec(target_path);
				return p;
			}
		}
		return res;
}
char* pathToExecutable(char* name, char* path){
	int len = 0;
	struct dirent *file;
	char * dir_name;
	char * real_file_name = alloca(sizeof *real_file_name * 512); //magical number for maximum path length
	DIR *dir;
	while (*(path+len++) != '\0') ;
	int i = 0;
	while (1) {
		while (((len -i) !=0 && *(path+len-i-1) !=':' ))++i;
		dir_name = alloca(sizeof *dir_name *(i+1) );
		if (strncpy(dir_name, path+len-i, i) == NULL) exit (11);
		*(dir_name+i) = '\0';
		//write (1,"\n->", 3);
		//write (1, dir_name, strlen(dir_name));
		dir  = opendir(dir_name);
		if (dir == NULL) exit(12);
		while ( (file = readdir(dir))!= NULL){
			/* this could not work with symlinks */
			//real_file_name = alloca(sizeof *real_file_name * (strlen(dir_name) + strlen(file->d_name)+5));
			strcpy(real_file_name, dir_name);
			strcat(real_file_name, "/");
			strcat(real_file_name, file->d_name);
//			*(real_file_name +  (strlen(dir_name) + strlen(file->d_name)) + 1) = '\0';
//			write (1,"\n--->", 5);
//			write (1, real_file_name, strlen(real_file_name));
//			write (1,"\n",1);
			if ( strcmp(name,file->d_name) == 0  ) {
		                if (!fileIsExec(real_file_name)) break; //I think there won't be another file with the same name :)
				int len = strlen(real_file_name);
				char * result = malloc (sizeof *result * (len +1));
				strcpy(result, real_file_name);
				// cleaning up
				return result;
			}
//			free(real_file_name);
			//free (file); //seg fault ;)
		}
		if ((len - i) == 0) break;
		// here we go again :) 
		i++;
		len -= i;
		i =0;
		free(dir_name);
	}
	return NULL;
}

void  nextChar(struct codeBreaking * code, char * c) {
	if (code->len <= 0 && ((code->len =read(code->fs, code->buf, BUF_LEN-1 ))<= 0)){   // I hope it is the end of the file and will see how it will work with interactive shell :)
		*c= 3 ; //supposevely end of file /text
		return;
	}
	*c = *(code->buf+code->index++);
	code->len--;
}

void setCommand(struct codeBreaking * code, char ** argument){
	//write(1,*argument, strlen(*argument));
	code->command = pathToExecutable(*argument, getenv("PATH"));
	if(code->command == NULL) exit (127);
}

// returns 0 if more 1 if end and something else on errors / probably errno in the future 
int nextArgument(char ** arg, struct codeBreaking * code){
	*arg =NULL;
	int one=0,two=0,esc=0;
	int result =0;
	char * cur = alloca (sizeof *cur);
	char * argument = alloca  (sizeof *argument * 128);
	int ind=0;
	while (1){
		nextChar(code, cur);
		if (*cur == 3){
			if (ind ==0) 
				return 1;
			if (esc ==1 || one ==1 || two ==1) 
				exit (122);
			result =1;
			break;
		}
		if (esc==1){
			if (*cur == '\n')
				 continue; // hopefully this is multi line command
			*(argument+ind++) = *cur;
			esc=0;
		} else {
			if (*cur == '\\')
				esc =  1 -esc;
			else if (two == 0 && *cur == '\'')
				one = 1 - one;
			else if (one == 0 && *cur == '"')
				two = 1- two;
			else if (two ==0 && one ==0 && (*cur == ' ' || *cur == ';' || *cur == '\n')){ // all end characters some day
				if (ind !=0){
					if (*cur == ';' || *cur == '\n')
						result = 1;
					break;
				}
			} else if (one == 0 && two == 0 && *cur == '#'){
				if (ind > 0) {
					code->index--;
					result =1;
					break;
				}
				do{nextChar(code, cur);}while(*cur != '\n' );
			} else 
				*(argument+ind++) = *cur;
		}
	}
	*arg= malloc(sizeof *arg * (ind +1));
	strncpy(*arg, argument, ind);
	*(*arg+ind) = '\0';
	return result;
}

// the magical thingy that have to cut everything and make most of the logical preexecution things, hopefully right :D
void cutCommand(struct codeBreaking * code ){
	char * buf;
	int i ;
	i= nextArgument(&buf, code);
	if (i !=0 && i != 1) 
		exit (111); // magical error code
	if(i== 1) return;
	setCommand(code, &buf);
	while ((i = nextArgument(&buf, code)) == 0){
		addToList(code->argv, buf);
	}
	if (buf != NULL)
		addToList(code->argv, buf);
	if (i !=1) exit (112); //another magical error code
}

void printCommand (struct codeBreaking * code){
	write (1, "command :", 9);
	struct node * n = code->argv->first;
	int i = 0, br=0;
	while(*(code->command + i ) != '\0') i++;
	write(1, code->command, i);
	char in = ' ';
	while( br < code->argv->size){
		i=0;
		while(*(n->c + i ) != '\0') i++;
		write(1, &in,1);
		write(1, n->c, i);
		n = n->next;
		br++;
	}
	write(1,"\n",1);
	
}

/*
int main(){
	struct codeBreaking cb ;
	init(&(cb.argv));
	if ( (cb.fs = open("rash.test2", O_RDONLY)) == -1){
		if (errno ==ENOENT){
			write (1,"ENOENT",6);
		} else if(errno ==EACCES) {
				write (1,"EACCES",6);
		} else if(errno == EEXIST) {
			write (1,"EEXIST", 6);
		} else {
			write (1,"UNKNOWN", 7);
		}
		return 1;
	}
	//cb.fs = 0; //interactive shell :)
	cb.command=NULL;
	char *c = malloc (sizeof c);
	cb.buf =malloc (sizeof *cb.buf * BUF_LEN);
	cb.len =0;
	while (1) {
		nextChar(&cb, c);
		if (*c==3) break;
		else write(1,c,1);
	}
	//sleep(5);
	cutCommand(&cb);
	//sleep(5);
	close (cb.fs);
	//sleep(5);
	runTheCommand(&cb);
	//sleep(5);
	printCommand(&cb);
	return 0;
}*/
