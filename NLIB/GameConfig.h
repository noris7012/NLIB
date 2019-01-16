#ifndef NLIB_GAME_CONFIG_H
#define NLIB_GAME_CONFIG_H

#include <stdint.h>
#include <vector>

#include "NetworkDefine.h"

struct GameConfig
{
	const char* host = nullptr;
	uint32_t port = 0;
	std::vector<bool>* loss_mask = nullptr;
	float loss_rate = 0.0f;
};

#endif