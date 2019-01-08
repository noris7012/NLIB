#include "NetworkLayer.h"

void NetworkLayer::ReadNext(ByteArrayPtr data)
{
	assert(_read_next != nullptr);
	if (_read_next != nullptr)
		_read_next(data);
}

void NetworkLayer::WriteNext(ByteArrayPtr data)
{
	assert(_write_next != nullptr);
	if (_write_next != nullptr)
		_write_next(data);
}

void NetworkLayer::FailNext(ByteArrayPtr data)
{
	assert(_fail_next != nullptr);
	if (_fail_next != nullptr)
		_fail_next(data);
}

void NetworkLayer::SetReadNext(std::function<void(ByteArrayPtr)> func)
{
	assert(_read_next == nullptr);
	_read_next = func;
}

void NetworkLayer::SetWriteNext(std::function<void(ByteArrayPtr)> func)
{
	assert(_write_next == nullptr);
	_write_next = func;
}

void NetworkLayer::SetFailNext(std::function<void(ByteArrayPtr)> func)
{
	assert(_fail_next == nullptr);
	_fail_next = func;
}
