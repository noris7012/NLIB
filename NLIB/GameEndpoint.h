#ifndef NLIB_GAME_ENDPOINT_H
#define NLIB_GAME_ENDPOINT_H

#include <cstdint>

class GameEndpoint
{
public:
	void UpdateRTT(float rtt);


	virtual void Update(uint64_t time) = 0;
private:
	float _rtt = 0.0f;
};

#endif