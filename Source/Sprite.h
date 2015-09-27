//***********************************************************************
//	File:		Sprite.h
//	Author:		Michael Sciortino
//	Purpose:	Will render a certain char as a sprite
//***********************************************************************

#pragma once

#include "Component.h"

class Sprite : public Component
{
private:

	bool m_bVisible;
	char m_cCharacter;

public:

	// Start, loop, end
	void Initialize();
	void Update();
	void Terminate();

	unsigned int GetType() const { return C_SPRITE; }

	// Getters
	char GetCharacter() const { return m_cCharacter; }
	bool GetVisible() const { return m_bVisible; }

	// Setter
	void SetCharacter(char _char) { m_cCharacter = _char; }
	void SetVisible(bool _vis) { m_bVisible = _vis; }

};