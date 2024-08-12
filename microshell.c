#include <stdio.h>
#include <string.h>
#include <unistd.h>

int errors(char *msg)
{
	write(2, msg, strlen(msg));
	return 1;
}

int main(int argc, char **argv, char **env)
{
	int i = 1;

	if (argc > 1)
	{
			printf("microshell: %s\n", argv[i]);
			while(argv[i])
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
				//Search for "/" in the command line
				// If found, execute the command
				// else is an argument
				if (strcmp(argv[i], "cd") == 0)
				{
					if (argv[i + 1] == NULL)
						errors("error: cd: bad arguments");
					if(chdir(argv[i + 1]) == -1)
					{
						errors("error: cd: cannot change directory to ");
						errors(argv[i + 1]);
						errors("\n");
					}
					//NEXT 5 LINES ONLY FOR DEBUGGING
					char cwd[1024];
                    if (getcwd(cwd, sizeof(cwd)) != NULL)
                    {
                        printf("Current Directory: %s\n", cwd);
                    }									
					// int e = 0;
					// while (env[e])
					// {
					// 	printf("%s\n",env[e]);
					// 	e++;
					// }
				}
				printf("executing: %s\n", argv[i]);
				if(execve(argv[i], cmd, env) == -1)
				{
					errors("error: cannot execute ");
					errors(argv[i]);
					errors("\n");
				}
				i++;
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