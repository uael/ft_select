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

#define TRM_USG "%s: Specify a terminal type with `setenv TERM <yourtype>`\n"

typedef TTY		t_tty;

typedef struct	s_trm
{
	t_tty		tty;
	t_tty		tmp;
	int			h;
	int			w;
	int			x;
	int 		y;
	t_ostream	out;
	t_du8		in;
}				t_trm;

static int	trm_cursor(int tty, int *col, int *row)
{
	uint8_t c;
	t_st	st;

	if (tty == -1)
		return ENOTTY;
	write(tty, "\033[6n", 4);
	st = EIO;
	if (read(tty, &c, 1) <= 0 || c != 27 || read(tty, &c, 1) <= 0 || c != '[')
		return (st);
	*row = 0;
	while (read(tty, &c, 1) > 0 && c >= '0' && c <= '9')
		*row = 10 * *row + c - '0';
	*row ? --*row : 0;
	if (c != ';')
		return (st);
	*col = 0;
	while (read(tty, &c, 1) > 0 && c >= '0' && c <= '9')
		*col = 10 * *col + c - '0';
	*col ? --*col : 0;
	if (c != 'R')
		return (st);
	return (0);
}

t_st	ft_trm_ctor(t_trm *self)
{
	char		*tnm;
	const char	*dev;

	FT_INIT(self, t_trm);
	if (!(tnm = getenv("TERM")))
		return (ft_passf(NOK, TRM_USG, "select"));
	if (!(dev = ttyname(STDIN_FILENO)))
		dev = ttyname(STDOUT_FILENO);
	if (!dev && !(dev = ttyname(STDERR_FILENO)))
		return ERR(errno = ENOTTY);
	if (tgetent(NULL, tnm) <= 0 || ST_NOK(ft_ostream_open(&self->out, dev)))
		return (ft_passf(NOK, "%s: %e.\n", errno));
	ft_du8_ctor(&self->in);
	self->h = tgetnum("li");
	self->w = tgetnum("co");
	tcgetattr(0, &self->tty);
	ft_memcpy(&self->tmp,  &self->tty, sizeof(t_tty));
	self->tty.c_lflag &= ~ICANON;
	self->tty.c_lflag &= ~ECHO;
	self->tty.c_cc[VMIN] = 1;
	self->tty.c_cc[VTIME] = 100;
	tcsetattr(0, TCSANOW, &self->tty);
	ft_ostream_flush(&self->out);
	trm_cursor(self->out.u.file.fd, &self->x, &self->y);
	return (OK);
}

void	ft_trm_refresh(t_trm *self)
{
	tcgetattr(0, &self->tty);
	self->h = tgetnum("li");
	self->w = tgetnum("co");
}

void	ft_trm_clear(t_trm *self)
{
	ft_ostream_puts(&self->out, tgoto(tgetstr("cm", NULL), self->x, self->y));
	ft_ostream_puts(&self->out, tgetstr("cd", NULL));
	ft_ostream_flush(&self->out);
}

void	ft_trm_dtor(t_trm *self)
{
	ft_ostream_puts(&self->out, tgetstr("ve", NULL));
	ft_trm_clear(self);
	tcsetattr(0, TCSANOW, &self->tmp);
	ft_du8_dtor(&self->in, NULL);
}

#define TRM_K_TAB 9
#define TRM_K_ENTER 10
#define TRM_K_ESC 27
#define TRM_K_ESCAPE 32
#define TRM_K_BACKSPACE 127
#define TRM_K_DELETE 126
#define TRM_K_UP 65
#define TRM_K_DOWN 66
#define TRM_K_RIGHT 67
#define TRM_K_LEFT 68

#define TRK_ONEK(ch) ((ch)==9||(ch)==10||(ch)==32||(ch)==127||(ch)==27)

int		ft_trm_getch(t_trm *t)
{
	uint8_t	c[2];
	uint8_t	n;
	int	r;

	n = 0;
	ft_memset(c, 0, 2 * sizeof(char));
	while (42)
		if (!ft_du8_size(&t->in) && ((r = (int)read(0, &c, 2)) < 0 ||
			(r && !ft_du8_pushnc(&t->in, c, (size_t)r))))
			return (ENO);
		else if (ft_du8_popn(&t->in, 2, c) <= 0)
			break ;
		else if ((n += r) == 1 && TRK_ONEK(c[0]))
			return (c[0]);
		else if (n == 2 && c[0] == 27 && c[1] == 91)
			continue ;
		else if (n == 4 && c[0] == 51 && c[1] == 126)
			return (c[1]);
		else if (n == 3 && (c[0] >= 65 && c[0] <= 68))
			return (c[0]);
		else
			break ;
	t->in.cur -= n;
	return (0);
}

char	*ft_caps_gotox(int x)
{
	return (tgoto(tgetstr("ch", NULL), 0, x));
}

char	*ft_caps_underline(void)
{
	return (tgetstr("us", NULL));
}

char	*ft_caps_rvideo(void)
{
	return (tgetstr("mr", NULL));
}

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
		ft_ostream_puts(&t->out, tgetstr("me", NULL));
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
			ft_ostream_puts(&t->out, tgetstr("do", NULL));
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
