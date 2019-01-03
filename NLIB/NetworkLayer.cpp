#include "NetworkLayer.h"

void NetworkLayer::ReadNext(PNLIBData data)
{
	assert(_read_next != nullptr);
	if (_read_next != nullptr)
		_read_next(data);
}

void NetworkLayer::WriteNext(PNLIBData data)
{
	assert(_write_next != nullptr);
	if (_write_next != nullptr)
		_write_next(data);
}

void NetworkLayer::SetReadNext(std::function<void(PNLIBData)> func)
{
	assert(_read_next == nullptr);
	_read_next = func;
}

void NetworkLayer::SetWriteNext(std::function<void(PNLIBData)> func)
{
	assert(_write_next == nullptr);
	_write_next = func;
}

void NetworkLayer::SetFailNext(std::function<void(PNLIBData)> func)
{
	assert(_fail_next == nullptr);
	_fail_next = func;
}
