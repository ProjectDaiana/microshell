#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

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
	else
	{
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

int is_pipe(char *arg)
{
	if (arg && strcmp(arg, "|") == 0)
	{
		printf("PIPE FOUND\n");
		return (1);
	}
	return (0);
}

int execute(char **argv, int i, char **env)
{
	int	fd[2];
	int pid;
	int status;

	if (is_pipe(argv[i]) && pipe(fd) == -1)
	{
		errors("error: cannot create pipe\n");
		exit(1);
	}
	pid = fork();
	if (pid == -1)
		errors("error: cannot fork\n");
	if (pid == 0)
	{
		if (fd[1])
		{
			dup2(fd[1], 1);
			close(fd[1]);
			close(fd[0]);
		}
		printf("i in child is :%d\n",i);
		printf("execute argv: %s\n", argv[i]);
		exec_cmd(argv, i, env);
		errors("error: cannot execute command\n");
		errors(argv[0]);
		errors("\n");
		exit(1);
	}
	waitpid(pid, &status, 0);
	if(is_pipe(argv[i]))
		dup2(fd[0], 0);
	close(fd[0]);
	close(fd[1]);
	return (WIFEXITED(status) && WEXITSTATUS(status));
}

int main(int argc, char **argv, char **env)
{
	int	i = 1;
	int status = 0;

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
			if (i)
				status = execute(argv, i, env);
			printf("Continues w next command: %s i: %d\n", argv[i], i);
			if (argv[i] == NULL)
				break;
			i++;
		}
	}
	return (status);
}


/*
$>./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
microshell
i love my microshell
$>

>./microshell 
*/