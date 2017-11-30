/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:11 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:37:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/str.h"

inline void	ft_bzero(void *ptr, size_t n)
{
	uint8_t *str;

	str = (uint8_t *)ptr;
	while (n--)
		*str++ = '\0';
}
