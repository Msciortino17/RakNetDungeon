//***********************************************************************
//	File:		Transform.h
//	Author:		Michael Sciortino
//	Purpose:	Standard for most game objects, contains positiona and orientation
//***********************************************************************

#pragma once

#include "Component.h"

enum Directions
{
	D_UP = 0, D_DOWN, D_LEFT, D_RIGHT
};

class Transform : public Component
{
private:

	int m_iX;
	int m_iY;
	unsigned int m_uiDirection;

public:

	// Start, loop, end
	void Initialize() override;
	void Update() override;
	void Terminate() override;

	unsigned int GetType() const { return C_TRANSFORM; }

	// Getters
	int GetX() const { return m_iX; }
	int GetY() const { return m_iY; }
	unsigned int GetDirection() const { return m_uiDirection; }

	// Setter
	void SetX(int _x) { m_iX = _x; }
	void SetY(int _y) { m_iY = _y; }
	void SetDirection(unsigned int _direction) { m_uiDirection = _direction; }

	// Helper methods
	void Move(int _dx, int _dy) { m_iX += _dx; m_iY += _dy; }

};