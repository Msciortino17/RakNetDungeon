//***********************************************************************
//	File:		Player.h
//	Author:		Michael Sciortino
//	Purpose:	Controlled by the current player, can move around
//***********************************************************************

#pragma once

#include "GameObject.h"

class Player : public GameObject
{
private:
	int timer = 0;

public:

	// Start, loop, end
	void Initialize();
	void Update();
	void Terminate();

	void Serialize(char * _buffer);
	void DeSerialize(char * _buffer);
	unsigned int GetSerialSize() { return 16; };

};