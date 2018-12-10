#ifndef NLIB_CLIENT_STATE_H
#define NLIB_CLIENT_STATE_H

#include "NetworkDefine.h"
#include "NetworkClient.h"

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

protected:
	NetworkClient* _client;
};

class ClientStateDisconnected : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() { return DISCONNECTED; }
	void Update(long time);
};

class ClientStateSendingConnectionRequest : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() { return SENDING_CONNECTIN_REQUEST; }
	void OnEnter();
	void Update(long time);

private:
	long _send_request_time;
	long _next_request_interval;
};

class ClientStateSendingConnectionResponse : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() { return SENDING_CONNECTIN_RESPONSE; }
	//void Update(long time);
};

class ClientStateConnected : public ClientState
{
public:
	E_CLIENT_STATE_ID getID() { return CONNECTED; }
	//void Update(long time);
};

#endif;