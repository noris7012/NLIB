#ifndef NLIB_LOGGER_H
#define NLIB_LOGGER_H

#include <queue>
#include <mutex>

class Logger
{
public:
	static Logger* GetInstance();

private:
	Logger();

public:
	void Log(const char* msg);
	void Log(std::string&& msg);

private:
	std::thread* _thread;

	std::mutex _lock;
	std::queue<std::string> _queue;
};

#endif