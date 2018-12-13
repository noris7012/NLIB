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
	
	// 패킷의 source IP 와 port 가 이미 연결되어 있다면 무시한다.
	auto address_id = recv->address.id();
	if (_connected_session_by_address_id.find(address_id) != _connected_session_by_address_id.end())
		return;

	packet->Print();

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