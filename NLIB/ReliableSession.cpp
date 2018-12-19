#include "ReliableSession.h"
#include "ReliablePacket.h"

ReliableSession::ReliableSession()
{

}

void ReliableSession::OnRecv(NLIBData data)
{
	ByteStream stream(const_cast<byte*>(data.data), data.length);
	ReliablePacket packet;
	auto rc = packet.Read(stream);

	// TODO 
	if (rc != E_READ_RESULT::SUCCESS)
		return;

	assert(_recv_next != nullptr);
	_recv_next(packet.GetData());
}