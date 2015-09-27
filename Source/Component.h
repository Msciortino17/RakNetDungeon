//***********************************************************************
//	File:		Component.h
//	Author:		Michael Sciortino
//	Purpose:	Base class for all other components
//***********************************************************************

#pragma once

enum ComponentTypes
{
	C_TRANSFORM = 0, C_SPRITE, C_INPUT, C_NETWORK
};

class GameObject;

class Component
{
private:

	GameObject * m_pOwner;

public:

	// Start, loop, end
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Terminate() = 0;
	virtual unsigned int GetType() const = 0;

	// Getter/Setter for owner
	GameObject * GetOwner() const { return m_pOwner; }
	void SetOwner(GameObject * _owner) { m_pOwner = _owner; }

};