/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   executor.c                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: gwinnink <gwinnink@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/09/22 16:29:32 by fpurdom       #+#    #+#                 */
/*   Updated: 2022/11/21 18:07:00 by fpurdom       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "enomem.h"
#include "libft.h"
#include "executor_utils.h"
#include "executor.h"
#include "heredoc.h"
#include "signals.h"

static void	free_pipes(t_pipe *pipes)
{
	free(pipes->pid);
	free(pipes);
}

static int	wait_forks(t_pipe *pipes, t_cmd *cmds)
{
	int	i;
	int	status;

	i = 0;
	while (i <= pipes->i)
	{
		if (waitpid(pipes->pid[i], &status, 0) == -1)
			return (3);
		i++;
	}
	free_pipes(pipes);
	if (rm_heredoc_files(cmds))
		return (3);
	suppress_sig_output();
	signal(SIGINT, sig_func_parent);
	if (WIFEXITED(status))
		return (check_memory(&status, cmds));
	if (WIFSIGNALED(status))
		return (sig_func_child(WTERMSIG(status)));
	return (0);
}

static t_pipe	*init_pipe(int size)
{
	t_pipe	*pipes;

	pipes = (t_pipe *)malloc(1 * sizeof(t_pipe));
	if (!pipes)
		exit (ENOMEM);
	pipes->pid = (int *)malloc((size) * sizeof(int));
	if (!pipes->pid)
		exit (ENOMEM);
	pipes->in_fd = STDIN_FILENO;
	pipes->i = -1;
	return (pipes);
}

int	executor(t_parser *parser, t_env **env)
{
	t_cmd		*command;
	t_pipe		*pipes;
	int			error;

	if (!parser->cmds->next && dont_fork(parser->cmds, env))
		return (g_code);
	pipes = init_pipe(parser->count);
	command = parser->cmds;
	while (command)
	{
		if (command->next)
			if (pipe(pipes->tube))
				return (3);
		error = check_heredoc(command, *env);
		if (error)
			return (free_pipes(pipes), error);
		if (do_fork(command, pipes, env))
			return (3);
		if ((!command->lst_cmd && close(pipes->tube[1]))
			|| (!command->frst_cmd && close(pipes->in_fd)))
			return (3);
		pipes->in_fd = pipes->tube[0];
		command = command->next;
	}
	return (wait_forks(pipes, parser->cmds));
}
