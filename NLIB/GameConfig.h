#ifndef NLIB_GAME_CONFIG_H
#define NLIB_GAME_CONFIG_H

#include <stdint.h>

#include "NetworkDefine.h"

struct GameConfig
{
	const char* host = nullptr;
	uint32_t port = 0;
};

#endif