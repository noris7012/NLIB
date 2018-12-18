#ifndef NLIB_RELIABLE_SESSION_H
#define NLIB_RELIABLE_SESSION_H

#include <stdint.h>

#include "NetworkSession.h"

class ReliableSession : public NetworkSession
{
public:
	ReliableSession(NetworkServer* server, uint64_t challenge_token_sequence, NLIBAddress address);
};

#endif