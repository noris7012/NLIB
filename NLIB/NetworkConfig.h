#ifndef NLIB_NETWORK_CONFIG_H
#define NLIB_NETWORK_CONFIG_H

#include <stdint.h>

#include "NetworkDefine.h"

struct NetworkConfig
{
	E_TRANSPORT_TYPE transport_type;
	const char* host;
	uint32_t port;
};

#endif