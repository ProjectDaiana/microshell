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
	if (arg == NULL || i < 2)
	{
		errors("error: cd: bad arguments");
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
	int i;
	i = 0;
	if (strcmp(cmd, "cd") == 0)
	{
		exec_cd(argv[i + 1], i);
		return (0);
	}
	else {
		printf("EXECUTING: %s\n", argv[i]);
		if(execve(argv[i], cmd, env) == -1)
		{
			errors("error: cannot execute ");
			errors(argv[i]);
			errors("\n");
		}
	}
}

int main(int argc, char **argv, char **env)
{
	int i = 1;

	if (argc > 1)
	{
		while(argv[i])
		{
			printf("argv[%d]: %s\n", i, argv[i]);
			if (!strcmp(argv[i], ";"))
			{
				i++;
				continue;
			}
			int pid = fork();
			if (pid == -1)
				errors("error: cannot fork\n");
			if (pid == 0)
			{
				exec_cmd(argv[i], i, env);
			}
			waitpid(pid, NULL, 0);
			i++;
			printf("Continues w next command: %s\n", argv[i]);
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