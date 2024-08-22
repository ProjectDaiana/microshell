#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

void erro(char *msg)
{
	while(*msg)
		write(2, msg++, 1);
}

void	setpipe(int haspipe, int *fd, int end)
{
//	printf("Lets pipe!\n")
	if(haspipe && (dup2(fd[end], end) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
	{
		erro("error: fatal\n");
		exit(1);
	}
}

int	cd(char **argv, int i)
{
	//printf("EXEC CD\n");
	if(i != 2)
	{
		erro("error: cd: bad arguments\n");
		return 1;
	}
	if(chdir(argv[1]) == -1)
	{
		erro("error: cd: cannot change directory to ");
		erro(argv[1]);
		erro("\n");
		return 1;
	}
	return 0;
}

int exec(char **argv, int i, char **envp)
{
	int status;
	int pid;
	int fd[2];
	int haspipe;

	haspipe = (argv[i] && !strcmp(argv[i], "|"));
	//printf("argv: %s\n", argv[0]);
	//printf("[i] %i\n", i);
	//printf("haspipe: %d\n", haspipe);
	if(!haspipe && !strcmp(argv[0], "cd"))
	{
		return(cd(argv, i));
	}
	if (haspipe && pipe(fd) == -1)
	{
		erro("error: fatal\n");
		exit(1);
	}
	pid = fork();
	if(pid == 0)
	{
		argv[i] = 0;
		//printf("child\n");
		setpipe(haspipe, fd, 1);
		if(!strcmp(argv[0], "cd"))
			return(cd(argv, i));
		execve(*argv, argv, envp);
		erro("error: cannot execute ");
		erro(argv[0]);
		erro("\n");
		exit(1);
	}
	waitpid(pid, &status, 0);
	setpipe(haspipe, fd, 0);
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
		while(argv[i] && strcmp(argv[i],"|") && strcmp(argv[i],";"))
			i++;
		if(i)
			status = exec(argv, i, envp);
		if (argv[i])
			i++;
	}
	//printf("status: %d\n", status);
	return status;
}



