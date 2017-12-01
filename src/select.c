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

#define TRM_DEV "/dev/tty"
#define TRM_USG "%s: Specify a terminal type with `setenv TERM <yourtype>`\n"

typedef TTY		t_tty;

typedef struct	s_trm
{
	t_tty		tty;
	int			height;
	int			width;
}				t_trm;

t_st	ft_trm_ctor(t_trm *self)
{
	char	*tnm;

	FT_INIT(self, t_trm);
	if (!(tnm = getenv("TERM")))
		return (ft_passf(NOK, TRM_USG, "select"));
	if (tgetent(NULL, tnm) <= 0 )
		return (ft_passf(NOK, "%s: %e.\n", errno));
	self->height = tgetnum("li");
	self->width = tgetnum("co");
	tcgetattr(0, &self->tty);
	self->tty.c_lflag &= ~ICANON;
	self->tty.c_lflag &= ~ECHO;
	self->tty.c_cc[VMIN] = 1;
	self->tty.c_cc[VTIME] = 100;
	return (OK);
}

typedef enum	s_trm_mv
{
	TRM_UP = 0,
	TRM_DOWN = 2,
	TRM_LEFT = 4,
	TRM_RIGHT = 6
}				t_trm_mv;

static char		*g_trm_mvs[8] =
{
	[TRM_UP] = "up",
	[TRM_UP + 1] = "UP",
	[TRM_DOWN] = "do",
	[TRM_DOWN + 1] = "DO",
	[TRM_LEFT] = "le",
	[TRM_LEFT + 1] = "LE",
	[TRM_RIGHT] = "nd",
	[TRM_RIGHT + 1] = "RI"
};

void	ft_trm_mv(t_trm_mv mv, int n)
{
	if (n == 1)
		tputs(tgetstr(g_trm_mvs[mv], NULL), 1, putchar);
	else if (n)
		tputs(tgoto(tgetstr(g_trm_mvs[mv + 1], NULL), 0, n), 1, putchar);
}

void	ft_trm_goto_x(int x)
{
	tputs(tgoto(tgetstr("ch", NULL), 0, x), 1, putchar);
}

void	ft_trm_goto_y(int y)
{
	tputs(tgoto(tgetstr("cv", NULL), 0, y), 1, putchar);
}

void	select_print(t_trm *t, int ac, char **av, int pad)
{
	int i;
	int j;
	int k;

	i = 0;
	k = 0;
	pad += 4;
	while (i < t->height && k < ac - 1)
	{
		j = 0;
		while (j < (t->width / pad) && k < ac - 1)
		{
			ft_trm_goto_x(j * pad);
			tputs("[ ", 1, putchar);
			ft_trm_goto_x((j * pad) + 2);
			tputs(av[++k], 1, putchar);
			ft_trm_goto_x((j * pad) + pad - 1);
			tputs("]", 1, putchar);
			++j;
		}
		ft_trm_mv(TRM_DOWN, 1);
		++i;
	}
}

int		main(int ac, char **av)
{
	t_trm		trm;

	if (ST_NOK(ft_trm_ctor(&trm)))
		return (EXIT_FAILURE);
	select_print(&trm, ac, av, 15);
	return (EXIT_SUCCESS);
}
