#ifndef NLIB_RUDP_LAYER_H
#define NLIB_RUDP_LAYER_H
#include "CUDPLayer.h"

class RUDPLayer
{
public:
	RUDPLayer(bool is_client);

protected:
	~RUDPLayer();

protected:
	CUDPLayer* _cudp_layer;
};

#endif