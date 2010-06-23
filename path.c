#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
/*
int strcpy(char **p, const char *k, int len){
	*p = malloc(sizeof **p  * len);
	if (*p == NULL) return 1;
	while (len-- >=0) *(*p+len) = *(k+len);
	return 0;
	}
*/

int fileIsExec(const char * name){
		struct stat statbuf;
		int res = (stat(name, &statbuf) == 0); //  file exists 
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
		} else {
			char target_path[256];
			int len = readlink (name, target_path, sizeof (target_path));
			if (len == -1) {
				if (errno == EINVAL)
					;// It's not a symbolic link
				else
					exit (33);
			}
			else {
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
	char * real_file_name ;
	DIR *dir;
	while (*(path+len++) != '\0') ;
	int i = 0;
	while (1) {
		while (((len -i) !=0 && *(path+len-i-1) !=':' ))++i;
		dir_name = malloc(sizeof *dir_name *(i+1) );
		if (strncpy(dir_name, path+len-i, i) == NULL) exit (11);
		*(dir_name+i) = '\0';
//		write (1,"\n->", 3);
//		write (1, dir_name, strlen(dir_name));
		dir  = opendir(dir_name);
		if (dir == NULL) exit(12);
		while ( (file = readdir(dir))!= NULL){
			/* this could not work with symlinks */
			real_file_name = malloc(sizeof *real_file_name * (strlen(dir_name) + strlen(file->d_name)+5));
			strcpy(real_file_name, dir_name);
			strcat(real_file_name, "/");
			strcat(real_file_name, file->d_name);
			*(real_file_name+strlen(dir_name) + strlen(file->d_name)+1)  = '\0';
			//write (1,"\n--->", 5);
			//write (1, real_file_name, strlen(real_file_name));
			 if ( strcmp(name,file->d_name) == 0  ) {
				 if (!fileIsExec(real_file_name)) break; //I think there won't be another file with the same name :)
				int len = strlen(real_file_name);
				char * result = malloc (sizeof *result * (len +1));
				strcpy(result, file->d_name);
				// cleaning up
				return result;
			}
			free(real_file_name);
			//free (file); seg Fault
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


int main (int argc, char** argv){
	char * path ;
	path = getenv ("PATH");
	int i = 0;
	i=0;
	write (1,"\n",1);
	if (argc != 2) {
		write(1, "use path <argument>\n",21);
		return 1;	
	}
	while (*(*(argv+1)+i++) != '\0') ;
//	write (1,*(argv+1) ,i);
	char * path_to_exec = pathToExecutable(*(argv+1), path);
	write (1, path_to_exec, strlen(path_to_exec));
	return 0;
}
