//***********************************************************************
//	File:		MessageTypes.h
//	Author:		Michael Sciortino
//	Purpose:	Different types of network messages
//***********************************************************************

#pragma once

#include "../Raknet/MessageIdentifiers.h"

#define SERVER_ADDRESS 0

enum GameMessages
{
	ID_ASSIGN_ID = ID_USER_PACKET_ENUM + 1,
	ID_DISCONNECT,
	ID_SERVER_SHUTDOWN,
	ID_CREATE_PLAYER,
	ID_DESTROY_PLAYER,
	ID_UPDATE_INPUT,
	ID_GAME_SNAPSHOT,
	ID_UPDATE_ENTITY,
	ID_ENTITY_MESSAGE,
	ID_IS_ALIVE
};

enum NWViewMessages
{
	ID_NWV_UPDATE_POSITION,
};