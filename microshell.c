#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int errors(char *msg)
{
	write(2, msg, strlen(msg));
	return 1;
}

int	exec_cd(char *arg, int i)
{
	if (arg == NULL || i < 2)
		errors("error: cd: bad arguments");
	if(chdir(arg) == -1)
	{
		errors("error: cd: cannot change directory to ");
		errors(arg);
		errors("\n");
	}
	//NEXT 5 LINES ONLY FOR DEBUGGING
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("Current Directory: %s\n", cwd);
	}
	return(0);	
}

int main(int argc, char **argv, char **env)
{
	int i = 1;

	if (argc > 1)
	{
			while(argv[i])
			{
				int pid = fork();

				if (pid == -1)
					errors("error: cannot fork\n");
				if (pid == 0)
				{
					printf("argv[%d]: %s\n", i, argv[i]);
					if(!strcmp(argv[i], ";"))
					{
						i++;
						printf("skipping ; i: %s\n", argv[i]);
						continue;
					}
					// else if(strcmp(argv[i], "|"))
					// {
					// 	dup2(argv[i]);
					// }
					char *cmd[] = {argv[i], NULL}; 
					printf("cmd: %s\n", cmd[0]);
					//EXECUTE cd or path
					//Search for "/" in the command line If found, execute the command
					// else check if is cd and has an argument
					if (strcmp(argv[i], "cd") == 0)
						exec_cd(argv[i + 1], i);	
					printf("EXECUTING: %s\n", argv[i]);
					if(execve(argv[i], cmd, env) == -1)
					{
						errors("error: cannot execute ");
						errors(argv[i]);
						errors("\n");
					}
					else
						waitpid(pid, NULL, 0);
				}
				i++;
				printf("Continues w next command: %d\n", pid);
			}
	}
	return (0);
}


/*
$>./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
microshell
i love my microshell
$>

>./microshell
*/