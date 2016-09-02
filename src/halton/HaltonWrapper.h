#ifndef CEPH_HALTON_WRAPPER_H
#define CEPH_HALTON_WRAPPER_H

#include <vector>

class HaltonWrapper {

public:
	static void halton_do_mapping(int pg_id, int max_devices, unsigned replication, std::vector<int>& out);
};

#endif
