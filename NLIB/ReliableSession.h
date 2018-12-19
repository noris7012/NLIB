#ifndef NLIB_RELIABLE_SESSION_H
#define NLIB_RELIABLE_SESSION_H

#include <cstdint>

#include "NetworkSession.h"
#include "NetworkStruct.h"
#include "ReliablePacket.h"

class ReliableSession
{
public:
	ReliableSession();

	void OnRecv(NLIBData data);

	uint32_t GetHeaderLength() { return ReliablePacket::GetHeaderLength(); }

private:
	BufferPool _buffer_pool;

public:
	std::function<void(NLIBData)> _recv_next = nullptr;
};

#endif