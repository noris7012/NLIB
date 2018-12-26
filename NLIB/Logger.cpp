#include "Logger.h"

#include <iostream>
#include <string>

Logger* Logger::GetInstance()
{
	static auto instance = new Logger();

	return instance;
}

Logger::Logger()
{

	_thread = new std::thread([this]()
	{
		while (true)
		{
			while (!_queue.empty())
			{
				_lock.lock();
				std::string msg = _queue.front();
				_queue.pop();
				_lock.unlock();

				std::cout << msg << std::endl;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});
}

void Logger::Log(const char* msg)
{
	_lock.lock();
	_queue.push(std::string(msg));
	_lock.unlock();
}

void Logger::Log(std::string&& msg)
{
	_lock.lock();
	_queue.push(msg);
	_lock.unlock();
}
