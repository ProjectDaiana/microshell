#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int errors(char *msg)
{
	write(2, msg, strlen(msg));
	return 1;
}

void print_cwd()
{
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("Current Directory: %s\n", cwd);
	}
}

int	exec_cd(char *arg, int i)
{
	print_cwd();
	if (arg == NULL || i != 2)
	{
		errors("error: cd: bad arguments");
		errors("\n");
		return(1);
	}
	if(chdir(arg) == -1)
	{
		errors("error: cd: cannot change directory to ");
		errors(arg);
		errors("\n");
		return(1);
	}
	print_cwd();
	return(0);	
}

int	exec_cmd(char **cmd, int i, char **env)
{
	if (strcmp(*cmd, "cd") == 0)
	{
		exec_cd(cmd[1], i);
		return (0);
	}
	else {
		printf("EXECUTING: %s\n", cmd[0]);
		cmd[i] = NULL;
		if(execve(*cmd, cmd, env) == -1)
		{
			errors("error: cannot execute ");
			errors(*cmd);
			errors("\n");
			return (1);
		}
	}
	return (0);
}

int main(int argc, char **argv, char **env)
{
	int i = 1;

	if (argc > 1)
	{
		while(argv[i])
		{
			argv += i;
			i = 0;
			printf("argv[%d]: %s\n", i, argv[i]);
			while (argv[i] && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0)
			{
				i++;
			}
			printf("i is :%d\n",i);
			
			int pid = fork();
			if (pid == -1)
				errors("error: cannot fork\n");
			if (pid == 0)
			{
				exec_cmd(argv, i, env);
				return (0);
			}
			waitpid(pid, NULL, 0);

			if (argv[i] == NULL)
				break;
			i++;
			printf("Continues w next command: %s i: %d\n", argv[i], i);
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