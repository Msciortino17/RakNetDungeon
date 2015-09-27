//***********************************************************************
//	File:		GamplayStatee.h
//	Author:		Michael Sciortino
//	Purpose:	Logic for playing the game in game mode
//***********************************************************************

#pragma once

#define WIN32_LEAN_AND_MEAN 

#include <vector>
#include <string>
#include "../Raknet/BitStream.h"

class GameObject;
class ClientManager;
class ServerManager;

class GameplayState
{
	// List of all game objects
	std::vector<GameObject*> m_vGameObjects;

	// Dimensions of map
	int m_iMapWidth;
	int m_iMapHeight;

	// Whether or not to redraw the scene
	bool m_bRefresh;

	// Whether or not this game is running
	bool m_bRunning;

	// For sending is alive on the server
	float m_fAliveTimer;

	// Networking
	std::string m_sCurrentMessage;
	ClientManager * m_pClientManager;
	ServerManager * m_pServerManager;

	// Helper methods
	void DrawGame();

public:
	GameplayState() = default;
	~GameplayState() = default;

	void StartServer();
	void StartClient();
	void UpdateServer();
	void UpdateClient();
	void Terminate();

	// Gameobject factory methods
	void CreatePlayer(unsigned int _id, char _sprite, int _x, int _y, unsigned int _controllerID);
	void DestroyPlayer(unsigned int _controllerID);


	/*************************
	*      Accessors         *
	**************************/
	ServerManager * GetServerManager() const { return m_pServerManager; }
	ClientManager * GetClientManager() const { return m_pClientManager; }
	bool GetRunning() const { return m_bRunning; }
	GameObject * GetGameObjectByID(unsigned int _id);
	GameObject * GetGameObjectByIndex(unsigned int _i) { return m_vGameObjects[_i]; };
	unsigned int GetNumGameObjects() const { return m_vGameObjects.size(); }

	/*************************
	*      Mutators          *
	**************************/
	void SetRefresh(bool _refresh) { m_bRefresh = _refresh; }
	void SetCurrentMessage(std::string _message);
	void SetRunning(bool _running) { m_bRunning = _running; }
	void SetServerManager(ServerManager * _server) { m_pServerManager = _server; }
	void SetClientManager(ClientManager * _client) { m_pClientManager = _client; }

};