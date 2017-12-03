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
		while (++x < g_s.mx && i < (int) g_s.av.len)
		{
			ft_trm_puts(&g_s.trm, "[ ");
			i == g_s.cur ? ft_trm_puts(&g_s.trm, ft_caps_underline()) : 0;
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
	ft_trm_refresh(&g_s.trm);
	ft_trm_clear(&g_s.trm);
	g_s.pad = 15 + 4;
	g_s.mx = g_s.trm.w / g_s.pad;
	g_s.my = g_s.trm.h;
	g_s.beg = (g_s.cur / g_s.mx) >= g_s.my
		? ((g_s.cur / g_s.mx) - (g_s.my - 1)) * g_s.mx : 0;
	slct_draw();
	ft_ostream_flush(&g_s.trm.out);
	return (1);
}

static t_st		slct(void)
{
	int	ch;

	ft_trm_puts(&g_s.trm, tgetstr("ti", NULL));
	ft_trm_puts(&g_s.trm, tgetstr("vi", NULL));
	while (slct_refresh() && (ch = ft_trm_getch(&g_s.trm)) > 0)
	{
		if (ch == TRM_K_ESCAPE)
			g_s.sel.buf[g_s.cur] ^= 1;
		if (ch == TRM_K_ESCAPE || ch == TRM_K_TAB || ch == TRM_K_RIGHT)
			g_s.cur = g_s.cur + 1 < (int)g_s.av.len ? g_s.cur + 1 : 0;
		else if (ch == TRM_K_LEFT)
			g_s.cur = g_s.cur >= 1 ? g_s.cur - 1 : (int)g_s.av.len - 1;
		else if (ch == TRM_K_UP && g_s.cur - g_s.mx >= 0)
			g_s.cur -= g_s.mx;
		else if (ch == TRM_K_DOWN || ch == TRM_K_UP)
			g_s.cur = g_s.cur + g_s.mx < (int)g_s.av.len
				? g_s.cur + g_s.mx : g_s.cur % g_s.mx;
		else if (ch == TRM_K_ENTER)
			break ;
		else if (ch == TRM_K_DELETE || ch == TRM_K_BACKSPACE)
			ft_vstr_remove(&g_s.av, (size_t)g_s.cur, NULL);
		else if (ch == TRM_K_ESC)
			return (NOK);
	}
	return (OK);
}

static void		slct_sighdl(int sig)
{
	if (sig == SIGTSTP || SIG_ISKILL(sig))
		ft_trm_dtor(&g_s.trm);
	else if (sig == SIGCONT)
	{
		signal(SIGTSTP, slct_sighdl);
		signal(SIGCONT, slct_sighdl);
		ft_trm_ctor(&g_s.trm);
	}
	SIG_ISKILL(sig) ? exit(0) : 0;
	if (sig == SIGTSTP)
	{
		signal(SIGTSTP, SIG_DFL);
		return ;
	}
	else if (sig != SIGCONT && sig != SIGWINCH)
		return ;
	slct_refresh();
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
	return (EXIT_SUCCESS);
}
