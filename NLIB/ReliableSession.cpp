#include "ReliableSession.h"

ReliableSession::ReliableSession(NetworkServer* server, uint64_t challenge_token_sequence, NLIBAddress address)
	: NetworkSession(server, challenge_token_sequence, address)
{

}