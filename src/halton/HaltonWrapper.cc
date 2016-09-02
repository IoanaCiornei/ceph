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

int HaltonWrapper::halton_do_mapping(int pg_id, unsigned replication, std::vector<int>& out)
{
		halton();

		int x = get_A003602(1);
		return x;
}

