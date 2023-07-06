# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
#include "../inc/minishell.h"

pid_t child_pid;

void handler(int signal)
{
	if (signal == SIGINT)
	{	
		exit(0);
	}
	else if (signal == EOF)
	{	
		exit(0);
	}
}

void set_signals(int pid)
{
	sigset_t			sigset;
	struct sigaction	sa;

	child_pid = pid;
	printf("child pid is %d\n", getpid());
	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&sigset);
	sa.sa_handler = handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaddset(&sigset, SIGINT);
	sigaddset(&sigset, SIGQUIT);
	while (1)
		;
}