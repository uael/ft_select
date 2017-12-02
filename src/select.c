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

void	select_draw_input(t_trm *t, t_vstr *av, int *i, int const *sel)
{
	int		x;

	x = 0;
	while (x < (t->w / i[0]) && i[2] < (int)av->len)
	{
		ft_ostream_puts(&t->out, "[ ");
		if (i[2] == i[1])
			ft_ostream_puts(&t->out, ft_caps_underline());
		if (sel[i[2]])
			ft_ostream_puts(&t->out, ft_caps_rvideo());
		ft_ostream_puts(&t->out, av->buf[i[2]++]);
		ft_ostream_puts(&t->out, ft_caps_reset());
		ft_ostream_puts(&t->out, ft_caps_gotox((x * i[0]) + i[0] - 1));
		ft_ostream_puts(&t->out, "]");
		++x;
	}
}

void	select_draw(t_trm *t, t_vstr *av, int *i, int const *sel)
{
	int		mx;
	int		my;
	int		y;

	ft_trm_refresh(t);
	ft_trm_clear(t);
	i[y = 0] += 4;
	mx = t->w / i[0];
	my = (int)av->len / mx > t->h - t->y ? t->h - t->y : (int)av->len / mx;
	i[2] = (i[1] / mx) >= my ? (uint32_t)((i[1] / mx) - (my - 1)) * mx : 0;
	while (y < my && i[2] < (int)av->len)
	{
		select_draw_input(t, av, i, sel);
		if (i[2] < (int)av->len && ++y < my)
			ft_ostream_puts(&t->out, ft_caps_down());
	}
	ft_ostream_flush(&t->out);
}

int		main(int ac, char **av)
{
	t_trm		t;
	t_vstr		avv;
	int			i;
	int			ch;
	int			col;
	int			sel[ac];

	if (ST_NOK(ft_trm_ctor(&t)))
		return (EXIT_FAILURE);
	ft_vstr_ctor(&avv);
	i = 0;
	while (++i < ac)
		if (!ft_vstr_pushc(&avv, av[i]))
			ft_fatal(ERR(errno), NULL, NULL, "%s: %e\n", "select", errno);
	col = t.w / (15 + 4);
	ft_memset(sel, 0, (size_t)ac);
	ft_ostream_puts(&t.out, tgetstr("vi", NULL));
	select_draw(&t, &avv, (int [3]){15, i = 0, 0}, sel);
	while ((ch = ft_trm_getch(&t)) >= 0)
	{
		if (ch == TRM_K_ESCAPE)
			sel[i] ^= 1;
		if (ch == TRM_K_ESCAPE || ch == TRM_K_TAB || ch == TRM_K_RIGHT)
			i = i + 1 < (int)avv.len ? i + 1 : 0;
		else if (ch == TRM_K_LEFT)
			i = i >= 1 ? i - 1 : (int)avv.len - 1;
		else if (ch == TRM_K_UP && i - col >= 0)
			i -= col;
		else if (ch == TRM_K_DOWN || ch == TRM_K_UP)
			i = i + col < (int)avv.len ? i + col : i % col;
		else if (ch == TRM_K_ENTER)
			break ;
		else if (ch == TRM_K_DELETE || ch == TRM_K_BACKSPACE)
			ft_vstr_remove(&avv, (size_t)i, NULL);
		else if (ch == TRM_K_ESC)
			return (ft_dtor(1, (t_dtor)ft_trm_dtor, &t, NULL));
		select_draw(&t, &avv, (int [3]){15, i, 0}, sel);
	}

	ft_trm_dtor(&t);
	i = -1;
	ch = 0;
	while (++i < (int)avv.len)
		if (sel[i] && ++ch)
			ft_putf(1, "%s ", avv.buf[i]);
	if (ch)
		ft_puts(1, "\b\n");
	ft_vstr_dtor(&avv, NULL);
	return (EXIT_SUCCESS);
}
