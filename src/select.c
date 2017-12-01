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
	t_ostream	out;
}				t_trm;

t_st	ft_trm_ctor(t_trm *self)
{
	char	*tnm;

	FT_INIT(self, t_trm);
	if (!(tnm = getenv("TERM")))
		return (ft_passf(NOK, TRM_USG, "select"));
	if (tgetent(NULL, tnm) <= 0 || ST_NOK(ft_ostream_open(&self->out, TRM_DEV)))
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

void	ft_trm_mv(t_trm *t, t_trm_mv mv, int n)
{
	if (n == 1)
		ft_ostream_puts(&t->out, tgetstr(g_trm_mvs[mv], NULL));
	else if (n)
		ft_ostream_puts(&t->out, tgoto(tgetstr(g_trm_mvs[mv + 1], NULL), 0, n));
}

void	ft_trm_goto_x(t_trm *t, int x)
{
	ft_ostream_puts(&t->out, tgoto(tgetstr("ch", NULL), 0, x));
}

void	ft_trm_goto_y(t_trm *t, int y)
{
	ft_ostream_puts(&t->out, tgoto(tgetstr("cv", NULL), 0, y));
}

void	select_draw(t_trm *t, t_vstr *av, int pad, int select)
{
	int			y;
	int			x;
	uint32_t	k;

	y = 0;
	k = 0;
	pad += 4;
	while (y < t->height - 1 && k < av->len - (x = 0))
	{
		while (x < (t->width / pad) && k < av->len)
		{
			ft_trm_goto_x(t, x * pad);
			ft_ostream_puts(&t->out, "[ ");
			ft_trm_goto_x(t, (x * pad) + 2);
			ft_ostream_puts(&t->out, av->buf[k++]);
			ft_trm_goto_x(t, (x * pad) + pad - 1);
			ft_ostream_puts(&t->out, "]");
			++x;
		}
		if (y < t->height - 1 && k < av->len && ++y)
			ft_trm_mv(t, TRM_DOWN, 1);
	}
	ft_ostream_puts(&t->out, tgoto(tgetstr("UP", NULL), 0, y));
	ft_trm_goto_x(t, (--select % (t->width / pad)) * pad);
	ft_trm_goto_y(t, select / (t->width / pad));
	ft_ostream_flush(&t->out);
}

int		main(int ac, char **av)
{
	t_trm		trm;
	t_vstr		avv;
	int			i;

	if (ST_NOK(ft_trm_ctor(&trm)))
		return (EXIT_FAILURE);
	ft_vstr_ctor(&avv);
	i = 0;
	while (++i < ac)
		if (!ft_vstr_pushc(&avv, av[i]))
			ft_fatal(ERR(errno), NULL, NULL, "%s: %e\n", "select", errno);
	select_draw(&trm, &avv, 15, 3);
	read(0, NULL, 1);
	return (EXIT_SUCCESS);
}
