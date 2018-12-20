#include "NetworkLayer.h"

void NetworkLayer::ReadNext(UNLIBData data)
{
	assert(_read_next != nullptr);
	if (_read_next != nullptr)
		_read_next(std::move(data));
}

void NetworkLayer::WriteNext(UNLIBData data)
{
	assert(_write_next != nullptr);
	if (_write_next != nullptr)
		_write_next(std::move(data));
}

void NetworkLayer::SetReadNext(std::function<void(UNLIBData)> func)
{
	assert(_read_next == nullptr);
	_read_next = func;
}

void NetworkLayer::SetWriteNext(std::function<void(UNLIBData)> func)
{
	assert(_write_next == nullptr);
	_write_next = func;
}
