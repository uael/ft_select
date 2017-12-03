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

#include <sys/ioctl.h>

#include "ft_select.h"

#define SIG_ISKILL(s) ((s)==SIGINT||(s)==SIGQUIT||(s)==SIGKILL||(s)==SIGTERM)

static t_slct	g_s;

static void		slct_draw(void)
{
	int	i;
	int	x;
	int	y;

	i = g_s.beg;
	y = -1;
	while (++y < g_s.rowm && i < (int)g_s.av.len && (x = -1))
	{
		while (++x < g_s.col && i < (int)g_s.av.len)
		{
			ft_trm_puts(&g_s.trm, "[ ");
			i == g_s.i ? ft_trm_puts(&g_s.trm, ft_caps_underline()) : 0;
			g_s.sel.buf[i] ? ft_trm_puts(&g_s.trm, ft_caps_rvideo()) : 0;
			ft_trm_puts(&g_s.trm,
				access(g_s.av.buf[i], F_OK) ? "\033[31m" : "\033[32m");
			ft_trm_puts(&g_s.trm, g_s.av.buf[i]);
			ft_trm_puts(&g_s.trm, "\033[0m");
			ft_trm_puts(&g_s.trm, ft_caps_reset());
			ft_trm_putr(&g_s.trm, ' ', (size_t)(g_s.pad -
				ft_strlen(g_s.av.buf[i++]) - 3));
			ft_trm_puts(&g_s.trm, "]");
		}
		if (i < (int)g_s.av.len && y + 1 < g_s.rowm)
			ft_trm_puts(&g_s.trm, "\n");
	}
}

static int		slct_ref(void)
{
	int i;

	if (!g_s.av.len)
		return (-1);
	if (!g_s.trm.on)
		return (0);
	ft_trm_clear(&g_s.trm);
	ft_trm_refresh(&g_s.trm);
	i = -1;
	g_s.pad = 0;
	while (++i < (int)g_s.av.len)
		g_s.pad = ft_i32max(g_s.pad, (int32_t)ft_strlen(g_s.av.buf[i]));
	g_s.pad += 4;
	if ((g_s.col = g_s.trm.w / g_s.pad) == 0)
		return (0);
	g_s.row = (int)g_s.av.len / g_s.col;
	g_s.rowm = g_s.trm.h;
	g_s.beg = (g_s.i / g_s.col) >= g_s.rowm
		? ((g_s.i / g_s.col) - (g_s.rowm - 1)) * g_s.col : 0;
	slct_draw();
	ft_ostream_flush(&g_s.trm.out);
	return (1);
}

#define DO_UP(i, c, r, ac) (((i)=((i)-(c)>=0?(i)-(c):(i)+((c)*(r))))>=(ac))
#define DO_DOWN(i, c, ac) ((i)+(c)<(ac)?(i)+(c):(i)%(c))

static t_st		slct(void)
{
	int	c;

	while ((c = slct_ref()) >= 0 &&
		(c ? (c = ft_trm_getch(&g_s.trm)) : 0) >= 0)
	{
		if (c == TRK_ESCAPE)
			g_s.sel.buf[g_s.i] ^= 1;
		if (c == TRK_ESCAPE || c == TRK_TAB || c == TRK_RIGHT)
			g_s.i = g_s.i + 1 < (int)g_s.av.len ? g_s.i + 1 : 0;
		else if (c == TRK_LEFT)
			g_s.i = g_s.i >= 1 ? g_s.i - 1 : (int)g_s.av.len - 1;
		else if (c == TRK_UP && DO_UP(g_s.i, g_s.col, g_s.row, (int)g_s.av.len))
			g_s.i = g_s.i - g_s.col;
		else if (c == TRK_DOWN)
			g_s.i = DO_DOWN(g_s.i, g_s.col, (int)g_s.av.len);
		else if (c == TRK_ENTER || c == TRK_ESC)
			break ;
		else if (c == TRK_DELETE || c == TRK_BACKSPACE)
		{
			ft_vstr_remove(&g_s.av, (size_t)g_s.i, NULL);
			ft_vu8_remove(&g_s.sel, (size_t)g_s.i, NULL);
			(int)g_s.av.len && g_s.i >= (int)g_s.av.len ? --g_s.i : 0;
		}
	}
	return (c == TRK_ENTER ? OK : NOK);
}

static void		slct_sighdl(int sig)
{
	if (sig == SIGWINCH)
		slct_ref();
	else if (SIG_ISKILL(sig))
	{
		ft_trm_dtor(&g_s.trm);
		ft_vstr_dtor(&g_s.av, NULL);
		ft_vu8_dtor(&g_s.sel, NULL);
		exit(0);
	}
	else if (sig == SIGCONT)
	{
		ft_trm_on(&g_s.trm);
		signal(SIGTSTP, slct_sighdl);
		signal(SIGCONT, slct_sighdl);
		slct_ref();
	}
	else if (sig == SIGTSTP)
	{
		ft_trm_off(&g_s.trm);
		signal(SIGTSTP, SIG_DFL);
		ioctl(0, TIOCSTI, (char[2]){g_s.trm.tmp.c_cc[VSUSP], 0});
	}
}

int				main(int ac, char **av)
{
	int	i;
	int	p;

	ft_memset(&g_s, 0, sizeof(t_slct));
	if (ac <= 1 || ST_NOK(ft_trm_ctor(&g_s.trm)))
		return (ac <= 1 ? EXIT_SUCCESS : EXIT_FAILURE);
	if (!ft_vstr_pushnc(&g_s.av, (const char **)(av + 1), (size_t)(ac - 1))
		|| !ft_vu8_grow(&g_s.sel, (size_t)(ac - 1)))
		ft_fatal(ERR(errno), NULL, NULL, "%s: %e\n", "select", errno);
	ft_memset(g_s.sel.buf, i = 0, (size_t)(ac - 1));
	while (i < 32)
		signal(i++, slct_sighdl);
	if (ST_NOK(slct()))
		return (ft_dtor(EXIT_SUCCESS, (t_dtor)ft_trm_dtor, &g_s.trm, NULL));
	p = 0;
	i = -1;
	ft_trm_dtor(&g_s.trm);
	while (++i < (int)g_s.av.len)
		if (g_s.sel.buf[i] && ++p)
			ft_putf(1, "%s ", g_s.av.buf[i]);
	p ? ft_puts(1, "\b\n") : 0;
	ft_vstr_dtor(&g_s.av, NULL);
	ft_vu8_dtor(&g_s.sel, NULL);
	return (EXIT_SUCCESS);
}
