#include "NetworkServer.h"

#include <iostream>
#include <assert.h>

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
	config.transport_type = UDP;
	config.port = port;

	assert(config.host == nullptr);

	Startup(config);

	return true;
}

void NetworkServer::Update(long time)
{

}

void NetworkServer::ProcessReceive(NLIBRecv* recv)
{
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