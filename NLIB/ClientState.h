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
	virtual void Update(long time) { };
	virtual void OnExit() { };
	virtual void HandlePacket(ProtocolPacket* packet) { };

protected:
	NetworkClient* _client;
};

class ClientStateDisconnected : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() { return E_CLIENT_STATE_ID::DISCONNECTED; }
	void OnEnter();
	void Update(long time);
};

class ClientStateSendingConnectionRequest : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() { return E_CLIENT_STATE_ID::SENDING_CONNECTION_REQUEST; }
	void OnEnter();
	void Update(long time);
	void HandlePacket(ProtocolPacket* packet);

private:
	long _send_request_time;
	long _next_request_interval;
	long _limit_request_time;
};

class ClientStateSendingConnectionResponse : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() { return E_CLIENT_STATE_ID::SENDING_CONNECTION_RESPONSE; }
	void OnEnter();
	void Update(long time);
	void HandlePacket(ProtocolPacket* packet);

private:
	long _send_response_time;
	long _next_response_interval;
	long _limit_response_time;
};

class ClientStateConnected : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() { return E_CLIENT_STATE_ID::CONNECTED; }
	void OnEnter();
	void Update(long time);
	void HandlePacket(ProtocolPacket* packet);

private:
	long _send_keep_alive_time;
	long _next_keep_alive_interval;
};

#endif