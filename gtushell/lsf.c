#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

int sizepathfun (char *path);

void filestat (struct stat p_stat);

int main(int argc, char const *argv[])
{
	struct dirent *p;
	DIR *Dirp;
	struct stat p_stat;
	char path[1024];
	char new_path[1024];
	getcwd(path,1024);
	int result;
	if((Dirp = opendir(path)) == NULL){	
			perror ("Could not open directory\n");
			return -1;
		}

		while((p = readdir(Dirp)) != NULL){		

			if(strcmp(".",p->d_name) == 0 || strcmp("..",p->d_name) == 0)	continue;

			sprintf(new_path,"%s/%s", path, p->d_name);

			
			if(lstat(new_path, &p_stat) == -1){
				perror("Could not get the status of the file\n");
				return 1;
			}

			if(S_ISDIR(p_stat.st_mode)){
				continue;
			}	
			
			else if(S_ISREG(p_stat.st_mode)){
				result = sizepathfun(new_path);
				fprintf(stdout, "R\t");
				filestat(p_stat);
				fprintf(stdout, "\t%d\t%s\n",result, p->d_name);

				
			}	

			else if(S_ISFIFO(p_stat.st_mode) || S_ISLNK(p_stat.st_mode)){	
				fprintf(stdout, "S\t");
				fprintf(stdout, "\t0\t%s\n", p->d_name);

			}	
			
		}
		
	return 0;
}

int sizepathfun (char *path){
	struct stat p_stat;

	if(lstat(path, &p_stat) == -1){	//get the stat of the given regular file
		perror("Could not get the status of the file");
		return 1;
	}
	return p_stat.st_size;	//return the size of given regular file
}

void filestat (struct stat p_stat){

	printf((p_stat.st_mode & S_IRUSR) ? "r":"-");
	printf((p_stat.st_mode & S_IWUSR) ? "w":"-");
	printf((p_stat.st_mode & S_IXUSR) ? "x":"-");

	printf((p_stat.st_mode & S_IRGRP) ? "r":"-");
	printf((p_stat.st_mode & S_IWGRP) ? "w":"-");
	printf((p_stat.st_mode & S_IXGRP) ? "x":"-");
	
	printf((p_stat.st_mode & S_IROTH) ? "r":"-");
	printf((p_stat.st_mode & S_IWOTH) ? "w":"-");
	printf((p_stat.st_mode & S_IXOTH) ? "x":"-");
}