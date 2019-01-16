#ifndef NLIB_GAME_SERVER_INTERFACE_H
#define NLIB_GAME_SERVER_INTERFACE_H

struct GameConfig;

class GameServerInterface
{
public:
	virtual bool Listen(GameConfig& config) = 0;
	
};

#endif
