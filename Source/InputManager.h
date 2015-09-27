//***********************************************************************
//	File:		InputManager.h
//	Author:		Michael Sciortino
//				Special thanks to Full Sail for providing the SGD wrappers I used
//				for reference when making this.
//	Purpose:	Will handle everything to do with mouse/keyboard input and will have
//				functions for the rest of the program to interact with it.
//***********************************************************************

#pragma once

#include <Windows.h>
#include "Keys.h"

class InputManager
{
	static InputManager * s_pInstance;			// Singleton reference

	InputManager() = default;
	~InputManager() = default;

	InputManager(const InputManager&) = delete;					//Copy Constructor
	InputManager& operator= (const InputManager&) = delete;		//Assignment operator

	HWND m_pWindow;

	float m_fMouseX;
	float m_fMouseY;

	float m_fMouseXPrev;
	float m_fMouseYPrev;

	float m_fMousewheel;

	// Key States
	enum
	{
		Bit_Current = 0x80,
		Bit_Previous = 0x40,
		Bit_Toggled = 0x01,
		Mask_State = 0xC0,
		State_Pressed = Bit_Current,
		State_Released = Bit_Previous,
	};

	unsigned char m_aKeyboard[256];

	// KEY NAME HELPER METHOD
	static const char* GetAllKeyNames(void);	// all keyboard/mouse key names in a 1D array (wchar_t[256][32])

public:

	static InputManager* GetInstance(void);				// Singleton accessor
	static void DeleteInstance(void);					// Singleton deletion

	bool Initialize(HWND _window);							// Start, Loop, End
	int Update();
	void Terminate();

	bool IsKeyPressed(Key _key);
	bool IsKeyDown(Key _key);
	bool IsKeyUp(Key _key);
	bool IsKeyReleased(Key _key);

	bool IsAnyKeyPressed();
	Key GetAnyKeyPressed();
	bool IsAnyKeyDown();
	Key GetAnyKeyDown();

	const char* GetKeyName(Key _key);

	float GetMouseX() const { return m_fMouseX; }
	float GetMouseY() const { return m_fMouseY; }
	float GetMouseXPrev() const { return m_fMouseXPrev; }
	float GetMouseYPrev() const { return m_fMouseYPrev; }

	float GetMouseWheelData();
	void MouseWheelIn(WPARAM _wParam);
	void ResetMouseWheel() { m_fMousewheel = 0.0f; }

	bool SetMousePos(float _x, float _y);

};