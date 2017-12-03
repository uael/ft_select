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
	int	k;

	i = g_s.beg;
	y = -1;
	while (++y < g_s.my && i < (int) g_s.av.len && (x = -1))
	{
		while (++x < g_s.c && i < (int) g_s.av.len)
		{
			ft_trm_puts(&g_s.trm, "[ ");
			i == g_s.i ? ft_trm_puts(&g_s.trm, ft_caps_underline()) : 0;
			g_s.sel.buf[i] ? ft_trm_puts(&g_s.trm, ft_caps_rvideo()) : 0;
			ft_trm_puts(&g_s.trm, g_s.av.buf[i++]);
			ft_trm_puts(&g_s.trm, ft_caps_reset());
			k = -1;
			while (++k < g_s.pad - 3 - (int)ft_strlen(g_s.av.buf[i - 1]))
				ft_trm_puts(&g_s.trm, " ");
			ft_trm_puts(&g_s.trm, "]");
		}
		if (i < (int) g_s.av.len && y + 1 < g_s.my)
			ft_trm_puts(&g_s.trm, "\n");
	}
}

static int 		slct_refresh(void)
{
	int i;

	if (!g_s.av.len)
		return (0);
	if (!g_s.trm.on)
		return (1);
	ft_trm_refresh(&g_s.trm);
	ft_trm_clear(&g_s.trm);
	i = -1;
	g_s.pad = 0;
	while (++i < (int)g_s.av.len)
		g_s.pad = ft_i32max(g_s.pad, (int32_t)ft_strlen(g_s.av.buf[i]));
	g_s.pad += 4;
	g_s.c = g_s.trm.w / g_s.pad;
	g_s.my = g_s.trm.h;
	g_s.beg = (g_s.i / g_s.c) >= g_s.my
		? ((g_s.i / g_s.c) - (g_s.my - 1)) * g_s.c : 0;
	slct_draw();
	ft_ostream_flush(&g_s.trm.out);
	return (1);
}

static t_st		slct(void)
{
	int	ch;

	while (slct_refresh() && (ch = ft_trm_getch(&g_s.trm)) >= 0)
	{
		if (ch == TRM_K_ESCAPE)
			g_s.sel.buf[g_s.i] ^= 1;
		if (ch == TRM_K_ESCAPE || ch == TRM_K_TAB || ch == TRM_K_RIGHT)
			g_s.i = g_s.i + 1 < (int)g_s.av.len ? g_s.i + 1 : 0;
		else if (ch == TRM_K_LEFT)
			g_s.i = g_s.i >= 1 ? g_s.i - 1 : (int)g_s.av.len - 1;
		else if (ch == TRM_K_UP && g_s.i - g_s.c >= 0)
			g_s.i -= g_s.c;
		else if (ch == TRM_K_DOWN || ch == TRM_K_UP)
			g_s.i = g_s.i + g_s.c < (int)g_s.av.len ? g_s.i + g_s.c :
				g_s.i % g_s.c;
		else if (ch == TRM_K_ENTER || ch == TRM_K_ESC)
			break ;
		else if (ch == TRM_K_DELETE || ch == TRM_K_BACKSPACE)
		{
			ft_vstr_remove(&g_s.av, (size_t)g_s.i, NULL);
			ft_vu8_remove(&g_s.sel, (size_t)g_s.i, NULL);
			(int)g_s.av.len && g_s.i >= (int)g_s.av.len ? --g_s.i : 0;
		}
	}
	return (ch == TRM_K_ENTER ? OK : NOK);
}

static void		slct_sighdl(int sig)
{
	if (sig == SIGWINCH)
		slct_refresh();
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
		slct_refresh();
	}
	else if (sig == SIGTSTP)
	{
		ft_trm_off(&g_s.trm);
		signal(SIGTSTP, SIG_DFL);
		ioctl(0, TIOCSTI, (char [2]){g_s.trm.tmp.c_cc[VSUSP], 0});
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
	while (i < 33)
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
