#ifndef NLIB_NETWORK_LAYER_H
#define NLIB_NETWORK_LAYER_H

#include <functional>
#include <cassert>

#include "NetworkStruct.h"
#include "ByteArray.h"

class NetworkLayer
{
public:
	inline void ReadNext(ByteArrayPtr data);
	inline void WriteNext(ByteArrayPtr data);
	inline void FailNext(ByteArrayPtr data);

	inline void SetReadNext(std::function<void(ByteArrayPtr)> func);
	inline void SetWriteNext(std::function<void(ByteArrayPtr)> func);
	inline void SetFailNext(std::function<void(ByteArrayPtr)> func);

	virtual void Write(ByteArrayPtr data) { assert(false); }
	virtual void Read(ByteArrayPtr data) { assert(false); }
	virtual void Fail(ByteArrayPtr data) { assert(false); }

private:
	std::function<void(ByteArrayPtr)> _read_next = nullptr;
	std::function<void(ByteArrayPtr)> _write_next = nullptr;
	std::function<void(ByteArrayPtr)> _fail_next = nullptr;
};

#endif
