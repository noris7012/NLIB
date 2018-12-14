#ifndef NLIB_CONNECT_TOKEN_H
#define NLIB_CONNECT_TOKEN_H

#include "NetworkDefine.h"

#include "ByteStream.h"

class ConnectToken
{
public:
	static void test();

public:
	ConnectToken();
	~ConnectToken();

	void Write(ByteStream& stream);
	E_READ_RESULT Read(ByteStream& stream);

	uint32_t Length();

private:
	const byte* _token;
};

#endif