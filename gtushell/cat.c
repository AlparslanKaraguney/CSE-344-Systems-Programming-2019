#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char const *argv[])
{
	struct stat p_stat;
	FILE *ptr;
	char input[1024];

	if(argc > 2)
	{
		fprintf(stdout, "usage: cat [filename]\n" );
		return 0;	
	}

	if(argc == 1)
	{
		while(fgets(input,1024,stdin) != NULL)
		{
			fprintf(stdout, "%s",input );
		}
	}
	else
	{
	
		if(lstat(argv[1], &p_stat) == -1){
			fprintf(stdout, "cat: %s: No such file or directory\n",argv[1] );
			return 1;
		}
		if(!S_ISREG(p_stat.st_mode)){
			fprintf(stdout, "cat: %s: Is not a regular file\n",argv[1] );
			return 1;
		}

		ptr = fopen(argv[1],"r");
		if (ptr == NULL)
		{
			perror("");
			return 0;
		}
		while(fgets(input,1024,ptr) != NULL){
			fprintf(stdout, "%s",input );
		}
	}
	fclose(ptr);
	return 0;
}