/*Alparslan KARAGUNEY
	1610440100	*/
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int z = 0;	//check for the [-z] operation use

int postOrderApply (char *path, int pathfun (char *path1));

int sizepathfun (char *path);

int main(int argc, char *argv[]) {
	
	//usage control
	//arguman conrol for option wtihout [-z]
	if (argc == 1)
	{
		if (strcmp("bunedu",argv[0]))
		{
			fprintf(stderr, "Usage: bunedu [-z] rootpath\n");
			return 1;
		}
		char temp[1024];
		fgets(temp,1024,stdin);
		postOrderApply(temp,sizepathfun);
	}
	else if (argc == 2) {
		if(strcmp("bunedu",argv[0])){
			fprintf(stderr, "Usage: bunedu [-z] rootpath\n");
			return 1;
		}
		if(strcmp("-z",argv[1]) == 0){
			char temp[1024];
			fgets(temp,1024,stdin);
			z = 1;
			fprintf(stdout, "tempim: %s\n",temp );
			postOrderApply(temp,sizepathfun);
			return 0;
		}
		postOrderApply(argv[1],sizepathfun);	//call for recursive function
		
	}
	//arguman control for option [-z]
	else if(argc == 3){
		if(strcmp("bunedu",argv[0]) || strcmp("-z",argv[1])){
			fprintf(stderr, "Usage: bunedu [-z] rootpath\n");
			return 1;
		}
		z = 1;
		postOrderApply(argv[2],sizepathfun);
	}
	//any other function call is wrong
	else{
		fprintf(stderr, "Usage: bunedu [-z] rootpath\n");
		return 1;
	}
	
	return 0;

}

int postOrderApply (char *path, int pathfun (char *path1)){
	struct dirent *p;
	DIR *Dirp;
	struct stat p_stat;
	char new_path[255];	//next path address
	int result = 0;

	//check for secret file in the directory
	if(strcmp(".",path) == 0 || strcmp("..",path) == 0)	return 0;


	if(lstat(path, &p_stat) == -1){	//get the statues of the file for the stat function
		perror("Could not get the status of the filesss");
		return 1;
	}
	if(access(path,R_OK)){	//check the file for the access
		printf("Cannot read folder %s\n",path );
		return -1;
	}
	
	if(S_ISREG(p_stat.st_mode))		//for regular files call pathfun function
		result += pathfun(path);

	else if(S_ISFIFO(p_stat.st_mode))	//for special files warn the user
		printf("Special file %s\n", path);
	
	else if(S_ISLNK(p_stat.st_mode))	//for special files warn the user
		printf("Special file %s\n", path);
	
	else if(S_ISDIR(p_stat.st_mode))	//directory files
	{
		
		if((Dirp = opendir(path)) == NULL){	//try to open the directory
			perror ("Could not open directory\n");
			return -1;
		}

		while((p = readdir(Dirp)) != NULL){		//try to read the directory until reach the end of it

			if(strcmp(".",p->d_name) == 0 || strcmp("..",p->d_name) == 0)	continue;
			//check for secret file in the directory
			sprintf(new_path,"%s/%s", path, p->d_name);
			//since we open directory get the new path
			
			//get the new path statues
			if(lstat(new_path, &p_stat) == -1){
				perror("Could not get the status of the file\n");
				return 1;
			}

			if(S_ISDIR(p_stat.st_mode))		//if it is a new directory call the funtion again for calculate its total size
				result += postOrderApply(new_path, pathfun);
			
			else if(S_ISREG(p_stat.st_mode))	//if it is a regular file call pathfun function
				result += pathfun(new_path);

			else if(S_ISFIFO(p_stat.st_mode))	//special file warning
				printf("Special file %s\n", p->d_name);
	
			else if(S_ISLNK(p_stat.st_mode))	//special file warning
				printf("Special file %s\n", p->d_name);
		}

		closedir(Dirp);	//close the directory
	}
	else	//any other file return -1;
		return -1;

	printf("%d 	  %s\n",result/1024,path);	//print the size and path info to the user

	if(!z)	//if [-z] operation is not used do not return subdirectories size
		return 0;
	return result;	//if [-z] operation is used then return the size of subdirectories to add the total size
}

int sizepathfun (char *path){
	struct stat p_stat;

	if(lstat(path, &p_stat) == -1){	//get the stat of the given regular file
		perror("Could not get the status of the file");
		return 1;
	}
	return p_stat.st_size;	//return the size of given regular file
}