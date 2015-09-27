//***********************************************************************
//	File:		GamplayStatee.cpp
//	Author:		Michael Sciortino
//	Purpose:	Logic for playing the game in game mode
//***********************************************************************

#include "GameplayState.h"
#include "../Source/Game.h"
#include "../Source/Player.h"
#include "../Source/InputManager.h"
#include "../Source/Component.h"
#include "../Source/Transform.h"
#include "../Source/Sprite.h"
#include "../Source/InputController.h"
#include "../Source/NetworkView.h"
#include "../RakNetDungeon/ClientManager.h"
#include "../RakNetDungeon/ServerManager.h"
#include "../RakNetDungeon/MessageTypes.h"
#include <iostream>

#define SERVER_PORT 6000

std::string getLocalIP();

/////////////////////////////
//	StartServer
//	 -Startup for the server
void GameplayState::StartServer()
{
	m_iMapWidth = 20;
	m_iMapHeight = 10;
	m_fAliveTimer = 0.0f;

	m_bRefresh = false;

	m_pClientManager = nullptr;
	m_pServerManager = nullptr;
}

/////////////////////////////
//	StartClient
//	 -Startup for the client
void GameplayState::StartClient()
{
	m_iMapWidth = 20;
	m_iMapHeight = 10;
	m_fAliveTimer = 0.0f;

	m_bRefresh = false;

	m_pClientManager = nullptr;
	m_pServerManager = nullptr;
}

/////////////////////////////
//	UpdateServer
//	 -Update loop for the server
void GameplayState::UpdateServer()
{
	// Parse messages
	m_pServerManager->ParseMessages();
	m_pServerManager->CheckConnections();

	// Update each gameobject
	for (unsigned int i = 0; i < m_vGameObjects.size(); ++i)
	{
		m_vGameObjects[i]->Update();

		// Reset input for the object
		if (m_vGameObjects[i]->HasComponent(C_INPUT))
		{
			InputController * input = (InputController*)m_vGameObjects[i]->GetComponent(C_INPUT);
			input->ResetKeys();
		}
	}
}

/////////////////////////////
//	UpdateClient
//	 -Update loop for the client
void GameplayState::UpdateClient()
{
	InputManager * input = InputManager::GetInstance();

	// Parse messages
	m_pClientManager->ParseMessages();

	// Let the server know we are still connected
	m_fAliveTimer += Game::GetInstance()->GetDT();
	if (m_fAliveTimer > 0.5f)
	{
		RakNet::BitStream bsAlive;
		bsAlive.Write((RakNet::MessageID)ID_IS_ALIVE);
		bsAlive.Write(m_pClientManager->GetID());
		m_pClientManager->SendToServer(&bsAlive);
		m_fAliveTimer = 0.0f;
	}

	// Input message
	if (input->IsAnyKeyPressed())
	{
		RakNet::BitStream bsInput;
		bsInput.Write((RakNet::MessageID)ID_UPDATE_INPUT);
		bsInput.Write(m_pClientManager->GetID());
		bsInput.Write(input->IsKeyPressed(Key::W));
		bsInput.Write(input->IsKeyPressed(Key::A));
		bsInput.Write(input->IsKeyPressed(Key::S));
		bsInput.Write(input->IsKeyPressed(Key::D));
		m_pClientManager->SendToServer(&bsInput);
	}

	if (m_bRefresh)
	{
		DrawGame();
		m_bRefresh = false;
	}
}

/////////////////////////////
//	DrawGame
//	 -Draw the map border and all game objects with a render component
void GameplayState::DrawGame()
{
	// Don't draw if this is the server
	if (m_pServerManager)
	{
		return;
	}

	// Clear the screen
	system("cls");

	// HUD
	if (Game::GetInstance()->GetIsHost())
	{
		std::cout << "ID: " << m_pClientManager->GetID() << " IP: " << getLocalIP() << "\n";
	}
	else if (m_pClientManager)
	{
		std::cout << "ID: " << m_pClientManager->GetID() << "\n";
	}

	// Top border
	std::cout << '+';
	for (int xx = 0; xx < m_iMapWidth; xx++)
	{
		std::cout << "-+";
	}

	// Grid & entities
	for (int yy = 0; yy < m_iMapHeight; yy++)
	{
		std::cout << "\n|";
		for (int xx = 0; xx < m_iMapWidth; xx++)
		{
			bool drewSprite = false;
			// Check all game objects and pick the one at xx,yy and try to render it here
			for (unsigned int i = 0; i < m_vGameObjects.size(); ++i)
			{
				if (m_vGameObjects[i]->HasComponent(C_TRANSFORM))
				{
					Transform * t = (Transform*)m_vGameObjects[i]->GetComponent(C_TRANSFORM);
					if (t->GetX() == xx && t->GetY() == yy &&
						m_vGameObjects[i]->HasComponent(C_SPRITE))
					{
						Sprite * s = (Sprite*)m_vGameObjects[i]->GetComponent(C_SPRITE);
						std::cout << s->GetCharacter();
						drewSprite = true;
					}
				}
			}

			if (!drewSprite)
			{
				std::cout << ' ';
			}
			std::cout << '|';
		}

		std::cout << "\n+";
		for (int xx = 0; xx < m_iMapWidth; xx++)
		{
			std::cout << "-+";
		}
	}

	std::cout << "\n\n" << m_sCurrentMessage;
}


/////////////////////////////
//	Terminate
//	 -Cleanup
void GameplayState::Terminate()
{
	// Terminate and delete each gameobject
	for (unsigned int i = 0; i < m_vGameObjects.size(); ++i)
	{
		m_vGameObjects[i]->Terminate();
		delete m_vGameObjects[i];
	}

	if (m_pServerManager)
	{
		m_pServerManager->Terminate();
		delete m_pServerManager;
	}

	if (m_pClientManager)
	{
		m_pClientManager->Terminate();
		delete m_pClientManager;
	}
}

/////////////////////////////
//	GetGameObjectByID
//	 -Return the game object with the matching id
GameObject * GameplayState::GetGameObjectByID(unsigned int _id)
{
	for (unsigned int i = 0; i < m_vGameObjects.size(); ++i)
	{
		if (m_vGameObjects[i]->GetID() == _id)
		{
			return m_vGameObjects[i];
		}
	}

	return nullptr;
}

/////////////////////////////
//	CreatePlayer
//	 -Create a player game object with the given info
void GameplayState::CreatePlayer(unsigned int _id, char _sprite, int _x, int _y, unsigned int _controllerID)
{
	Player * player = new Player;
	player->Initialize();

	Transform * playerT = (Transform*)player->GetComponent(C_TRANSFORM);
	playerT->SetX(_x);
	playerT->SetY(_y);

	Sprite * playerS = (Sprite*)player->GetComponent(C_SPRITE);
	playerS->SetCharacter(_sprite);
	player->SetID(_id);
	player->SetGame(this);

	InputController * input = (InputController*)player->GetComponent(C_INPUT);
	input->SetID(_controllerID);
	
	NetworkView * nwView = (NetworkView*)player->GetComponent(C_NETWORK);
	nwView->SetServerManager(m_pServerManager);
	nwView->SetClientManager(m_pClientManager);

	m_vGameObjects.push_back(player);
}


/////////////////////////////
//	DestroyPlayer
//	 -Find and remove the player with the matching player id
void GameplayState::DestroyPlayer(unsigned int _controllerID)
{
	for (unsigned int i = 0; i < m_vGameObjects.size(); ++i)
	{
		if (m_vGameObjects[i]->HasComponent(C_INPUT))
		{
			InputController * input = (InputController*)m_vGameObjects[i]->GetComponent(C_INPUT);
			if (input->GetID() == _controllerID)
			{
				m_vGameObjects[i]->Terminate();
				delete m_vGameObjects[i];
				m_vGameObjects.erase(m_vGameObjects.begin() + i);
			}
		}
	}
}

/////////////////////////////
//	SetCurrentMessage
//	 -Change the message displayed at the bottom
void GameplayState::SetCurrentMessage(std::string _message)
{
	m_sCurrentMessage = _message;
	DrawGame();
	m_bRefresh = true;
}

std::string getLocalIP()
{
	std::string localIP = "NO LOCAL IP FOUND";

	// setup Winsock so we can get the Local IP 
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	{
		return "ERROR STARTING UP WINSOCK FOR LOCAL IP";
	}

	// Get PC Name
	char PCName[80] = {};
	if (gethostname(PCName, sizeof(PCName)) == SOCKET_ERROR)
	{
		return "ERROR GETTING HOST NAME FOR LOCAL IP";
	}

	// Get the host name from the PC Name
	hostent *hostName = gethostbyname(PCName);
	if (hostName == nullptr)
	{
		return "ERROR LOOKING UP HOST FOR LOCAL IP";
	}

	// get the IP's, return the first one
	for (int i = 0; hostName->h_addr_list[i] != 0; ++i)
	{
		in_addr addr;
		memcpy(&addr, hostName->h_addr_list[i], sizeof(in_addr));
		localIP = inet_ntoa(addr);
		break;
	}

	// Clean up winsock
	WSACleanup();

	return localIP;
}