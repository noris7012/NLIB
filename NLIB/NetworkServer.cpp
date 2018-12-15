#include "NetworkServer.h"

#include <iostream>
#include <assert.h>
#include "Utility.h"

NetworkServer::NetworkServer()
{

}

NetworkServer::~NetworkServer()
{

}

bool NetworkServer::Listen(uint32_t port)
{
	// TODO Check State
	NetworkConfig config;
	config.transport_type = E_TRANSPORT_TYPE::UDP;
	config.port = port;

	assert(config.host == nullptr);

	Startup(config);

	return true;
}

void NetworkServer::Update(long time)
{
	for (auto it = _connected_session_by_id.begin(); it != _connected_session_by_id.end(); ++it)
	{
		auto session = it->second;
		if (session == nullptr)
			continue;

		session->Update(time);
	}

	for (int i = 0; i < NLIB_MAX_CONNECTION_SLOT; ++i)
	{
		auto session = _connection_slot[i];
		if (session == nullptr)
			continue;

		session->Update(time);
	}
}

void NetworkServer::ProcessReceive(NLIBRecv* recv)
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
	
	// ��Ŷ�� source IP �� port �� �̹� ����Ǿ� �ִٸ� �����Ѵ�.
	auto address_id = recv->address.id();
	if (_connected_session_by_address_id.find(address_id) != _connected_session_by_address_id.end())
		return;

	packet->Print();

	switch (packet->GetID())
	{
	case E_PACKET_ID::CONNECTION_REQUEST:
		HandleConnectionRequest(packet, recv);
		break;
	case E_PACKET_ID::CONNECTION_RESPONSE:
		HandleConnectionResponse(packet, recv);
		break;
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
		Send(packet);
		return;
	}

	// Check Slot Full
	uint32_t idx = -1;
	for (int i = 0; i < NLIB_MAX_CONNECTION_SLOT; ++i)
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
		Send(packet);
		return;
	}

	// Slot �� �̹� �ִٸ� ó�����̹Ƿ� ����.
	for (int i = 0; i < NLIB_MAX_CONNECTION_SLOT; ++i)
	{
		auto session = _connection_slot[i];
		if (session == nullptr)
			continue;

		if (session->IsSameAddress(r->address))
		{
			return;
		}
	}

	// TODO challenge_token_encrypted ��¥ ��ȣȭ�ϱ�
	byte* challenge_token_encrypted = new byte[NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH];
	uint64_t challenge_token_sequence = _challenge_token_sequence++;

	_connection_slot[idx] = new NetworkSession(this, challenge_token_sequence, challenge_token_encrypted, r->address);
}

void NetworkServer::HandleConnectionResponse(ProtocolPacket* p, NLIBRecv* r)
{
	NetworkSession* session = nullptr;
	for (int i = 0; i < NLIB_MAX_CONNECTION_SLOT; ++i)
	{
		auto tmp_session = _connection_slot[i];
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

	session->HandlePacket(p);
}

void NetworkServer::HandleConnectionKeepAlive(ProtocolPacket* p, NLIBRecv* r)
{

}

void NetworkServer::HandleConnectionPayload(ProtocolPacket* p, NLIBRecv* r)
{

}
void NetworkServer::HandleConnectionDisconnect(ProtocolPacket* p, NLIBRecv* r)
{

}