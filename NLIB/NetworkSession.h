#ifndef NLIB_NETWORK_SESSION_H
#define NLIB_NETWORK_SESSION_H

#include <stdint.h>
#include <map>
#include <vector>

#include "NetworkDefine.h"
#include "NetworkStruct.h"
#include "SessionState.h"
#include "ProtocolPacket.h"
#include "NetworkServer.h"

class SessionState;
class NetworkServer;

class NetworkSession
{
public:
	NetworkSession(NetworkServer* server, uint64_t challenge_token_sequence, NLIBAddress address);
	~NetworkSession();

public:
	void Update(uint64_t time);
	void Send(ProtocolPacket& packet);
	void RecvPacket(ProtocolPacket* packet);
	//void ProcessReceive(NLIBRecv* data);

	void OnConnected();
	void OnDisconnected();

public:
	bool SetState(E_SESSION_STATE_ID state_id);
	E_SESSION_STATE_ID GetStateID();

	uint64_t GetChallengeTokenSequence() { return _challenge_token_sequence; }
	const byte* GetChallengeTokenEncrypted() { return _challenge_token_encrypted;  }

	bool IsSameAddress(NLIBAddress& address);
	uint64_t GetAddressID();
	NLIBAddress& GetAddress();

	uint32_t GetClientID() { return _client_id; }
	uint32_t GetSlotID() { return _slot_id; }
	void SetConnection(uint32_t slot_id, uint32_t client_id)
	{
		_slot_id = slot_id;
		_client_id = client_id; 
	};

private:
	NetworkServer* _server;
	uint64_t _created_time;

	SessionState* _state;
	std::map<E_SESSION_STATE_ID, SessionState*> _state_map;
	std::map<E_SESSION_STATE_ID, std::vector<E_SESSION_STATE_ID>*> _state_transition_table;

	uint64_t _challenge_token_sequence;
	const byte* _challenge_token_encrypted;
	NLIBAddress _address;

	uint32_t _client_id;
	uint32_t _slot_id;
};

#endif