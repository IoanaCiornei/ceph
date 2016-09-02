#ifdef __KERNEL__
# include <linux/string.h>
#else
# include <stdio.h>
#endif


void halton()
{
	fprintf(stderr, "Ioana are mere\n");
}

int get_A003602(int index)
{
	if (index % 2 == 1)
		return ((index + 1) / 2);
	else
		return get_A003602(index/2);
}

