//***********************************************************************
//	File:		InputController.h
//	Author:		Michael Sciortino
//	Purpose:	Holds the input state for an object
//***********************************************************************

#pragma once

#include "Component.h"
#include <map>

class InputController : public Component
{

	std::map<char, bool> m_mKeys;
	unsigned int m_uiControllerID;

public:

	// Start, loop, end
	void Initialize();
	void Update();
	void Terminate();

	unsigned int GetType() const { return C_INPUT; }

	bool GetKeyDown(char _key) { return m_mKeys[_key]; }
	void SetKeyDown(char _key, bool _value) { m_mKeys[_key] = _value; }

	unsigned int GetID() const { return m_uiControllerID; }
	void SetID(unsigned int _id) { m_uiControllerID = _id; }

	void ResetKeys();

};