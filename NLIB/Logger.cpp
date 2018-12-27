#include "Logger.h"

#include <sstream>
#include <iostream>
#include <string>
#include "Utility.h"

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
	std::string str(msg);

	auto now = Utility::GetTime();

	std::stringstream ss;
	ss << "[" << Utility::TimeToString(now) << "] " << str;

	_lock.lock();
	_queue.push(ss.str());
	_lock.unlock();
}

void Logger::Log(std::string&& msg)
{
	auto now = Utility::GetTime();

	std::stringstream ss;
	ss << "[" << Utility::TimeToString(now) << "] " << msg;

	_lock.lock();
	_queue.push(ss.str());
	_lock.unlock();
}
