#ifndef NLIB_UDP_LAYER_H
#define NLIB_UDP_LAYER_H

#include <cstdint>

#include "NetworkStruct.h"
#include "NetworkEndpoint.h"

class NetworkEndpoint;

class CUDPLayer
{
public:
	CUDPLayer();

protected:
	~CUDPLayer();

public:
	virtual void Update(uint64_t time) = 0;
	virtual void OnRecv(NLIBRecv* recv) = 0;

protected:
	NetworkEndpoint* _endpoint;
	BufferPool _buffer_pool;
};

#endif
