#include "GameEndpoint.h"
#include <cassert>
#include <iostream>

void GameEndpoint::UpdateRTT(float rtt)
{
	static int cnt = 0;
	assert(rtt >= 0.0);

	if (_rtt == 0.0f && rtt > 0.0f)
	{
		_rtt = rtt;
	}
	else
	{
		_rtt += (rtt - _rtt) * 0.1f;
	}

	if (++cnt % 10 == 0)
		std::cout << "[RTT] : " << _rtt << std::endl;
}
