#ifdef __KERNEL__
# include <linux/string.h>
#else
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
#endif

#define HALTON_BASE 2

static int get_A003602(int index)
{
	if (index % 2 == 1)
		return ((index + 1) / 2);
	else
		return get_A003602(index/2);
}

static void get_layout(int size, int *layout)
{
	int i, start;

	start = size;
	for (i = 0; i < size; i++)
		layout[i] = get_A003602(start++) - 1;
}

static float get_halton_sequence(int i, int base)
{
	float result = 0;
	float f = 1;
	int index = i;

	while (index > 0) {
		f = f / base;
		result = result + f * (index % base);
		index = (int) index / base;
	}
	return result;
}

static int get_order(int max_devices)
{
	int order = 0;
	int value = 1;

	do {
		value = 2 * value;
		order++;
	} while (value < max_devices);

	return order;
}

static int get_layout_size(int order)
{
	return pow(2, order);
}

void halton(int pg_id, int max_devices, unsigned replication, int *rawout)
{
	int order, layout_size, unified_index, chosen_osd;
	unsigned i, layout_index;
	int *layout;

	/* compute the layout order */
	order = get_order(max_devices);
	layout_size = get_layout_size(order);

	/* alloc memory for the layout array */
	layout = malloc(layout_size * sizeof(int));
	if (!layout) {
		fprintf(stderr, "Could not alloc memory, returning\n");
		return;
	}

	/* create the fractal layout */
	get_layout(layout_size, layout);

	for (i = 0; i < replication; i++) {
		unified_index = pg_id * replication + i;

		layout_index = get_halton_sequence(unified_index, HALTON_BASE) * layout_size;

		chosen_osd = layout[layout_index];
		rawout[i] = chosen_osd;

		fprintf(stderr, "pg_id %d, max_devices %d, rep %u, unified_index %d, layout_index %u, chosen_osd %d\n",
			pg_id, max_devices, replication, unified_index, layout_index, chosen_osd);
	}

	fprintf(stderr, "PG %d : [", pg_id);
	for (i = 0; i < replication; i++)
		fprintf(stderr, " %d ", rawout[i]);
	fprintf(stderr, "]\n");

	/* free the space used by the layout */
	free(layout);
}
