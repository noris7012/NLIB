#include "NetworkServer.h"

#include <iostream>
#include <cassert>

#include "Utility.h"
#include "ReliableSession.h"

bool NetworkServer::Listen(uint32_t port)
{
	// TODO Check State
	NetworkConfig config;
	config.transport_type = E_TRANSPORT_TYPE::UDP;
	config.port = port;

	assert(config.host == nullptr);
	assert(_endpoint != nullptr);

	_endpoint->Startup(config);

	return true;
}

void NetworkServer::Update(uint64_t time)
{
	for (auto session : _connected_session)
	{
		if (session != nullptr)
			session->Update(time);
	}

	for (auto session : _connection_slot)
	{
		if (session != nullptr)
			session->Update(time);
	}
}


void NetworkServer::Send(NLIBAddress& address, ProtocolPacket& packet)
{
	if (_endpoint)
	{
		auto buffer = _buffer_pool.Acquire();
		ByteStream stream(buffer);
		packet.Write(stream);
		_endpoint->Send(address, stream.Data(), stream.Length());
		_buffer_pool.Release(buffer);		
	}
}

void NetworkServer::OnRecv(NLIBRecv* recv)
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[ Server Receive ] " << std::endl;
	std::cout << Utility::ByteToString(recv->buffer->data, recv->length) << std::endl;
#endif

	ByteStream stream(recv->buffer->data, recv->length);
	ProtocolPacket* packet = ProtocolPacket::Deserialize(stream);

	assert(packet != nullptr);

	if (packet == nullptr)
		return;

	packet->Print();

	auto address_id = recv->address.id();
	if (_connected_session_by_address_id.find(address_id) != _connected_session_by_address_id.end())
	{
		switch (packet->GetID())
		{
		case E_PACKET_ID::CONNECTION_KEEP_ALIVE:
			HandleConnectionKeepAlive(packet, recv);
			break;
		case E_PACKET_ID::CONNECTION_PAYLOAD:
			HandleConnectionPayload(packet, recv);
			break;
		case E_PACKET_ID::CONNECTION_DISCONNECT:
			HandleConnectionDisconnect(packet, recv);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (packet->GetID())
		{
		case E_PACKET_ID::CONNECTION_REQUEST:
			HandleConnectionRequest(packet, recv);
			break;
		case E_PACKET_ID::CONNECTION_RESPONSE:
			HandleConnectionResponse(packet, recv);
			break;
		default:
			break;
		}

	}

	// TODO Test Code
	delete packet;
}

/*
서버는 connection request 패킷을 처리하기 위해 다음 단계를 거친다.
-* 패킷이 1062 바이트가 아니라면 무시한다.
-* protocol id 가 dedicated server가 기대한 값과 다르다면 무시한다.
-* connect token 의 expire timestamp 가 current timestamp 보다 작다면 무시한다.

* encrypted private connect token data 가 private key와 version info, protocol id and expire timestamp 를 이용해 복호화 되지 않는다면 무시한다.
* 복호화된 private connect token 이 어떤 이유에서든 읽히지 않는다면 무시한다. 예를 들면 server address 갯수의 범위가 [1, 32] 가 아니거나, address type 이 [0, 1]이 아닐수 있다.
	* dedicated server의 public 주소가 private connect token 의 서버주소 안에 없다면 무시한다.

* 해킷의 source IP 와 port 가 이미 연결되어 있다면 무시한다.
* private connect token data 에 포함된 client id 가 이미 연결되어 있다면 무시한다.
	* connect token 이 다른 IP, port 를 갖는 패킷에 의해 사용되었다면 무시한다.

* 그렇지 않다면 private connect token hmac 과 packet source IP address and port 를 already used connect token history 에 추가한다.
* 이용 가능한 클라이언트 슬롯이 없다면 서버가 풀인 것이므로 connection denied 패킷을 보낸다.
* 패킷의 source IP 와 포트를 위한 암호화 맵핑을 추가해서 해당 주소와 포트로 부터 오는 패킷은 private connect token 의 client to server key 를 이용해서 복호화 할 수 있게 하고, 해당 주소와 포트로 보내는 패킷은 server to client key 로 암호활 수 있게 한다. 암호화 매핑은 해당 주소와 포트 부터 타임아웃 시간 동안 패킷이 오고가지 않으면 만료되거나, 타임아웃 시간 내에 커넥션을 맺는걸 실패하면 만료된다.
* 어떠한 이유로 매핑이 되지 않는다면 패킷은 무시한다.
* 그렇지 않아면 connection challen 패킷으로 응답하고 challenge sequence number를 증가시킨다.
*/


void NetworkServer::HandleConnectionRequest(ProtocolPacket* p, NLIBRecv* r)
{
	// Check Session Full
	if (_connected_session_by_id.size() >= NLIB_MAX_SESSION)
	{
		ProtocolPacketConnectionDenied packet;
		Send(r->address, packet);
		return;
	}

	// Check Slot Full
	int32_t idx = -1;
	for (int32_t i = 0; i < NLIB_MAX_CONNECTION_SLOT; ++i)
	{
		auto session = _connection_slot[i];
		if (session == nullptr)
		{
			idx = i;
			break;
		}
	}

	if (idx < 0)
	{
		ProtocolPacketConnectionDenied packet;
		Send(r->address, packet);
		return;
	}

	// Slot 에 이미 있다면 처리중이므로 무시.
	for (auto session : _connection_slot)
	{
		if (session == nullptr)
			continue;

		if (session->IsSameAddress(r->address))
		{
			return;
		}
	}

	_connection_slot[idx] = new NetworkSession(this, _challenge_token_sequence++, r->address);
}

void NetworkServer::HandleConnectionResponse(ProtocolPacket* p, NLIBRecv* r)
{
	NetworkSession* session = nullptr;
	for (auto tmp_session : _connection_slot)
	{
		if (tmp_session == nullptr)
			continue;

		if (tmp_session->IsSameAddress(r->address))
		{
			session = tmp_session;
			break;
		}
	}

	// Request 가 없었는데 Response 패킷이 옴
	if (session == nullptr)
		return;

	session->RecvPacket(p);
}

void NetworkServer::HandleConnectionKeepAlive(ProtocolPacket* p, NLIBRecv* r)
{
	assert(p != nullptr && p->GetID() == E_PACKET_ID::CONNECTION_KEEP_ALIVE);

	auto packet = static_cast<ProtocolPacketConnectionKeepAlive*>(p);
	auto session = _connected_session_by_id[packet->GetClientID()];

	assert(session != nullptr);
	if (session == nullptr)
		return;

	session->RecvPacket(p);
}

void NetworkServer::HandleConnectionPayload(ProtocolPacket* p, NLIBRecv* r)
{
	assert(p != nullptr && p->GetID() == E_PACKET_ID::CONNECTION_PAYLOAD);

	auto packet = static_cast<ProtocolPacketConnectionPayload*>(p);
	auto session = _connected_session_by_id[packet->GetClientID()];

	assert(session != nullptr);
	if (session == nullptr)
		return;

	session->RecvPacket(p);
}

void NetworkServer::HandleConnectionDisconnect(ProtocolPacket* p, NLIBRecv* r)
{
	assert(p != nullptr && p->GetID() == E_PACKET_ID::CONNECTION_DISCONNECT);

	auto packet = static_cast<ProtocolPacketConnectionDisconnect*>(p);
	auto session = _connected_session_by_id[packet->GetClientID()];

	if (session == nullptr)
		return;

	session->SetState(E_SESSION_STATE_ID::DISCONNECTED);
}

void NetworkServer::OnConnected(NetworkSession* session)
{
	_connected_session_mutex.lock();

	uint32_t client_id = Utility::Rand32();
	while (_connected_session_by_id.find(client_id) != _connected_session_by_id.end())
	{
		client_id = Utility::Rand32();
	}

	uint32_t slot_id = 0;
	while (slot_id < NLIB_MAX_SESSION)
	{
		auto session = _connected_session[slot_id];
		if (session == nullptr)
			break;

		++slot_id;
	}

	if (slot_id >= NLIB_MAX_SESSION)
	{
		session->SetState(E_SESSION_STATE_ID::DISCONNECTED);
		_connected_session_mutex.unlock();
		return;
	}

	_connected_session[slot_id] = session;
	_connected_session_by_id[client_id] = session;
	_connected_session_by_address_id[session->GetAddressID()] = session;

	session->SetConnection(slot_id, client_id);

	_connected_session_mutex.unlock();

	for (auto& i : _connection_slot)
	{
		auto tmp_session = i;
		if (tmp_session == nullptr)
			continue;

		// 혹시 slot 에 복수개 할당된 경우가 있을까봐.
		if (tmp_session->IsSameAddress(session->GetAddress()))
		{
			i = nullptr;
		}
	}
}

void NetworkServer::OnDisconnected(NetworkSession* session)
{
	_connected_session_mutex.lock();

	_connected_session[session->GetSlotID()] = nullptr;
	_connected_session_by_id.erase(session->GetClientID());
	_connected_session_by_address_id.erase(session->GetAddressID());

	_connected_session_mutex.unlock();

	// TODO delete 가 잘못될 가능성이 있음. mark 만 해놓고 Update 에서 객체 삭제.
	delete session;
}