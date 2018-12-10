#include "NetworkEndpoint.h"

#include <chrono>

NetworkEndpoint::NetworkEndpoint()
{

}

NetworkEndpoint::~NetworkEndpoint()
{

}

void NetworkEndpoint::startup(NetworkConfig& config)
{
	_thread = new std::thread([this]()
	{
		using namespace std::chrono;

		while (_running)
		{
			auto time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

			update(time);

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});
}

void NetworkEndpoint::update(long time)
{

}