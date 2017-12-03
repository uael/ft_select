/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:30 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/17 09:57:12 by null             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SELECT_H
# define FT_SELECT_H

#include <libft.h>
#include <term.h>

typedef struct	s_slct
{
	t_trm		trm;
	t_vstr		av;
	t_vu8		sel;
	int			c;
	int			my;
	int			beg;
	int			i;
	int 		pad;
}				t_slct;

#endif
