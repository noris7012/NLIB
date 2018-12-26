#include "GameEndpoint.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include "Utility.h"
#include "Logger.h"

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

	std::stringstream stream;
	stream << "[RTT] : " << _rtt << " , " << rtt;

	Logger::GetInstance()->Log(stream.str());
}
