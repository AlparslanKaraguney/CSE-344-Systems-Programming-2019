#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define INITIAL_STRING "gtushell:"
#define EXIT_STRING "exit"
#define INPUT_SIZE 1024




void execute_operations(int command_number,char *executable_files, char *operation[]);

int command_line_parser(char *command_words[],char *command_line);

void help();

void Signal_Handler(int signo){
    fprintf(stdout,"\nExiting safely\n");
    exit(0);
}

int command_line_parser(char *command_words[],char *command_line){
	int counter = 0;
	memset(command_words,'\0',INPUT_SIZE);
	char *split = "\n \t";
	char *parsed_words = strtok(command_line,split);

	while(parsed_words != NULL){

		command_words[counter] = parsed_words;
		++counter;
		parsed_words = strtok(NULL,split);
	}
	return counter;
}


int main (void) {
	
	struct sigaction signal_handler;
    memset(&signal_handler,0, sizeof(signal_handler));
    signal_handler.sa_handler=&Signal_Handler;
    sigaction(SIGINT,&signal_handler,NULL);
    sigset_t proc,prevProc;
    sigemptyset(&proc);
    sigaddset(&proc,SIGINT);

	char first_input[INPUT_SIZE];
	char *command_line_words[INPUT_SIZE];
	int command_word_count;
	char first_path[INPUT_SIZE];

	char previous_commands[100][INPUT_SIZE];
	int previoys_command_index = 0;

	getcwd(first_path,INPUT_SIZE);
	char path[INPUT_SIZE];
	
	sigprocmask(SIG_BLOCK,&proc,&prevProc);
     //   operate(TOKENS, tokCount, oldPath);
	while(1) {
		
		fprintf(stdout, "%s%s$ ",INITIAL_STRING,getcwd(path,INPUT_SIZE) );
		
		if (strcmp(fgets(first_input, INPUT_SIZE, stdin),"\n") == 0)
			continue;
				
		if (first_input[strlen(first_input) - 1] == '\n')
			first_input[strlen(first_input) - 1] = '\0';

		command_word_count = command_line_parser(command_line_words,first_input);

		if (command_word_count == 0)
			continue;

		if (command_line_words[0][0] == '!'){
			int index;
			index = atoi(&command_line_words[0][1]);

			if (index < previoys_command_index)
			{
				index = previoys_command_index - index;
				strcpy(previous_commands[previoys_command_index],previous_commands[index]);
				++previoys_command_index;
				command_word_count = command_line_parser(command_line_words,previous_commands[index]);
			}
			else
			{
				fprintf(stdout, "You did not type that much command\n" );
				continue;
			}
		}
		else
		{
			strcpy(previous_commands[previoys_command_index],first_input);
			++previoys_command_index;
		}
		
		if (strcmp(command_line_words[0],"exit") == 0)
			break;

		if (strcmp(command_line_words[0],"help") == 0){
			help();
			continue;
		}

		if (strcmp(command_line_words[0],"cd") == 0)
		{
		
			if (chdir(command_line_words[1]) != 0)
			{
				fprintf(stdout, "No such file or directory\n" );
				perror("");
				continue;
			}
			continue;
		}
		if (strcmp(command_line_words[0],"pwd") == 0 || strcmp(command_line_words[0],"lsf") == 0 || strcmp(command_line_words[0],"cat") == 0 ||
			strcmp(command_line_words[0],"wc") == 0 || strcmp(command_line_words[0],"bunedu") == 0)
		{

			execute_operations(command_word_count,first_path,command_line_words);
			wait(NULL);
		}
		else
		{
			fprintf(stdout, "%s: command not found\n",first_input );
		}
			wait(NULL);
	}
        sigprocmask(SIG_SETMASK,&prevProc,NULL);
	
	
	return 0;
}

void help()
{
	fprintf(stdout, "- lsf; which will list file type (R for regular file, S for non-regular(special) file),\n"
	" access rights (int the form of rwxr-xr-x, just like actual ls), file size(bytes) and file name of all files \n"
	" (not directories) in the present working directory. It doesn’t take any argument. Does not list any directory.\n");
	fprintf(stdout, "- pwd; which will print the present working directory\n") ;
	fprintf(stdout, "- cd; which will change the present working directory to the location provided as argument\n");
	fprintf(stdout, "- help; which will print the list of supported commands\n");
	fprintf(stdout, "- cat; which will print on standard output the contents of the file provided to it as argument or from\n "
	" standard input. The file can be on any directory on the system.\n");
	fprintf(stdout, "- wc; which will print on standard output the number of lines in the file provided to it as argument \n"
	" or the string coming from standard input until EOF character\n");
	fprintf(stdout, "- bunedu; displays the sizes of the subdirectories of the tree rooted at the directory\n");
	fprintf(stdout, "- !n; which will print on standard output n-th previously typed command\n");
	fprintf(stdout, "- exit; which will exit the shell\n");
}




void execute_operations(int command_number,char *executable_files ,char *operation[]){
	pid_t pid;
	int flag_pipe = -1;
	int flag_K = -1;
	int flag_B = -1;
	int in_out_pipe[2];

	
	for (int i = 0; i < command_number; ++i)
	{
		if (strcmp(operation[i],"|") == 0)
			flag_pipe = i;
		if (strcmp(operation[i],"<") == 0)
			flag_K = i;
		if (strcmp(operation[i],">") == 0)
			flag_B = i;
	}

	int pid_main = fork();

	if(pid_main == 0)
	{
		if (flag_K == -1 && flag_pipe == -1 && flag_B == -1	)
		{
			pid = fork();
			if(pid == 0)
			{
				char executable_path[INPUT_SIZE];
				sprintf(executable_path,"%s/%s", executable_files,operation[0]);
				execvp(executable_path,operation);
				exit(0);	
			}
		}
		else if(flag_pipe > -1)
		{

			if(pipe(in_out_pipe) == -1)
				fprintf(stdout, "Error pipe\n" );

			int pid = fork();
			
			if (pid == 0)
			{				
				close(in_out_pipe[0]);
				dup2(in_out_pipe[1],STDOUT_FILENO);

				close(in_out_pipe[1]);			


				char *temp[INPUT_SIZE];
				for (int i = 0; i < flag_pipe; ++i)
				{
					temp[i] = operation[i];
				}
				char executable_path[INPUT_SIZE];
				sprintf(executable_path,"%s/%s", executable_files,temp[0]);
				

				execvp(executable_path,temp);
				
				exit(0);
			}

			int pid1 = fork();

			if (pid1 == 0)
			{	
				close(in_out_pipe[1]);
				dup2(in_out_pipe[0],STDIN_FILENO);
				
				close(in_out_pipe[0]);

				char *temp[INPUT_SIZE];
				int j = 0;
				for (int i = flag_pipe+1; i < command_number; ++i)
				{
					temp[j] = operation[i];
					++j;
				}
				char executable_path[INPUT_SIZE];
				sprintf(executable_path,"%s/%s", executable_files,temp[0]);

				execvp(executable_path,temp);
				
				exit(0);
			}
			close(in_out_pipe[0]);
			close(in_out_pipe[1]);

		}


		else if (flag_K > -1) // <
		{
			if(pipe(in_out_pipe) == -1)
				fprintf(stdout, "Error pipe\n" );
			
			int pid = fork();
			if (pid == 0)
			{
				FILE *ptr;
				close(in_out_pipe[0]);
				dup2(in_out_pipe[1],STDOUT_FILENO);
				close(in_out_pipe[1]);

				char *temp[INPUT_SIZE];
				int j = 0;
				for (int i = flag_K+1; i < command_number; ++i)
				{
					temp[j] = operation[i];
					++j;
				}

				ptr = fopen(temp[0], "r");
				char s[1024];
				while(fgets(s,1024,ptr) != NULL)
				{
					fprintf(stdout, "%s",s );
				}

				fclose(ptr);
				exit(0);
			}
			else
			{
				close(in_out_pipe[1]);
				dup2(in_out_pipe[0],STDIN_FILENO);
				close(in_out_pipe[0]);

				char *temp[INPUT_SIZE];
				for (int i = 0; i < flag_K; ++i)
				{
					temp[i] = operation[i];
				}

				char executable_path[INPUT_SIZE];
				sprintf(executable_path,"%s/%s", executable_files,temp[0]);

				execvp(executable_path,temp);
			}
		}



		else if (flag_B > -1) // >
		{

			if(pipe(in_out_pipe) == -1)
				fprintf(stdout, "Error pipe\n" );

			int pid = fork();
			if (pid == 0)
			{
				FILE *ptr1;
				FILE *ptr2;
				close(in_out_pipe[1]);

				char *temp[INPUT_SIZE];
				int j = 0;
				for (int i = flag_B+1; i < command_number; ++i)
				{
					temp[j] = operation[i];
					++j;
				}

				ptr1 = fdopen(in_out_pipe[0], "r");
				ptr2 = fopen(temp[0], "w");

				
				char s[1024]; 
				while(fgets(s,1024,ptr1) != NULL)
				{
					fprintf(ptr2, "%s",s );
				}
				close(in_out_pipe[0]);

				fclose(ptr1);
				fclose(ptr2);
				exit(0);
			}
			else
			{
				close(in_out_pipe[0]);
				dup2(in_out_pipe[1],STDOUT_FILENO);

				close(in_out_pipe[1]);

				char *temp[INPUT_SIZE];
				for (int i = 0; i < flag_B; ++i)
				{
					temp[i] = operation[i];
				}
				char executable_path[INPUT_SIZE];
				sprintf(executable_path,"%s/%s", executable_files,temp[0]);

				execvp(executable_path,temp);
			}
			
		}
	}
	wait(NULL);
}