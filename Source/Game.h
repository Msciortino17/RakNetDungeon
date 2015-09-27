//***********************************************************************
//	File:		Game.h
//	Author:		Michael Sciortino
//	Purpose:	Logic for playing the game
//***********************************************************************

#pragma once

#define WIN32_LEAN_AND_MEAN 

#include <vector>
#include <string>
#include <thread>
#include "XTime.h"

class GameplayState;

class Game
{
private:
	static Game * s_pInstance;			// Singleton reference

	Game() = default;
	~Game() = default;

	Game(const Game&) = delete;					//Copy Constructor
	Game& operator= (const Game&) = delete;		//Assignment operator

	// Whether or not the game is still going
	bool m_bRunning;

	// Keep track if this computer is hosting the server
	bool m_bIsHost;

	// For main menu
	int m_iMenuCursor;
	bool m_bInMenu;

	float m_fDeltaTime;
	XTime m_xTime;

	// Networking
	std::thread m_tServerThread;
	GameplayState * m_pServerGame;
	GameplayState * m_pClientGame;

	// Helper methods
	void DrawMenu();
	void ProcessMenuOption();
	void JoinGame(std::string _ip);

public:
	static Game* GetInstance(void);				// Singleton accessor
	static void DeleteInstance(void);			// Singleton deletion

	void Initialize();
	void Update();
	void Terminate();
	
	/*************************
	*      Accessors         *
	**************************/
	bool GetRunning() const { return m_bRunning; }
	bool GetIsHost() const { return m_bIsHost; }
	float GetDT() const { return m_fDeltaTime; }

	/*************************
	*      Mutators          *
	**************************/
	void SetRunning(bool _running) { m_bRunning = _running; }
	void SetIsHost(bool _host) { m_bIsHost = _host; }

};