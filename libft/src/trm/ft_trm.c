/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_trm.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:11 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:37:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/ioctl.h>

#include "libft/trm.h"

#define TRM_USG "%s: Specify a terminal type with `setenv TERM <yourtype>`\n"

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

t_st		ft_trm_ctor(t_trm *self)
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
	tcgetattr(self->out.u.file.fd, &self->tty);
	ft_memcpy(&self->tmp,  &self->tty, sizeof(t_trmios));
	self->tty.c_lflag &= ~ICANON;
	self->tty.c_lflag &= ~ECHO;
	self->tty.c_cc[VMIN] = 1;
	self->tty.c_cc[VTIME] = 100;
	tcsetattr(self->out.u.file.fd, TCSANOW, &self->tty);
	ft_trm_refresh(self);
	ft_trm_register(self);
	return (OK);
}

void		ft_trm_dtor(t_trm *self)
{
	ft_ostream_puts(&self->out, tgetstr("te", NULL));
	ft_ostream_puts(&self->out, tgetstr("ve", NULL));
	ft_trm_clear(self);
	tcsetattr(self->out.u.file.fd, TCSANOW, &self->tmp);
	ft_ostream_close(&self->out);
	ft_du8_dtor(&self->in, NULL);
	FT_INIT(self, t_trm);
}

void		ft_trm_register(t_trm *self)
{
	trm_cursor(self->out.u.file.fd, &self->x, &self->y);
}

inline void	ft_trm_refresh(t_trm *self)
{
	struct winsize	w;

	ft_trm_register(self);
	if (!isatty(self->out.u.file.fd))
		return ;
	if (ioctl(self->out.u.file.fd, TIOCGWINSZ, &w))
		ft_fatal(ERR(errno), NULL, NULL, "ioctl: %e", errno);
	self->w = w.ws_col;
	self->h = w.ws_row;
}

inline void	ft_trm_clear(t_trm *self)
{
	ft_ostream_puts(&self->out, tgetstr("cl", NULL));
}
