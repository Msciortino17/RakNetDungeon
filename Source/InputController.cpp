//***********************************************************************
//	File:		InputController.cpp
//	Author:		Michael Sciortino
//	Purpose:	Holds the input state for an object
//***********************************************************************

#include "InputController.h"

//////////////////////
//	Initialize
//	 -Startup
void InputController::Initialize()
{
}

//////////////////////
//	Update
//	-Main loop
void InputController::Update()
{
}

//////////////////////
//	Terminate
//	 -Cleanup and shutdown
void InputController::Terminate()
{
	delete this;
}

//////////////////////
//	ResetKeys
//	 -Set each key to false
void InputController::ResetKeys()
{
	for (auto iter = m_mKeys.begin(); iter != m_mKeys.end(); ++iter)
	{
		(*iter).second = false;
	}
}