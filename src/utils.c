#include "hardware_connect.h"


//string manipulation function
int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i])
	{
		i++;
	}
	if (n == i)
	{
		return (0);
	}
	return (s1[i] - s2[i]);
}

double ft_atof(const char *str)
{
    double result = 0.0;
    double factor = 1.0;
    int sign = 1;

    if (!str)
        return 0.0;

    while (*str == ' ' || (*str >= 9 && *str <= 13))
        str++;

    if (*str == '-' || *str == '+')
    {
        if (*str == '-')
            sign = -1;
        str++;
    }

    while (*str >= '0' && *str <= '9')
    {
        result = result * 10.0 + (*str - '0');
        str++;
    }

    if (*str == '.')
    {
        str++;
        while (*str >= '0' && *str <= '9')
        {
            factor *= 0.1;
            result += (*str - '0') * factor;
            str++;
        }
    }

    return result * sign;
}


// void	*ft_memcpy(void *dest, const void *src, size_t n)
// {
// 	unsigned char		*d;
// 	const unsigned char	*s;
// 	size_t				i;

// 	d = dest;
// 	s = src;
// 	if (d == NULL && s == NULL)
// 		return (NULL);
// 	i = 0;
// 	while (i < n)
// 	{
// 		d[i] = s[i];
// 		i++;
// 	}
// 	return (dest);
// }


void	*ft_memcpy_idx(void *dest, const void *src, size_t start, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;
	size_t				i;

	d = dest;
	s = (const unsigned char *)src + start; 
	
	if (d == NULL || src == NULL)
		return (NULL);

	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (dest);
}