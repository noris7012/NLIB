#include "NetworkEndpoint.h"

#include <chrono>

NetworkEndpoint::NetworkEndpoint()
{

}

NetworkEndpoint::~NetworkEndpoint()
{

}

void NetworkEndpoint::Startup(NetworkConfig& config)
{
	_running = false;

	_transport = TransportLayer::Create(config.transport_type);
	_transport->Startup(config, this);

	_thread = new std::thread([this]()
	{
		using namespace std::chrono;

		while (_running)
		{
			auto time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

			InternalUpdate(time);
			Update(time);

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});
}

bool NetworkEndpoint::IsConnected()
{
	return _transport->IsConnected();
}

void NetworkEndpoint::InternalUpdate(long time)
{

}

void NetworkEndpoint::HandleReceive(S_Recv_Ptr data)
{
	_recv_queue_mutex.lock();
	_recv_queue.push(data);
	_recv_queue_mutex.unlock();
}

void NetworkEndpoint::Send(const char* data)
{
	S_Send send;
	memcpy_s(send.data, sizeof(send.data), data, sizeof(data) + 1);
	send.length = sizeof(data);

	_transport->Send(send);
}