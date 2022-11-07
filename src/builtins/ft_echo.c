/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_echo.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: fpurdom <fpurdom@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/20 14:08:02 by fpurdom       #+#    #+#                 */
/*   Updated: 2022/11/03 14:56:15 by fpurdom       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "libft.h"
#include <unistd.h>
#include <stdbool.h>

int	ft_echo(char **strs)
{
	bool	n_option;

	n_option = false;
	while (!ft_strncmp(*strs, "-n", 3))
	{
		n_option = true;
		strs++;
	}
	while (*strs)
	{
		ft_putstr_fd(*strs, 1);
		if (*(strs + 1))
			ft_putchar_fd(' ', 1);
		strs++;
	}
	if (n_option == false)
		ft_putchar_fd('\n', 1);
	g_code = 0;
	return (1);
}
