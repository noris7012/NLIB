#ifndef NLIB_NETWORK_STRUCT_H
#define NLIB_NETWORK_STRUCT_H

#include "NetworkDefine.h"
#include <memory>

struct S_Recv
{
	char data[MAX_MTU_SIZE];
	int length;
};

using S_Recv_Ptr = std::shared_ptr<S_Recv>;

struct S_Send
{
	char data[MAX_MTU_SIZE];
	int length;
};

using S_Send_Ptr = std::shared_ptr<S_Send>;

struct Buffer
{
	byte data[MAX_MTU_SIZE];
};

struct NLIBAddress
{
	uint32_t ip;
	uint16_t port;

	uint64_t id() {
		uint64_t ret = 0;
		ret += ((uint64_t)ip) << 32;
		ret += port;

		return ret;
	}
};

struct NLIBRecv
{
	Buffer* buffer;
	uint32_t length;
	NLIBAddress address;
};

#endif