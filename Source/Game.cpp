//***********************************************************************
//	File:		Game.cpp
//	Author:		Michael Sciortino
//	Purpose:	Logic for playing the game
//***********************************************************************

#include "Game.h"
#include "GameObject.h"
#include "Player.h"
#include "InputManager.h"
#include "Component.h"
#include "Transform.h"
#include "Sprite.h"
#include "InputController.h"
#include "../Raknet/BitStream.h"
#include "../RakNetDungeon/GameplayState.h"
#include "../RakNetDungeon/ServerManager.h"
#include "../RakNetDungeon/ClientManager.h"
#include <iostream>

#define SERVER_PORT 6000

#pragma region Singleton methods
//Instantiate the static member for the games instance
Game* Game::s_pInstance = nullptr;

/////////////////////////////////////
//	GetInstance
//		-Allocate ONE instance and return it
Game* Game::GetInstance(void)
{
	if (s_pInstance == nullptr)
	{
		s_pInstance = new Game;
	}

	return s_pInstance;
}

/////////////////////////////////////
//	DeleteInstance
//		-Deallocate ONE instance
void Game::DeleteInstance(void)
{
	delete s_pInstance;
	s_pInstance = nullptr;
}
#pragma endregion

void ServerThread(GameplayState * _game);
std::string getLocalIP();

/////////////////////////////
//	Initialize
//	 -Startup
void Game::Initialize()
{
	m_bRunning = true;

	m_iMenuCursor = 0;
	m_bIsHost = false;
	m_bInMenu = true;
	m_pServerGame = nullptr;
	m_pClientGame = nullptr;

	InputManager * input = InputManager::GetInstance();
	input->Initialize(GetConsoleWindow());

	DrawMenu();
}

/////////////////////////////
//	Update
//	 -Main loop
void Game::Update()
{
	// Input
	InputManager * input = InputManager::GetInstance();
	input->Update();

	m_xTime.Signal();
	m_fDeltaTime = (float)m_xTime.Delta();

	if (m_bInMenu)
	{
		// Do menu stuff
		if (input->IsKeyPressed(Key::Up))
		{
			m_iMenuCursor--;
			if (m_iMenuCursor < 0)
			{
				m_iMenuCursor = 3;
			}
			DrawMenu();
		}

		if (input->IsKeyPressed(Key::Down))
		{
			m_iMenuCursor++;
			if (m_iMenuCursor > 3)
			{
				m_iMenuCursor = 0;
			}
			DrawMenu();
		}

		if (input->IsKeyPressed(Key::Enter))
		{
			ProcessMenuOption();
		}
	}
	else
	{
		// In game stuff here
		m_pClientGame->UpdateClient();
	}

	// Exit if pressed escape
	if (InputManager::GetInstance()->IsKeyPressed(Key::Escape))
	{
		m_bRunning = false;
	}
}

/////////////////////////////
//	Terminate
//	 -Cleanup & shutdown
void Game::Terminate()
{
	m_bRunning = false;

	if (m_bIsHost)
	{
		m_pClientGame->Terminate();
		m_pServerGame->SetRunning(false);
		m_tServerThread.join();
		m_pServerGame->Terminate();
		delete m_pServerGame;
		delete m_pClientGame;
	}
	else
	{
		if (m_pClientGame)
		{
			m_pClientGame->Terminate();
			delete m_pClientGame;
		}
	}


	// Input
	InputManager * input = InputManager::GetInstance();
	input->Terminate();
	InputManager::DeleteInstance();
}


/////////////////////////////
//	DrawMenu
//	 -Draw the options for the menu
void Game::DrawMenu()
{
	// Clear the screen
	system("cls");

	std::cout << "     RakNet     \n";

	std::cout << "   Create game  ";
	m_iMenuCursor == 0 ? std::cout << " <\n" : std::cout << "\n";

	std::cout << "    Join game   ";
	m_iMenuCursor == 1 ? std::cout << " <\n" : std::cout << "\n";

	std::cout << "   Change Name   ";
	m_iMenuCursor == 2 ? std::cout << " <\n" : std::cout << "\n";

	std::cout << "      Exit       ";
	m_iMenuCursor == 3 ? std::cout << " <\n" : std::cout << "\n";
}

/////////////////////////////
//	ProcessMenuOption
//	 -Do something based on what option on the menu was selected
void Game::ProcessMenuOption()
{
	switch (m_iMenuCursor)
	{
		case 0: // Create a server
		{
			// Create the server
			m_pServerGame = new GameplayState;
			m_pServerGame->StartServer();
			ServerManager * server = new ServerManager;
			server->Initialize(SERVER_PORT, 4);
			server->SetGame(m_pServerGame);
			m_pServerGame->SetServerManager(server);
			m_tServerThread = std::thread(ServerThread, m_pServerGame);

			// Join as a client
			JoinGame("127.0.0.1");

			// Update game state
			m_bInMenu = false;
			m_bIsHost = true;

			break;
		}
		case 1: // Join a server
		{
			// Retrieve info from the user
			std::string ip;
			system("cls");
			std::cout << "Connect to: ";
			std::cin >> ip;

			// Join as a client
			JoinGame(ip);

			// Update game state
			m_bInMenu = false;

			break;
		}
		case 2: // Enter name
		{
			std::string temp;
			system("cls");
			std::cout << "Enter your name: ";
			std::cin >> temp;
			DrawMenu();
			break;
		}
		case 3: // Exit program
		{
			m_bRunning = false;
			break;
		}
	}
}

/////////////////////////////
//	JoinGame
//	 -Has the client startup and connect to the given ip
void Game::JoinGame(std::string _ip)
{
	m_pClientGame = new GameplayState;
	m_pClientGame->StartClient();
	ClientManager * client = new ClientManager;
	client->Initialize(_ip, SERVER_PORT);
	client->SetGame(m_pClientGame);
	m_pClientGame->SetClientManager(client);
}

/////////////////////////////
//	ServerThread
//	 -The function that will run in it's own thread. Will act as the server
void ServerThread(GameplayState * _game)
{
	while (_game->GetRunning())
	{
		_game->UpdateServer();
	}
}
