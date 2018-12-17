#ifndef NLIB_RELIABLE_CLIENT_H
#define NLIB_RELIABLE_CLIENT_H

#include <stdint.h>

#include "NetworkClient.h"
#include "NetworkDefine.h"

class ReliableClient : public NetworkClient
{
public:
	void Send(const byte* data, uint32_t length);
};

#endif
