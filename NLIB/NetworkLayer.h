#ifndef NLIB_NETWORK_LAYER_H
#define NLIB_NETWORK_LAYER_H

#include <functional>
#include <cassert>

#include "NetworkStruct.h"
#include "ByteArray.h"

class NetworkLayer
{
public:
	void ReadNext(ByteArrayPtr data)
	{
		assert(_read_next != nullptr);
		if (_read_next != nullptr)
			_read_next(data);
	}

	void WriteNext(ByteArrayPtr data)
	{
		assert(_write_next != nullptr);
		if (_write_next != nullptr)
			_write_next(data);
	}

	void FailNext(ByteArrayPtr data)
	{
		assert(_fail_next != nullptr);
		if (_fail_next != nullptr)
			_fail_next(data);
	}

	void SetReadNext(std::function<void(ByteArrayPtr)> func)
	{
		assert(_read_next == nullptr);
		_read_next = func;
	}

	void SetWriteNext(std::function<void(ByteArrayPtr)> func)
	{
		assert(_write_next == nullptr);
		_write_next = func;
	}

	void SetFailNext(std::function<void(ByteArrayPtr)> func)
	{
		assert(_fail_next == nullptr);
		_fail_next = func;
	}

	virtual void Write(ByteArrayPtr data) { assert(false); }
	virtual void Read(ByteArrayPtr data) { assert(false); }
	virtual void Fail(ByteArrayPtr data) { assert(false); }

private:
	std::function<void(ByteArrayPtr)> _read_next = nullptr;
	std::function<void(ByteArrayPtr)> _write_next = nullptr;
	std::function<void(ByteArrayPtr)> _fail_next = nullptr;
};

#endif
