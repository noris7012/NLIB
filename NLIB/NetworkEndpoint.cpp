#include "NetworkEndpoint.h"

#include <chrono>

#include "Utility.h"

NetworkEndpoint::NetworkEndpoint()
{

}

NetworkEndpoint::~NetworkEndpoint()
{

}

void NetworkEndpoint::Startup(NetworkConfig& config)
{
	_transport = TransportLayer::Create(config.transport_type);
	_transport->Startup(config, this);

	_thread = new std::thread([this]()
	{
		using namespace std::chrono;

		_running = true;
		while (_running)
		{
			auto time = Utility::GetTime();

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
	while (true)
	{
		auto data = Pop();
		if (data == nullptr)
			break;

		ProcessReceive(data);

		_buffer_pool.Release(data->buffer);
		delete data;
	}
}

NLIBRecv* NetworkEndpoint::Pop()
{
	// TODO Implement Synchronized Queue
	_recv_queue_mutex.lock();
	NLIBRecv* data = nullptr;
	if (!_recv_queue.empty())
	{
		data = _recv_queue.front();
		_recv_queue.pop();
	}
	_recv_queue_mutex.unlock();

	return data;
}

void NetworkEndpoint::HandleReceive(char* data, std::size_t length)
{
	auto buffer = _buffer_pool.Acquire();
	memcpy_s(buffer->data, sizeof(buffer->data), data, length);

	NLIBRecv* recv = new NLIBRecv();
	recv->buffer = buffer;
	recv->length = length;

	// TODO Implement Synchronized Queue
	_recv_queue_mutex.lock();
	_recv_queue.push(recv);
	_recv_queue_mutex.unlock();
}

void NetworkEndpoint::Send(const char* data)
{
	S_Send send;
	memcpy_s(send.data, sizeof(send.data), data, sizeof(data) + 1);
	send.length = sizeof(data);

	_transport->Send(send);
}

void NetworkEndpoint::Send(ByteStream& stream)
{
	S_Send data;
	data.length = stream.Length();
	memcpy_s(data.data, sizeof(data.data), stream.Data(), stream.Length());
	assert(sizeof(data.data) == MAX_MTU_SIZE);

	_transport->Send(data);
}

void NetworkEndpoint::Send(ProtocolPacket& packet)
{
	auto buffer = _buffer_pool.Acquire();
	ByteStream stream(buffer);
	packet.Write(stream);
	_transport->Send(stream);
	_buffer_pool.Release(buffer);
}