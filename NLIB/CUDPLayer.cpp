#include "CUDPLayer.h"

CUDPLayer::CUDPLayer()
{
	_endpoint = new NetworkEndpoint(this);
}

CUDPLayer::~CUDPLayer()
{
	delete _endpoint;	
}