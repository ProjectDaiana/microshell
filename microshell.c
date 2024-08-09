#include <stdio.h>

int errors(char *msg)
{
	printf("%s\n", msg);
	return 1;
}

int main(int argc, char **argv, char **env)
{
	if (argc > 1)
	{
		printf("Usage: %s <command>\n", argv[0]);
		return 1;
	}
	return (0);
}