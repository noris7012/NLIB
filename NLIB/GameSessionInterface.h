#ifndef NLIB_GAME_SESSION_INTERFACE_H
#define NLIB_GAME_SESSION_INTERFACE_H

class GameSessionInterface
{
public:
	virtual void WritePacket(const byte* bytes, uint32_t length) = 0;
	virtual void Close() = 0;
};

#endif
