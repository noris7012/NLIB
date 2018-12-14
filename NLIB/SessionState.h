#ifndef NLIB_SESSION_STATE_H
#define NLIB_SESSION_STATE_H

#include "NetworkDefine.h"
#include "NetworkSession.h"
#include "ProtocolPacket.h"

class NetworkSession;
class ProtocolPacket;

class SessionState
{
public:
	static SessionState* Create(E_SESSION_STATE_ID state_id, NetworkSession* session);

	void SetSession(NetworkSession* session);

public:
	virtual E_SESSION_STATE_ID GetID() = 0;
	virtual void OnEnter() { };
	virtual void Update(long time) { };
	virtual void OnExit() { };
	virtual void HandlePacket(ProtocolPacket* packet) { };

protected:
	NetworkSession* _session;
};

class SessionStateDisconnected : public SessionState
{
public:
	E_SESSION_STATE_ID GetID() { return E_SESSION_STATE_ID::DISCONNECTED;  }
	void OnEnter();
	void Update(long time);
};

class SessionStateSendingConnectionChallenge : public SessionState
{
public:
	E_SESSION_STATE_ID GetID() { return E_SESSION_STATE_ID::SENDING_CONNECTION_CHALLENGE; }
	void OnEnter();
	void Update(long time);
	void HandlePacket(ProtocolPacket* packet);

private:
	long _send_request_time;
	long _next_request_interval;
	long _limit_request_time;
};

class SessionStateConnected : public SessionState
{
public:
	E_SESSION_STATE_ID GetID() { return E_SESSION_STATE_ID::CONNECTED; }
	void OnEnter();
	void Update(long time);
	void HandlePacket(ProtocolPacket* packet);

private:
	long _send_keep_alive_time;
	long _next_keep_alive_interval;
};

#endif