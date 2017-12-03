/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft/trm.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:30 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/23 14:38:40 by null             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_TRM_H
# define LIBFT_TRM_H

# include <term.h>

# include "io.h"
# include "lib.h"

# define TRM_K_TAB 9
# define TRM_K_ENTER 10
# define TRM_K_ESC 27
# define TRM_K_ESCAPE 32
# define TRM_K_BACKSPACE 127
# define TRM_K_DELETE 126
# define TRM_K_UP 65
# define TRM_K_DOWN 66
# define TRM_K_RIGHT 67
# define TRM_K_LEFT 68

extern char		*ft_caps_gotox(int x);
extern char		*ft_caps_underline(void);
extern char		*ft_caps_rvideo(void);
extern char		*ft_caps_reset(void);
extern char		*ft_caps_up(int n);

typedef TTY		t_trmios;

typedef struct	s_trm
{
	t_trmios	tty;
	t_trmios	tmp;
	int			h;
	int			w;
	t_ostream	out;
	t_du8		in;
	t_bool		on;
}				t_trm;

extern t_st		ft_trm_ctor(t_trm *self);
extern void		ft_trm_dtor(t_trm *self);
extern void		ft_trm_refresh(t_trm *self);
extern void		ft_trm_clear(t_trm *self);
extern int		ft_trm_getch(t_trm *self);
extern t_sz		ft_trm_puts(t_trm *self, char const *s);
extern ssize_t	ft_trm_putr(t_trm *self, char c, size_t n);
extern void		ft_trm_on(t_trm *self);
extern void		ft_trm_off(t_trm *self);

#endif
