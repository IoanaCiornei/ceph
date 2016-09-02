#include <vector>
#include <stdlib.h>
#include <map>
#include <set>
#include <string>
#include <iosfwd>

#include "HaltonWrapper.h"

extern "C" {
#include "halton.h"
}

void HaltonWrapper::halton_do_mapping(int pg_id, int max_devices, unsigned replication, std::vector<int>& out)
{
	int rawout[replication];

	halton(pg_id, max_devices, replication, rawout);
	for (unsigned i = 0; i < replication; i++)
		out[i] = rawout[i];
}

