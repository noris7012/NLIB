#include "NetworkEndpoint.h"

#include <chrono>

#include "Utility.h"

NetworkEndpoint::NetworkEndpoint()
	: _thread(nullptr)
	, _running(false)
	, _transport(nullptr)
{
}

void NetworkEndpoint::Startup(NetworkConfig& config)
{
	_transport = new TransportLayer();
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

void NetworkEndpoint::Destroy()
{
	/*
	_running = false;
	_thread->join();

	delete _thread;
	_thread = nullptr;

	_transport->Destroy();
	delete _transport;
	_transport = nullptr;
	*/
}

void NetworkEndpoint::InternalUpdate(uint64_t time)
{
	while (true)
	{
		auto data = Pop();
		if (data == nullptr)
			break;

		HandleReceive(data);

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

void NetworkEndpoint::HandleReceive(char* data, std::size_t length, NLIBAddress& address)
{
	auto buffer = _buffer_pool.Acquire();
	memcpy_s(buffer->data, sizeof(buffer->data), data, length);

	NLIBRecv* recv = new NLIBRecv();
	recv->buffer = buffer;
	recv->length = length;
	recv->address = address;

	// TODO Implement Synchronized Queue
	_recv_queue_mutex.lock();
	_recv_queue.push(recv);
	_recv_queue_mutex.unlock();
}

void NetworkEndpoint::Send(NLIBAddress& address, const byte* data, uint32_t length)
{
	if (_transport)
		_transport->Send(address, data, length);
}

void NetworkEndpoint::Send(NLIBAddress& address, UNLIBData data)
{
	if (_transport)
		_transport->Send(address, std::move(data));
}
