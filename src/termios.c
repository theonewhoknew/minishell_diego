#include <termios.h>
#include <unistd.h>
#include "../inc/minishell.h"

void set_termios()
{
	struct termios term1;

	printf("entra en termios\n");
  	if (tcgetattr(STDOUT_FILENO, &term1) != 0)
    	perror("tcgetattr() error");
  	else 
	{
        term1.c_lflag     &=  ~ECHO;
    	if (tcsetattr(STDIN_FILENO, TCSANOW, &term1) != 0)
      		perror("tcsetattr() error");
    	if (tcgetattr(STDIN_FILENO, &term1) != 0)
      		perror("tcgetattr() error");
  }
}