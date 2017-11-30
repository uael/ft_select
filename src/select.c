/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:30 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/17 09:57:12 by null             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>
#include <term.h>
#include <termios.h>
#include <curses.h>
#include <sys/ioctl.h>

typedef enum	e_caps
{
	CAPS_CL = 0,
	CAPS_CM,
	CAPS_SO,
	CAPS_SE,
	CAPS_KR,
	CAPS_VI,
	CAPS_TI
}				t_caps;

typedef TTY		t_termios;
typedef struct winsize	t_wsz;

typedef struct	s_term
{
	char		*caps[7];
	t_termios	ios;
	t_wsz		wsz;
}				t_term;

t_st	ft_term_ctor(t_term *self)
{
	char *term;

	FT_INIT(self, t_term);
	if (!(term = getenv("TERM")))
		return (ft_ret(NOK, "%s: Set the env TERM var\n", "select"));
	if (tgetent(NULL, term) == -1)
		return (ft_ret(NOK, "%s: %e\n", "select", errno));
	self->caps[CAPS_CL] = tgetstr("cl", NULL);
	self->caps[CAPS_CM] = tgetstr("cm", NULL);
	self->caps[CAPS_SO] = tgetstr("so", NULL);
	self->caps[CAPS_SE] = tgetstr("se", NULL);
	self->caps[CAPS_KR] = tgetstr("kr", NULL);
	self->caps[CAPS_VI] = tgetstr("vi", NULL);
	self->caps[CAPS_TI] = tgetstr("ti", NULL);
	tcgetattr(0, &self->ios);
	self->ios.c_lflag &= ~ICANON;
	self->ios.c_lflag &= ~ECHO;
	self->ios.c_cc[VMIN] = 1;
	self->ios.c_cc[VTIME] = 100;
	return (OK);
}

int		main(int ac, char **av)
{
	int		i;
	t_term	term;
	char	buf[4];

	if (ST_NOK(ft_term_ctor(&term)))
		return (EXIT_FAILURE);
	tputs(term.caps[CAPS_VI], 1, putchar);
	tputs(term.caps[CAPS_TI], 1, putchar);
	while (1)
	{
		tputs(term.caps[CAPS_CL], 1, putchar);
		i = 0;
		while (++i < ac)
		{
			ft_puts(1, av[i]);
			ft_putc(1, '\n');
		}
		bzero(buf, 4);
		read(0, buf, 4);
		ioctl(0, TIOCGWINSZ, &term.wsz);
	}
}
