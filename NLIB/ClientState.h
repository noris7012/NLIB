#ifndef NLIB_CLIENT_STATE_H
#define NLIB_CLIENT_STATE_H

#include "NetworkDefine.h"

class ClientState
{
public:
	static ClientState* create(E_CLIENT_STATE_ID state_id);

public:
	virtual E_CLIENT_STATE_ID getID() = 0;
	virtual void OnEnter() { };
	virtual void Update(long time) { };
	virtual void OnExit() { };
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
	//void Update(long time);
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