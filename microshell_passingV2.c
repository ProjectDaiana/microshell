#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void print_error(char *msg)
{
	while(*msg)
		write(2, msg++,1);
}

void set_pipe(int haspipe, int *fd, int end)
{
	//printf("Lets pipe\n");
	if (haspipe && (dup2(fd[end], end) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
	{
		print_error("error: fatal\n");
		exit(1);
	}
}

int cd(char *argv, int i)
{
	//printf("EXECUTING CD\n");
	if(i != 2)
	{
		print_error("error: cd: bad arguments\n");
		return(1);	
	}
	if (chdir(argv) == -1)
	{
		print_error("error: cd: cannot change directory to ");
		print_error(&argv[1]);
		print_error("\n");
		return(1);
	}
	return 0;
}

int exct(char **argv, int i, char **envp)
{
	int status;
	int haspipe = (argv[i] && !strcmp(argv[i], "|"));
	int pid;
	int fd[2];

	//printf("argv: %s\n",argv[i-1]);
	//printf("i is : %d\n",i);
	if (!haspipe && !strcmp(argv[0], "cd"))
	{
	//	printf("no pipe cd \n");
		return(cd(argv[i-1], i));
	}
//	if (haspipe == 1)
//		printf("haspipe\n");
	if(haspipe && pipe(fd) == -1)
	{
		print_error("error: fatal\n");
		exit(1);
	}
	pid = fork();
	if(pid == 0)
	{		
		argv[i] = 0;
		set_pipe(haspipe, fd, 1);
		//printf("after pipe\n");	
		if(haspipe && !strcmp(argv[0],"cd"))
			return(cd(argv[i-1], i));
	//	printf("executing in child\n");
		execve(*argv, argv, envp);
		print_error("error: cannot execute ");
		print_error(*argv);
		print_error("\n");
		exit(1);
	}
	waitpid(pid, &status, 0);
	set_pipe(haspipe, fd, 0);
	return(WIFEXITED(status) && WEXITSTATUS(status));
}

int main(int argc, char **argv, char **envp)
{
	(void)argc;
	int i = 1;
	int status = 0;

	while(argv[i])
	{
		argv += i;		
		i = 0;
		while(argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (i)
			status = exct(argv, i, envp);
		if(argv[i])
			i++;
	}
	//printf("status: %d", status);
	return status;
}