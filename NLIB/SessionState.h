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
	virtual void Update(uint64_t time) { };
	virtual void OnExit() { };
	virtual void RecvPacket(ProtocolPacket* packet) { };
	virtual void Write(ByteArrayPtr) { };

protected:
	NetworkSession* _session;
};

class SessionStateInit : public SessionState
{
public:
	E_SESSION_STATE_ID GetID() override  { return E_SESSION_STATE_ID::INIT; }
	void OnEnter() override;
};

class SessionStateDisconnected : public SessionState
{
public:
	E_SESSION_STATE_ID GetID() override { return E_SESSION_STATE_ID::DISCONNECTED; }
	void OnEnter() override;
	void Update(uint64_t time) override;
	void OnExit() override;
	void RecvPacket(ProtocolPacket* packet) override;

private:
	uint64_t _send_request_time;
	uint64_t _next_request_interval;
	uint64_t _limit_request_time;
};

class SessionStateSendingConnectionChallenge : public SessionState
{
public:
	E_SESSION_STATE_ID GetID() override  { return E_SESSION_STATE_ID::SENDING_CONNECTION_CHALLENGE; }
	void OnEnter() override;
	void Update(uint64_t time) override;
	void RecvPacket(ProtocolPacket* packet) override;

private:
	uint64_t _send_request_time;
	uint64_t _next_request_interval;
	uint64_t _limit_request_time;
};

class SessionStateConnected : public SessionState
{
public:
	E_SESSION_STATE_ID GetID() override { return E_SESSION_STATE_ID::CONNECTED; }
	void OnEnter() override;
	void Update(uint64_t time) override;
	void RecvPacket(ProtocolPacket* packet) override;
	void Write(ByteArrayPtr data) override;

private:
	uint64_t _send_keep_alive_time;
	uint64_t _next_keep_alive_interval;
	uint64_t _last_recv_time;
};

#endif