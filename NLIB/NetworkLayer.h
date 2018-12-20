#ifndef NLIB_NETWORK_LAYER_H
#define NLIB_NETWORK_LAYER_H

#include <functional>
#include <cassert>

#include "NetworkStruct.h"

class NetworkLayer
{
public:
	void ReadNext(UNLIBData data);
	void WriteNext(UNLIBData data);

	void SetReadNext(std::function<void(UNLIBData)> func);
	void SetWriteNext(std::function<void(UNLIBData)> func);

	virtual void Write(UNLIBData data) { assert(false); }
	virtual void Read(UNLIBData data) { assert(false); }

private:
	std::function<void(UNLIBData)> _read_next = nullptr;
	std::function<void(UNLIBData)> _write_next = nullptr;
};

#endif
