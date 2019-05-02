#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char const *argv[])
{
	struct stat p_stat;
	FILE *ptr;
	int counter = 0;

	if(argc > 2)
	{
		fprintf(stdout, "wc: usage: wc filename\n" );
		return 0;	
	}

	if (argc == 1)
	{
		char temp[1024];
		//fgets(temp,1024,ptr);
		while(fgets(temp,1024,stdin) != NULL){
			++counter;
		}
	}
	else{
		
		if(lstat(argv[1], &p_stat) == -1){
			fprintf(stdout, "wc: %s: No such file or directory\n",argv[1] );
			return 1;
		
		}
		if(!S_ISREG(p_stat.st_mode)){
			fprintf(stdout, "wc: %s: Is not a regular file\n",argv[1] );
			return 1;
		}

		ptr = fopen(argv[1],"r");

		if (ptr == NULL)
		{
			perror("");
			return 0;
		}
		char c;
		
		do{
			c = fgetc(ptr);
			if (c == '\n')
				++counter;
			
		}while(c != EOF);
		++counter;

		fclose(ptr);
	}

	fprintf(stdout, "%d \n",counter );

	return 0;
}