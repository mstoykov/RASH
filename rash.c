//      rash.c
//      
//      Copyright 2010 M.Stoikov <m.stoikov@gmail.com>
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
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include "path_misc.h"
#include "execution.h"
void interactive(){
	struct codeBreaking cb ;
	init(&(cb.argv));
	cb.fs = 0;
	cb.command=NULL;
	char *c = malloc (sizeof c);
	cb.buf =malloc (sizeof *cb.buf * BUF_LEN);
	cb.len =0;
	while(1){
		cutCommand(&cb);
		if (cb.command == NULL) break;
		fsync(0);
		fsync(1);
		fsync(2);
		runTheCommand(&cb);
		freeAll(cb.argv);
		cb.command = NULL;
	}
	//printCommand(&cb);
	close (cb.fs);
}

void executeFile(char * filename){
	struct codeBreaking cb ;
	init(&(cb.argv));
	if ( (cb.fs = open(filename, O_RDONLY)) == -1){
		if (errno ==ENOENT){
			write (1,"ENOENT",6);
		} else if(errno ==EACCES) {
				write (1,"EACCES",6);
		} else if(errno == EEXIST) {
			write (1,"EEXIST", 6);
		} else {
			write (1,"UNKNOWN", 7);
		}
		return;
	}
	cb.command=NULL;
	char *c = malloc (sizeof c);
	cb.buf =malloc (sizeof *cb.buf * BUF_LEN);
	cb.len =0;
	while(1){
		cutCommand(&cb);
		if (cb.command == NULL) break;
		runTheCommand(&cb);
		//printCommand(&cb);
		freeAll(cb.argv);
		cb.command = NULL;
	}
	
	close (cb.fs);
}

int main(int argc, char** argv)
{
	if (argc >2) 
		write(1, "usage: rash [file]",18);	
	if (argc == 1) 
		interactive();
	if (argc == 2)
		executeFile(*(argv+1));
	return 0;
}
