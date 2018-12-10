#include "NetworkClient.h"

#include <thread>
#include <chrono>

int main(int argc, char* argv[])
{
	NetworkClient client;

	client.connect("127.0.0.1", 7171);

	while (!client.IsConnected())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	client.Send("abc");

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	}
}