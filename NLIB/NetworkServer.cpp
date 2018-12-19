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
������ connection request ��Ŷ�� ó���ϱ� ���� ���� �ܰ踦 ��ģ��.
-* ��Ŷ�� 1062 ����Ʈ�� �ƴ϶�� �����Ѵ�.
-* protocol id �� dedicated server�� ����� ���� �ٸ��ٸ� �����Ѵ�.
-* connect token �� expire timestamp �� current timestamp ���� �۴ٸ� �����Ѵ�.

* encrypted private connect token data �� private key�� version info, protocol id and expire timestamp �� �̿��� ��ȣȭ ���� �ʴ´ٸ� �����Ѵ�.
* ��ȣȭ�� private connect token �� � ���������� ������ �ʴ´ٸ� �����Ѵ�. ���� ��� server address ������ ������ [1, 32] �� �ƴϰų�, address type �� [0, 1]�� �ƴҼ� �ִ�.
	* dedicated server�� public �ּҰ� private connect token �� �����ּ� �ȿ� ���ٸ� �����Ѵ�.

* ��Ŷ�� source IP �� port �� �̹� ����Ǿ� �ִٸ� �����Ѵ�.
* private connect token data �� ���Ե� client id �� �̹� ����Ǿ� �ִٸ� �����Ѵ�.
	* connect token �� �ٸ� IP, port �� ���� ��Ŷ�� ���� ���Ǿ��ٸ� �����Ѵ�.

* �׷��� �ʴٸ� private connect token hmac �� packet source IP address and port �� already used connect token history �� �߰��Ѵ�.
* �̿� ������ Ŭ���̾�Ʈ ������ ���ٸ� ������ Ǯ�� ���̹Ƿ� connection denied ��Ŷ�� ������.
* ��Ŷ�� source IP �� ��Ʈ�� ���� ��ȣȭ ������ �߰��ؼ� �ش� �ּҿ� ��Ʈ�� ���� ���� ��Ŷ�� private connect token �� client to server key �� �̿��ؼ� ��ȣȭ �� �� �ְ� �ϰ�, �ش� �ּҿ� ��Ʈ�� ������ ��Ŷ�� server to client key �� ��ȣȰ �� �ְ� �Ѵ�. ��ȣȭ ������ �ش� �ּҿ� ��Ʈ ���� Ÿ�Ӿƿ� �ð� ���� ��Ŷ�� ������ ������ ����ǰų�, Ÿ�Ӿƿ� �ð� ���� Ŀ�ؼ��� �δ°� �����ϸ� ����ȴ�.
* ��� ������ ������ ���� �ʴ´ٸ� ��Ŷ�� �����Ѵ�.
* �׷��� �ʾƸ� connection challen ��Ŷ���� �����ϰ� challenge sequence number�� ������Ų��.
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

	// Slot �� �̹� �ִٸ� ó�����̹Ƿ� ����.
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

	// Request �� �����µ� Response ��Ŷ�� ��
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

		// Ȥ�� slot �� ������ �Ҵ�� ��찡 �������.
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

	// TODO delete �� �߸��� ���ɼ��� ����. mark �� �س��� Update ���� ��ü ����.
	delete session;
}