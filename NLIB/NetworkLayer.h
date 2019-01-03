#ifndef NLIB_NETWORK_LAYER_H
#define NLIB_NETWORK_LAYER_H

#include <functional>
#include <cassert>

#include "NetworkStruct.h"

class NetworkLayer
{
public:
	void ReadNext(PNLIBData data);
	void WriteNext(PNLIBData data);
	void FailNext(PNLIBData data);

	void SetReadNext(std::function<void(PNLIBData)> func);
	void SetWriteNext(std::function<void(PNLIBData)> func);
	void SetFailNext(std::function<void(PNLIBData)> func);

	virtual void Write(PNLIBData data) { assert(false); }
	virtual void Read(PNLIBData data) { assert(false); }
	virtual void Fail(PNLIBData data) { assert(false); }

private:
	std::function<void(PNLIBData)> _read_next = nullptr;
	std::function<void(PNLIBData)> _write_next = nullptr;
	std::function<void(PNLIBData)> _fail_next = nullptr;
};

#endif
