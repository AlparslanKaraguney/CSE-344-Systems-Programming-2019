#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	char path[1024];
	if(argc > 1)
	{
		fprintf(stdout, "usage: pwd\n" );
		return 0;	
	}
	if (strcmp(argv[0],"pwd") == 0)
	{
		getcwd(path,1024);
		fprintf(stdout, "%s\n",path );
	}
	return 0;
}