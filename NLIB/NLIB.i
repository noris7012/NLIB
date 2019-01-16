%module(directors="1") nlib

%{
#include "GamePacket.h"
#include "GameConfig.h"
#include "GameClientInterface.h"
#include "GameClientHandler.h"
#include "GameClientFactory.h"
#include "GameServerInterface.h"
#include "GameServerHandler.h"
#include "GameServerFactory.h"
%}

%feature("director") GameClientHandler;
%feature("director") GameServerHandler;

%include "GameConfig.h"
%include "GameClientInterface.h"
%include "GameClientHandler.h"
%include "GameClientFactory.h"
%include "GameServerInterface.h"
%include "GameServerHandler.h"
%include "GameServerFactory.h"