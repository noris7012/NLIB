#ifndef NLIB_NETWORK_LAYER_H
#define NLIB_NETWORK_LAYER_H

#include <functional>
#include <cassert>

#include "NetworkStruct.h"
#include "ByteArray.h"

class NetworkLayer
{
public:
	void ReadNext(const ReadParam& param)
	{
		assert(_read_next != nullptr);
		if (_read_next != nullptr)
			_read_next(param);
	}

	void WriteNext(const WriteParam& param)
	{
		assert(_write_next != nullptr);
		if (_write_next != nullptr)
			_write_next(param);
	}

	void FailNext(const FailParam& param)
	{
		assert(_fail_next != nullptr);
		if (_fail_next != nullptr)
			_fail_next(param);
	}

	void SetReadNext(std::function<void(const ReadParam&)> func)
	{
		assert(_read_next == nullptr);
		_read_next = func;
	}

	void SetWriteNext(std::function<void(const WriteParam&)> func)
	{
		assert(_write_next == nullptr);
		_write_next = func;
	}

	void SetFailNext(std::function<void(const FailParam&)> func)
	{
		assert(_fail_next == nullptr);
		_fail_next = func;
	}

	virtual void Write(const WriteParam& param) { assert(false); }
	virtual void Read(const ReadParam& param) { assert(false); }
	virtual void Fail(const FailParam& param) { assert(false); }

private:
	std::function<void(const ReadParam&)> _read_next = nullptr;
	std::function<void(const WriteParam&)> _write_next = nullptr;
	std::function<void(const FailParam&)> _fail_next = nullptr;
};

#endif
