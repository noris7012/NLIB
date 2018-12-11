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

struct NLIBRecv
{
	Buffer* buffer;
	uint32_t length;
};

#endif