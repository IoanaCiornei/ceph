#ifndef CEPH_HALTON_WRAPPER_H
#define CEPH_HALTON_WRAPPER_H

#include <vector>

class HaltonWrapper {

public:
	static int halton_do_mapping(int pg_id, unsigned replication, std::vector<int>& out);
};

#endif
