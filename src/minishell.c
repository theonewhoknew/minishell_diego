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

//int	search_redir(char **args)

int	check_redir(char **args, int type)
{
	int fd;
	int i;

	i = 0;
	while (args[i])
	{
		if (type == 0 && ft_strcmp(args[i], ">"))
			fd = open(args[i + 1], O_RDWR | O_CREAT | O_TRUNC, 00644); // > salida
		if (ft_strcmp(args[i], ">>"))
			fd = open(args[i + 1], O_RDWR | O_CREAT | O_APPEND, 00644); // >> salida
		if (ft_strcmp(args[i], "<"))
			fd = open(args[i + 1], O_RDONLY); // < entrada
//		if (ft_strcmp(args[i], "<<"))
		i++;
	}
	return (fd);
}

void test_pipe(t_pipe *p)
{
//	write(p->p[1], "el pipe funciona\n", 17);
	char buffer[17];
	read(p->p[0], buffer, 17);
	write(1, "<<<", 3);
	write(1, buffer, 17);
	write(1, ">>>", 3);
	write(1, "\n", 1);
}

int	parse_line(char *line, char **envp, t_pipe *in, t_pipe *out)
//int	parse_line(char *line, char **envp)
{
	char	**args;
	char	*cmd;
	pid_t	pid;
//	t_pipe	p;
	int		exit;
	pid = fork();
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
//	test_pipe(out);
	waitpid(pid, &exit, 0);
	exit = WEXITSTATUS(exit);
	return (exit);
}

int parse_pipex(char *line, char **envp)
{
	char	**pipes;
	int		i;
	int		pipex;
	int		exit;
	t_pipe	*p;

	pipex = count_ascii(line, '|'); 
//	t_pipe	p[pipex];
//	printf("n_pipes = %d\n", pipex);
	pipes = ft_split(line, '|');
	if (pipex == 0)
		exit = parse_line(pipes[0], envp, NULL, NULL); //sin pipe
	else 
	{
		p = (t_pipe *)malloc(sizeof(t_pipe) * (pipex + 1));
		pipe(p[0].p);
		exit = parse_line(pipes[0], envp, NULL, &p[0]); //primer pipe
//		test_pipe(&p[0]);
		close(p[0].p[1]); // cierras la salida/escritura del pipe
		i = 1;
		while (pipex > 1 && pipes[i + 1])
		{
			pipe(p[i].p);
//			test_pipe(&p[i - 1]);
			exit = parse_line(pipes[i], envp, &p[i - 1], &p[i]);// pipe intermedio
//			test_pipe(&p[i]);
			close(p[i - 1].p[0]);
			close(p[i].p[1]);
			i++;
		}
		exit = parse_line(pipes[i], envp, &p[i - 1], NULL); //ultimo pipe
		close(p[i - 1].p[0]);
	}
	return (exit);
}

int parse_ands(char *line, char **envp)
{
	char	**ands;
	int		i;
	int		exit;

	ands = ft_split(line, '&');//modificarlo para que parta con strings
	i = 0;
	while (ands[i])
	{
		exit = parse_pipex(ands[i], envp);
		i++;
	}
	return (exit);
}

int parse_ors(char *line, char **envp)
{
	char	**ors;
	int		i;
	int		exit;

	ors = ft_split(line, '|');//modificarlo para que parta con strings
	i = 0;
	while (ors[i])
	{
		exit = parse_ands(ors[i], envp);
		if (exit == 0)
			break ;
		i++;
	}
	return (exit);
}

pid_t		child_pid;

int new_shell(char **envp)
{	
	char				*c;
	int					exit_code;

	while (1)
	{	
		printf("pid is %d\n", getpid());
		c = readline("jgravalo> ");
		if (c == NULL)
			exit(1);
		add_history(c);
		exit_code = parse_pipex(c, envp);
		free(c);
	}
		return (exit_code);
}

void handler(int signal)
{
	if (signal == SIGINT)
	{	
		printf("child process is %d\n", child_pid);
		printf("return of kill is %d\n", kill(child_pid, SIGTERM));
	}
}

void set_signals(char **envp)
{
	sigset_t			sigset;
	struct sigaction	sa;
	pid_t				pid;
	int					status;

	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handler;
	sigaction(SIGINT, &sa, NULL);
	sigaddset(&sigset, SIGINT);
	while (1)
	{	
		printf("parent pid is %d\n", getpid());
		printf("return of waitpid is %d\n", waitpid(child_pid, &status, 0));
		perror("waitpid");
		if(WIFSIGNALED(status))
		{	
			printf("entra al newshell\n");
			write(1,"\n", 1);
			pid = fork();
			child_pid = pid;
			if (pid == 0)
				new_shell(envp);
		}
		else
		{	
			printf("entra al exit\n");
			exit(0);
		}
	}	
}

int main(int argc, char **argv, char **envp)
{	
	pid_t		pid;

	if (!argc && !argv && !envp)
		return (0);
	pid = fork();
	child_pid = pid;
	if (pid > 0)
		set_signals(envp);
	if (pid == 0)
		new_shell(envp);
}

