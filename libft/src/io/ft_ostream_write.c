/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ostream_write.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:33 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/23 07:21:44 by null             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/io/ostream.h"

inline t_sz	ft_ostream_write(t_ostream *self, char const *sr, size_t len)
{
	if (self->kind == OSTREAM_FILE)
		return (ft_ofstream_write(&self->u.file, sr, len));
	return (ft_omstream_write(&self->u.mem, sr, len));
}

t_sz		ft_ostream_writef(t_ostream *self, char const *fmt, ...)
{
	va_list	ap;
	t_sz	sz;

	va_start(ap, fmt);
	sz = ft_ostream_vwritef(self, fmt, ap);
	va_end(ap);
	return (sz);
}

inline t_sz	ft_ostream_vwritef(t_ostream *self, char const *fmt, va_list ap)
{
	if (self->kind == OSTREAM_FILE)
		return (ft_ofstream_vwritef(&self->u.file, fmt, ap));
	return (ft_omstream_vwritef(&self->u.mem, fmt, ap));
}
