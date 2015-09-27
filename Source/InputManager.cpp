//***********************************************************************
//	File:		InputManager.cpp
//	Author:		Michael Sciortino
//				Special thanks to Full Sail for providing the SGD wrappers I used
//				for reference when making this.
//	Purpose:	Will handle everything to do with mouse/keyboard input and will have
//				functions for the rest of the program to interact with it.
//***********************************************************************

#include "InputManager.h"

#define WIN32_LEAN_AND_MEAN
#include <cstring>
#include <cstdio>

#pragma region Singleton methods
//Instantiate the static member for the games instance
InputManager* InputManager::s_pInstance = nullptr;

/////////////////////////////////////
//	GetInstance
//		-Allocate ONE instance and return it
InputManager* InputManager::GetInstance(void)
{
	if (s_pInstance == nullptr)
	{
		s_pInstance = new InputManager;
	}

	return s_pInstance;
}

/////////////////////////////////////
//	DeleteInstance
//		-Deallocate ONE instance
void InputManager::DeleteInstance(void)
{
	delete s_pInstance;
	s_pInstance = nullptr;
}
#pragma endregion


/////////////////////////////////////
//	Initialize
//		-Startup, set the array to 0
bool InputManager::Initialize(HWND _window)
{
	memset(m_aKeyboard, 0, sizeof(m_aKeyboard));

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(_window, &ptMouse);

	m_fMouseX = (float)ptMouse.x;
	m_fMouseY = (float)ptMouse.y;

	RECT rClip = {};
	GetClipCursor(&rClip);
	m_fMouseX -= (float)rClip.left;
	m_fMouseY -= (float)rClip.top;

	m_fMouseXPrev = m_fMouseX;
	m_fMouseYPrev = m_fMouseY;

	m_pWindow = _window;

	return true;
}

/////////////////////////////////////
//	Update
//		-Update mouse position and read all values for keys
int InputManager::Update()
{
	m_fMouseXPrev = m_fMouseX;
	m_fMouseYPrev = m_fMouseY;

	POINT mouse = {};
	RECT clip = {};
	GetCursorPos(&mouse);
	ScreenToClient(m_pWindow, &mouse);
	GetClipCursor(&clip);
	mouse.x -= clip.left;
	mouse.y -= clip.top;

	m_fMouseX = (float)mouse.x;
	m_fMouseY = (float)mouse.y;

	// Poll mouse / keyboard button state ONLY if window has focus
	BYTE keyboard[256] = {};
	GetKeyState(0);

	if (GetForegroundWindow() == m_pWindow)
		GetKeyboardState(keyboard);

	// Ignore: 0, 7, 10-11, 14-15, 21-26, 28-31, 58-64, 94, 136-143, 146-159, 184-185, 193-218, 224-225, 227-245, 252, 255
#define UPDATE_KEY( key )			m_aKeyboard[ key ] = ((m_aKeyboard[ key ] & Bit_Current) >> 1) | (keyboard[ key ])
#define UPDATE_KEYS( start, stop )	for( int i = start; i <= stop; i++ )	UPDATE_KEY(i);

	UPDATE_KEYS(1, 6);
	UPDATE_KEYS(8, 9);
	UPDATE_KEYS(12, 13);
	UPDATE_KEYS(16, 20);
	UPDATE_KEY(27);
	UPDATE_KEYS(32, 57);
	UPDATE_KEYS(65, 93);
	UPDATE_KEYS(95, 135);
	UPDATE_KEYS(144, 145);
	UPDATE_KEYS(160, 183);
	UPDATE_KEYS(186, 192);
	UPDATE_KEYS(219, 223);
	UPDATE_KEY(226);
	UPDATE_KEYS(246, 251);
	UPDATE_KEYS(253, 254);

#undef UPDATE_KEYS
#undef UPDATE_KEY

	return 1;
}

/////////////////////////////////////
//	Terminate
//		-Cleanup and shutdown
void InputManager::Terminate()
{

}

/////////////////////////////////////
//	IsKeyPressed
//		-Returns true if the given key was pressed
bool InputManager::IsKeyPressed(Key _key)
{
	return (m_aKeyboard[(int)_key] & Mask_State) == State_Pressed;
}

/////////////////////////////////////
//	IsKeyDown
//		-Returns true if the given key is being pressed at all
bool InputManager::IsKeyDown(Key _key)
{
	return (m_aKeyboard[(int)_key] & Bit_Current) == Bit_Current;
}

/////////////////////////////////////
//	IsKeyUp
//		-Returns true if the given key is not being pressed
bool InputManager::IsKeyUp(Key _key)
{
	return (m_aKeyboard[(int)_key] & Bit_Current) != Bit_Current;
}

/////////////////////////////////////
//	IsKeyReleased
//		-Returns true if the given key was just released
bool InputManager::IsKeyReleased(Key _key)
{
	return (m_aKeyboard[(int)_key] & Mask_State) == State_Released;
}

/////////////////////////////////////
//	IsAnyKeyPressed
//		-Check if any keys are pressed
bool InputManager::IsAnyKeyPressed()
{
	// Loop through every key and check if even a single one is pressed
	for (int key = 255; key >= 0; key--)
	{
		if ((m_aKeyboard[key] & Mask_State) == State_Pressed)
			return true;
	}

	// Otherwise no keys pare pressed
	return false;
}

/////////////////////////////////////
//	GetAnyKeyPressed
//		-Returns the first key that is pressed at the moment
Key InputManager::GetAnyKeyPressed()
{
	// Default to no key
	Key any = Key::None;

	// Check if any character-key is pressed
	for (int key = 255; key >= 0; key--)
	{
		if ((m_aKeyboard[key] & Mask_State) == State_Pressed)
		{
			// Magic code...
			unsigned int scan = MapVirtualKeyW((unsigned int)key, MAPVK_VK_TO_VSC);

			WCHAR ch[32];
			int result = ToUnicode((unsigned int)key, scan, m_aKeyboard, ch, 32, 0);
			if (result == 1)				// it is a character-key
				return (Key)key;
			else if (any == Key::None)		// it is the first 'any' key
				any = (Key)key;
		}
	}


	// No character-keys are down
	return any;
}

/////////////////////////////////////
//	IsAnyKeyDown
//		-Returns true if any keys are down
bool InputManager::IsAnyKeyDown()
{
	// Check if any key is down
	for (int key = 255; key >= 0; key--)
	{
		if ((m_aKeyboard[key] & Bit_Current) == Bit_Current)
			return true;
	}
	
	// No keys are down
	return false;
}

/////////////////////////////////////
//	GetAnyKeyDown
//		-Returns the first key found that is down
Key InputManager::GetAnyKeyDown()
{
	// Check if any character-key is down first (to match GetAnyCharDown)
	// otherwise, use any key
	Key any = Key::None;

	// Check if any character-key is down
	for (int key = 255; key >= 0; key--)
	{
		if ((m_aKeyboard[key] & Bit_Current) == Bit_Current)
		{
			// Attempt to translate the key into a character
			// (some keys do not convert: Shift, Ctrl, Alt, etc.)
			unsigned int scan = MapVirtualKeyW((unsigned int)key, MAPVK_VK_TO_VSC);

			WCHAR ch[32];
			int result = ToUnicode((unsigned int)key, scan, m_aKeyboard, ch, 32, 0);
			if (result == 1)				// it is a character-key
				return (Key)key;
			else if (any == Key::None)		// it is the first 'any' key
				any = (Key)key;
		}
	}

	// No character-keys are down
	return any;
}

/////////////////////////////////////
//	GetKeyName
//		-Translate the given key into a char array

const char* InputManager::GetKeyName(Key _key)
{
	// Access the contiguous block of names (wchar_t[256][32])
	const char* names = GetAllKeyNames();

	// Manually index into the array (skip 32 characters per key)
	return (names + ((int)_key * 32));
}

/////////////////////////////////////
//	GetInstance
//		-Allocate ONE instance and return it
bool InputManager::SetMousePos(float _x, float _y)
{
	// Attempt to move the Windows cursor
	POINT mouse = { (LONG)_x, (LONG)_y };
	ClientToScreen(m_pWindow, &mouse);
	BOOL result = SetCursorPos((int)mouse.x, (int)mouse.y);

	// Store updated mouse position
	RECT clip = {};
	GetCursorPos(&mouse);
	ScreenToClient(m_pWindow, &mouse);
	GetClipCursor(&clip);
	mouse.x -= clip.left;
	mouse.y -= clip.top;

	m_fMouseX = (float)mouse.x;
	m_fMouseY = (float)mouse.y;

	return true;
}

/////////////////////////////////////
//	GetAllKeyNames
//		-Helper method, returns every key name in a single array
const char* InputManager::GetAllKeyNames(void)
{
	// Return all the mouse / keyboard key names in a
	// contiguous block of wchar_t[256][32]
	// Literal constant string has static storage
	return
		"Unknown 0x00\0 000000000000000000"
		"Left Mouse Button\0 0000000000000"
		"Right Mouse Button\0 000000000000"
		"Unknown 0x03\0 000000000000000000"
		"Middle Mouse Button\0 00000000000"
		"X1 Mouse Button\0 000000000000000"
		"X2 Mouse Button\0 000000000000000"
		"Unknown 0x07\0 000000000000000000"
		"Backspace\0 000000000000000000000"
		"Tab\0 000000000000000000000000000"
		"Unknown 0x0A\0 000000000000000000"
		"Unknown 0x0B\0 000000000000000000"
		"Clear\0 0000000000000000000000000"
		"Enter\0 0000000000000000000000000"
		"Unknown 0x0E\0 000000000000000000"
		"Unknown 0x0F\0 000000000000000000"
		"Shift\0 0000000000000000000000000"
		"Control\0 00000000000000000000000"
		"Alt\0 000000000000000000000000000"
		"Pause\0 0000000000000000000000000"
		"Caps Lock\0 000000000000000000000"
		"Unknown 0x15\0 000000000000000000"
		"Unknown 0x16\0 000000000000000000"
		"Unknown 0x17\0 000000000000000000"
		"Unknown 0x18\0 000000000000000000"
		"Unknown 0x19\0 000000000000000000"
		"Unknown 0x1A\0 000000000000000000"
		"Escape\0 000000000000000000000000"
		"Unknown 0x1C\0 000000000000000000"
		"Unknown 0x1D\0 000000000000000000"
		"Unknown 0x1E\0 000000000000000000"
		"Unknown 0x1F\0 000000000000000000"
		"Space\0 0000000000000000000000000"
		"Page Up\0 00000000000000000000000"
		"Page Down\0 000000000000000000000"
		"End\0 000000000000000000000000000"
		"Home\0 00000000000000000000000000"
		"Left Arrow\0 00000000000000000000"
		"Up Arrow\0 0000000000000000000000"
		"Right Arrow\0 0000000000000000000"
		"Down Arrow\0 00000000000000000000"
		"Select\0 000000000000000000000000"
		"Print\0 0000000000000000000000000"
		"Execute\0 00000000000000000000000"
		"Print Screen\0 000000000000000000"
		"Insert\0 000000000000000000000000"
		"Delete\0 000000000000000000000000"
		"Help\0 00000000000000000000000000"
		"0\0 00000000000000000000000000000"
		"1\0 00000000000000000000000000000"
		"2\0 00000000000000000000000000000"
		"3\0 00000000000000000000000000000"
		"4\0 00000000000000000000000000000"
		"5\0 00000000000000000000000000000"
		"6\0 00000000000000000000000000000"
		"7\0 00000000000000000000000000000"
		"8\0 00000000000000000000000000000"
		"9\0 00000000000000000000000000000"
		"Unknown 0x3A\0 000000000000000000"
		"Unknown 0x3B\0 000000000000000000"
		"Unknown 0x3C\0 000000000000000000"
		"Unknown 0x3D\0 000000000000000000"
		"Unknown 0x3E\0 000000000000000000"
		"Unknown 0x3F\0 000000000000000000"
		"Unknown 0x3A\0 000000000000000000"
		"A\0 00000000000000000000000000000"
		"B\0 00000000000000000000000000000"
		"C\0 00000000000000000000000000000"
		"D\0 00000000000000000000000000000"
		"E\0 00000000000000000000000000000"
		"F\0 00000000000000000000000000000"
		"G\0 00000000000000000000000000000"
		"H\0 00000000000000000000000000000"
		"I\0 00000000000000000000000000000"
		"J\0 00000000000000000000000000000"
		"K\0 00000000000000000000000000000"
		"L\0 00000000000000000000000000000"
		"M\0 00000000000000000000000000000"
		"N\0 00000000000000000000000000000"
		"O\0 00000000000000000000000000000"
		"P\0 00000000000000000000000000000"
		"Q\0 00000000000000000000000000000"
		"R\0 00000000000000000000000000000"
		"S\0 00000000000000000000000000000"
		"T\0 00000000000000000000000000000"
		"U\0 00000000000000000000000000000"
		"V\0 00000000000000000000000000000"
		"W\0 00000000000000000000000000000"
		"X\0 00000000000000000000000000000"
		"Y\0 00000000000000000000000000000"
		"Z\0 00000000000000000000000000000"
		"Left Windows\0 000000000000000000"
		"Right Windows\0 00000000000000000"
		"Applications\0 000000000000000000"
		"Unknown 0x5E\0 000000000000000000"
		"Sleep\0 0000000000000000000000000"
		"NumPad 0\0 0000000000000000000000"
		"NumPad 1\0 0000000000000000000000"
		"NumPad 2\0 0000000000000000000000"
		"NumPad 3\0 0000000000000000000000"
		"NumPad 4\0 0000000000000000000000"
		"NumPad 5\0 0000000000000000000000"
		"NumPad 6\0 0000000000000000000000"
		"NumPad 7\0 0000000000000000000000"
		"NumPad 8\0 0000000000000000000000"
		"NumPad 9\0 0000000000000000000000"
		"Multiply\0 0000000000000000000000"
		"Add\0 000000000000000000000000000"
		"Separator\0 000000000000000000000"
		"Subtract\0 0000000000000000000000"
		"Decimal\0 00000000000000000000000"
		"Divide\0 000000000000000000000000"
		"F1\0 0000000000000000000000000000"
		"F2\0 0000000000000000000000000000"
		"F3\0 0000000000000000000000000000"
		"F4\0 0000000000000000000000000000"
		"F5\0 0000000000000000000000000000"
		"F6\0 0000000000000000000000000000"
		"F7\0 0000000000000000000000000000"
		"F8\0 0000000000000000000000000000"
		"F9\0 0000000000000000000000000000"
		"F10\0 000000000000000000000000000"
		"F11\0 000000000000000000000000000"
		"F12\0 000000000000000000000000000"
		"F13\0 000000000000000000000000000"
		"F14\0 000000000000000000000000000"
		"F15\0 000000000000000000000000000"
		"F16\0 000000000000000000000000000"
		"F17\0 000000000000000000000000000"
		"F18\0 000000000000000000000000000"
		"F19\0 000000000000000000000000000"
		"F20\0 000000000000000000000000000"
		"F21\0 000000000000000000000000000"
		"F22\0 000000000000000000000000000"
		"F23\0 000000000000000000000000000"
		"F24\0 000000000000000000000000000"
		"Unknown 0x88\0 000000000000000000"
		"Unknown 0x89\0 000000000000000000"
		"Unknown 0x8A\0 000000000000000000"
		"Unknown 0x8B\0 000000000000000000"
		"Unknown 0x8C\0 000000000000000000"
		"Unknown 0x8D\0 000000000000000000"
		"Unknown 0x8E\0 000000000000000000"
		"Unknown 0x8F\0 000000000000000000"
		"Num Lock\0 0000000000000000000000"
		"Scroll Lock\0 0000000000000000000"
		"Unknown 0x92\0 000000000000000000"
		"Unknown 0x93\0 000000000000000000"
		"Unknown 0x94\0 000000000000000000"
		"Unknown 0x95\0 000000000000000000"
		"Unknown 0x96\0 000000000000000000"
		"Unknown 0x97\0 000000000000000000"
		"Unknown 0x98\0 000000000000000000"
		"Unknown 0x99\0 000000000000000000"
		"Unknown 0x9A\0 000000000000000000"
		"Unknown 0x9B\0 000000000000000000"
		"Unknown 0x9C\0 000000000000000000"
		"Unknown 0x9D\0 000000000000000000"
		"Unknown 0x9E\0 000000000000000000"
		"Unknown 0x9F\0 000000000000000000"
		"Left Shift\0 00000000000000000000"
		"Right Shift\0 0000000000000000000"
		"Left Control\0 000000000000000000"
		"Right Control\0 00000000000000000"
		"Left Alt\0 0000000000000000000000"
		"Right Alt\0 000000000000000000000"
		"Browser Back\0 000000000000000000"
		"Browser Forward\0 000000000000000"
		"Browser Refresh\0 000000000000000"
		"Browser Stop\0 000000000000000000"
		"Browser Search\0 0000000000000000"
		"Browser Favorites\0 0000000000000"
		"Browser Home\0 000000000000000000"
		"Volume Mute\0 0000000000000000000"
		"Volume Down\0 0000000000000000000"
		"Volume Up\0 000000000000000000000"
		"Media Next Track\0 00000000000000"
		"Media Previous Track\0 0000000000"
		"Media Stop\0 00000000000000000000"
		"Media Play / Pause\0 000000000000"
		"Launch Mail\0 0000000000000000000"
		"Select Media\0 000000000000000000"
		"Launch Application 1\0 0000000000"
		"Launch Application 2\0 0000000000"
		"Unknown 0xB8\0 000000000000000000"
		"Unknown 0xB9\0 000000000000000000"
		"Semicolon\0 000000000000000000000"
		"Plus\0 00000000000000000000000000"
		"Comma\0 0000000000000000000000000"
		"Minus\0 0000000000000000000000000"
		"Period\0 000000000000000000000000"
		"Question\0 0000000000000000000000"
		"Tilde\0 0000000000000000000000000"
		"Unknown 0xC1\0 000000000000000000"
		"Unknown 0xC2\0 000000000000000000"
		"Unknown 0xC3\0 000000000000000000"
		"Unknown 0xC4\0 000000000000000000"
		"Unknown 0xC5\0 000000000000000000"
		"Unknown 0xC6\0 000000000000000000"
		"Unknown 0xC7\0 000000000000000000"
		"Unknown 0xC8\0 000000000000000000"
		"Unknown 0xC9\0 000000000000000000"
		"Unknown 0xCA\0 000000000000000000"
		"Unknown 0xCB\0 000000000000000000"
		"Unknown 0xCC\0 000000000000000000"
		"Unknown 0xCD\0 000000000000000000"
		"Unknown 0xCE\0 000000000000000000"
		"Unknown 0xCF\0 000000000000000000"
		"Unknown 0xD0\0 000000000000000000"
		"Unknown 0xD1\0 000000000000000000"
		"Unknown 0xD2\0 000000000000000000"
		"Unknown 0xD3\0 000000000000000000"
		"Unknown 0xD4\0 000000000000000000"
		"Unknown 0xD5\0 000000000000000000"
		"Unknown 0xD6\0 000000000000000000"
		"Unknown 0xD7\0 000000000000000000"
		"Unknown 0xD8\0 000000000000000000"
		"Unknown 0xD9\0 000000000000000000"
		"Unknown 0xDA\0 000000000000000000"
		"Open Bracket\0 000000000000000000"
		"Pipe\0 00000000000000000000000000"
		"Close Bracket\0 00000000000000000"
		"Quote\0 0000000000000000000000000"
		"OEM 8\0 0000000000000000000000000"
		"Unknown 0xE0\0 000000000000000000"
		"Unknown 0xE1\0 000000000000000000"
		"Backslash\0 000000000000000000000"
		"Unknown 0xE3\0 000000000000000000"
		"Unknown 0xE4\0 000000000000000000"
		"Unknown 0xE5\0 000000000000000000"
		"Unknown 0xE6\0 000000000000000000"
		"Unknown 0xE7\0 000000000000000000"
		"Unknown 0xE8\0 000000000000000000"
		"Unknown 0xE9\0 000000000000000000"
		"Unknown 0xEA\0 000000000000000000"
		"Unknown 0xEB\0 000000000000000000"
		"Unknown 0xEC\0 000000000000000000"
		"Unknown 0xED\0 000000000000000000"
		"Unknown 0xEE\0 000000000000000000"
		"Unknown 0xEF\0 000000000000000000"
		"Unknown 0xF0\0 000000000000000000"
		"Unknown 0xF1\0 000000000000000000"
		"Unknown 0xF2\0 000000000000000000"
		"Unknown 0xF3\0 000000000000000000"
		"Unknown 0xF4\0 000000000000000000"
		"Unknown 0xF5\0 000000000000000000"
		"Attn\0 00000000000000000000000000"
		"CrSel\0 0000000000000000000000000"
		"ExSel\0 0000000000000000000000000"
		"Erase EOF\0 000000000000000000000"
		"Play\0 00000000000000000000000000"
		"Zoom\0 00000000000000000000000000"
		"Unknown 0xFC\0 000000000000000000"
		"PA1\0 000000000000000000000000000"
		"OEM Clear\0 000000000000000000000"
		"Unknown 0xFC\0 000000000000000000"
		;
}

/////////////////////////////////////
//	GetMouseWheelData
//		-Returns a float from -1 to 1 determining how fast the mousewheel is moving
float InputManager::GetMouseWheelData()
{
	return m_fMousewheel / 120.0f;
}

/////////////////////////////////////
//	MouseWheelIn
//		-Used for updating the mouse wheel value
void InputManager::MouseWheelIn(WPARAM _wParam)
{
	m_fMousewheel = GET_WHEEL_DELTA_WPARAM(_wParam);
}