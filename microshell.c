#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int errors(char *msg)
{
	while (*msg)
		write(2, msg++, 1);
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
	//print_cwd();
	if (arg == NULL || i != 2)
	{
		errors("error: cd: bad arguments");
		errors("\n");
		exit(1);
	}
	if(chdir(arg) == -1)
	{
		errors("error: cd: cannot change directory to ");
		errors(arg);
		errors("\n");
		exit(1);
	}
	//print_cwd();
	return(0);	
}

void set_pipe(int has_pipe, int *fd, int end)
{
	if(has_pipe && (dup2(fd[end], end) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
	{
		errors("error: fatal dup\n");
		exit(1);
	}
}


int execute(char **argv, int i, char **env)
{
	int fd[2];
	int pid;
	int status;
	int has_pipe;
	has_pipe = argv[i] && !strcmp(argv[i], "|");

	if (!has_pipe && !strcmp(*argv, "cd"))
		return (exec_cd(*argv, i));
	if (has_pipe && pipe(fd) == -1)
	{
		errors("error: fatal\n");
		exit(1);
	}
	pid = fork();
	if (pid == 0)
	{
		argv[i] = 0;
		set_pipe(has_pipe, fd, 1);
		if (!strcmp(*argv, "cd"))
			return (exec_cd(*argv, i));
		//printf("i in child is :%d\n",i);
		//printf("execute argv: %s\n", argv[1]);
		execve(*argv, argv, env);
		errors("error: cannot execute "), errors(*argv), errors("\n");
		exit(1);
	}
	waitpid(pid, &status, 0);
    set_pipe(has_pipe, fd, 0);
    return WIFEXITED(status) && WEXITSTATUS(status);
}

int main(int argc, char **argv, char **env)
{
	int	i = 1;
	int status = 0;
	(void)argc;

	while(argv[i])
	{
		argv += i;
		i = 0;
		//printf("argv[%d]: %s\n", i, argv[i]);
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		//printf("i is :%d\n",i);
		if(i)
			status = execute(argv, i, env);
		//printf("Continues w next command: %s i: %d\n", argv[i], i);
		if(argv[i] == NULL)
			break;
		i++;
	}
	return (status);
}
