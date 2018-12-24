#ifndef NLIB_GAME_ENDPOINT_H
#define NLIB_GAME_ENDPOINT_H

class GameEndpoint
{
public:
	void UpdateRTT(float rtt);

private:
	float _rtt = 0.0f;
};

#endif