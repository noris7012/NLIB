#ifndef NLIB_NETWORK_CLIENT_H
#define NLIB_NETWORK_CLIENT_H

#include <map>
#include <vector>
#include <cstdint>

#include "NetworkDefine.h"
#include "NetworkEndpoint.h"
#include "ClientState.h"
#include "ProtocolPacket.h"
#include "CUDPLayer.h"

class ClientState;

class NetworkClient : public CUDPLayer
{
public:
	NetworkClient();
	~NetworkClient();

	bool Connect(const char* host, unsigned short port);
	bool IsConnected();
	void Disconnect();

public:
	void Update(uint64_t time) override;
	void OnRecv(NLIBRecv* recv) override;

public:
	void SendPacket(const byte* data, uint32_t length);

public:
	void Send(ProtocolPacket& packet);

public:
	bool SetState(E_CLIENT_STATE_ID state_id);

	void SetChallengeToken(uint64_t challenge_token_sequence, const byte* challenge_token_encrypted);

	uint64_t GetChallengeTokenSequence() { return _challenge_token_sequence; }
	const byte* GetChallengeTokenEncrypted() { return _challenge_token_encrypted; }

	uint32_t GetClientID() { return _client_id; }
	void SetClientID(uint32_t client_id) { _client_id = client_id; };

private:
	NLIBAddress _address;
	ClientState* _state;
	std::map<E_CLIENT_STATE_ID, ClientState*> _state_map;
	std::map<E_CLIENT_STATE_ID, std::vector<E_CLIENT_STATE_ID>*> _state_transition_table;

	uint64_t _challenge_token_sequence;
	const byte* _challenge_token_encrypted;

	uint32_t _client_id;
};

#endif