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

void handler(int signal)
{
	if (signal == SIGINT)
	{	
		write(1, "SIGINT\n", 7);
	}
	if (signal == SIGQUIT)
	{	
		exit(0);
	}
}

void set_signals()
{
	sigset_t			sigset;
	struct sigaction	sa;

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