#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../inc/minishell.h"

int	parse_line(char *line, char **envp, t_pipe *in, t_pipe *out)
//int	parse_line(char *line, char **envp)
{
	char	**args;
	char	*cmd;
	pid_t	pid;
//	t_pipe	p;
	int		exit;
	pid = fork();
	if (pid > 0)
		exit = set_signals(pid, envp);
	if (pid == 0)
	{
		//fd = check_redir(args); //si hay redireccion, borrarla de la linea
		if (in != NULL)
		{
			dup2(in->p[0], 0);
			close(in->p[0]);
			close(in->p[1]);
//			test_pipe(in);
		}
		if (out != NULL)
		{
			dup2(out->p[1], 1);
			close(out->p[0]);
			close(out->p[1]);
		}
		dup2(1, 2);
		args = ft_split_marks(line, ' ');
		cmd = file_cmd(args[0], envp);
		execve(cmd, args, envp);
	}
	exit = WEXITSTATUS(exit);
	return (exit);
}

int parse_pipex(char *line, char **envp)
{	
	t_shell shell;
	int		i;

	shell.pipex = count_ascii(line, '|'); 
	shell.pipes = ft_split(line, '|');
	if (shell.pipex == 0)
		shell.exit = parse_line(shell.pipes[0], envp, NULL, NULL); //sin pipe
	else 
	{
		shell.p = (t_pipe *)malloc(sizeof(t_pipe) * (shell.pipex + 1));
		pipe(shell.p[0].p);
		shell.exit = parse_line(shell.pipes[0], envp, NULL, &shell.p[0]); //primer pipe
		close(shell.p[0].p[1]); // cierras la salida/escritura del pipe
		i = 1;
		while (shell.pipex > 1 && shell.pipes[i + 1])
		{
			pipe(shell.p[i].p);
			shell.exit = parse_line(shell.pipes[i], envp, &shell.p[i - 1], &shell.p[i]);// pipe intermedio
			close(shell.p[i - 1].p[0]);
			close(shell.p[i].p[1]);
			i++;
		}
		shell.exit = parse_line(shell.pipes[i], envp, &shell.p[i - 1], NULL); //ultimo pipe
		close(shell.p[i - 1].p[0]);
	}
	return (shell.exit);
}

static void handler(int sig) 
{	
	if (sig == SIGINT)
    	write(1, "\njgravalo> ", 11);
	if (sig == SIGQUIT)
		return ;
}
  
int new_shell(char **envp)
{	
	char				*c;
	int					exit_code;

	set_termios();
	while (1)
	{	
		signal(SIGINT, handler);
		signal(SIGQUIT, handler);
		c = readline("jgravalo> ");
		if (c == NULL)
		{	
			write(1, "exit\n", 6);
			exit(1);
		}
		if (c[0] != 0)
		{	
			add_history(c);
			exit_code = parse_pipex(c, envp);
			free(c);
		}
	}
		return (exit_code);
}

int main(int argc, char **argv, char **envp)
{	
	if (!argc && !argv && !envp)
		return (0);
	new_shell(envp);		
}

