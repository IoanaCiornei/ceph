#ifdef __KERNEL__
# include <linux/string.h>
#else
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
#endif

#define HALTON_BASE 2

static int remapped_index;

static void print_placement(int *array, int size, const char *message)
{
	int i;

	fprintf(stderr, "%s : [", message);
	for (i = 0; i < size; i++)
		fprintf(stderr, " %d ", array[i]);
	fprintf(stderr, "]\n");
}

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

static int is_element_of(int elem, int *array, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		if (array[i] == elem)
			return 1;
	}
	return 0;
}

static void fix_nonexistant_osds(int *ideal_out, int replication, int max_devices, int *raw_out)
{
	int i, remapped_osd, up_index;
	int *up_osds;

	up_osds = malloc(max_devices * sizeof(int));
	if (!up_osds) {
		fprintf(stderr, "Could not alloc memory, returning\n");
		return;
	}
	for (i = 0; i < max_devices; i++)
		up_osds[i] = i;

	remapped_osd = -1;
	for (i = 0; i < replication; i++) {

		if (ideal_out[i] >= max_devices) {
			do {
				up_index = get_halton_sequence(remapped_index++, HALTON_BASE) * max_devices;
				remapped_osd = up_osds[up_index];
			} while(is_element_of(remapped_osd, ideal_out, replication));

			raw_out[i] = remapped_osd;
		} else {
			raw_out[i] = ideal_out[i];
		}
	}

	free(up_osds);
}

static void get_ideal_placement(int pg_id, int max_devices, unsigned replication, int *ideal_out)
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
		ideal_out[i] = chosen_osd;

		fprintf(stderr, "pg_id %d, max_devices %d, rep %u, unified_index %d, layout_index %u, chosen_osd %d\n",
			pg_id, max_devices, replication, unified_index, layout_index, chosen_osd);
	}

	/* free the space used by the layout */
	free(layout);
}

void halton(int pg_id, int max_devices, unsigned replication, int *raw_out)
{
	int *ideal_out;

	ideal_out = malloc(replication * sizeof(int));
	if (!ideal_out) {
		fprintf(stderr, "Coult not alloc memory, returning\n");
		return;
	}


	/* get ideal placement of the OSDs in the given PG */
	get_ideal_placement(pg_id, max_devices, replication, ideal_out);
	print_placement(ideal_out, replication, "ideal_out");

	/* remap the nonexistant OSDs */
	fix_nonexistant_osds(ideal_out, replication, max_devices, raw_out);
	print_placement(raw_out, replication, "raw_out");
}
