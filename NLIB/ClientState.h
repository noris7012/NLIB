#ifndef NLIB_CLIENT_STATE_H
#define NLIB_CLIENT_STATE_H

#include "NetworkDefine.h"
#include "NetworkClient.h"
#include "ProtocolPacket.h"

class NetworkClient;
class ProtocolPacket;

class ClientState
{
public:
	static ClientState* create(E_CLIENT_STATE_ID state_id, NetworkClient* client);

	void SetClient(NetworkClient* client);
public:
	virtual E_CLIENT_STATE_ID getID() = 0;
	virtual void OnEnter() { };
	virtual void Update(uint64_t time) { };
	virtual void OnExit() { };
	virtual void RecvPacket(ProtocolPacket* packet) { };
	virtual void Write(PNLIBData data) { };

protected:
	NetworkClient* _client;
};

class ClientStateInit : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() override  { return E_CLIENT_STATE_ID::INIT; }
	void OnEnter() override;
};

class ClientStateDisconnected : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() override  { return E_CLIENT_STATE_ID::DISCONNECTED; }
	void OnEnter() override;
	void Update(uint64_t time) override;
	void OnExit() override;
	void RecvPacket(ProtocolPacket* packet) override;

private:
	uint64_t _send_request_time;
	uint64_t _next_request_interval;
	uint64_t _limit_request_time;
};

class ClientStateSendingConnectionRequest : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() override  { return E_CLIENT_STATE_ID::SENDING_CONNECTION_REQUEST; }
	void OnEnter() override;
	void Update(uint64_t time) override;
	void RecvPacket(ProtocolPacket* packet) override;

private:
	uint64_t _send_request_time;
	uint64_t _next_request_interval;
	uint64_t _limit_request_time;
};

class ClientStateSendingConnectionResponse : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() override  { return E_CLIENT_STATE_ID::SENDING_CONNECTION_RESPONSE; }
	void OnEnter() override;
	void Update(uint64_t time) override;
	void RecvPacket(ProtocolPacket* packet) override;

private:
	uint64_t _send_response_time;
	uint64_t _next_response_interval;
	uint64_t _limit_response_time;
};

class ClientStateConnected : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() override  { return E_CLIENT_STATE_ID::CONNECTED; }
	void OnEnter() override;
	void Update(uint64_t time) override;
	void RecvPacket(ProtocolPacket* packet) override;
	void Write(PNLIBData data) override;

private:
	uint64_t _send_keep_alive_time;
	uint64_t _next_keep_alive_interval;
};

#endif