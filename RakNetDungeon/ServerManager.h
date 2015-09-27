//***********************************************************************
//	File:		ServerManager.h
//	Author:		Michael Sciortino
//	Purpose:	For all things networking related to the server side
//***********************************************************************

#pragma once

#define WIN32_LEAN_AND_MEAN 

#include "../Source/XTime.h"
#include <string>
#include <map>
#include "../Raknet/RakPeerInterface.h"
#include "../Raknet/RakNetTypes.h"  // MessageID

#define MAX_TIMEOUT 3.0f

class GameplayState;

class ServerManager
{
private:

	// Holds most of the raknet functions this class will be using
	RakNet::RakPeerInterface * m_pPeer;

	// Maximum number of clients allowed to connect for server
	unsigned short m_usMaxClients;

	// The port of the server
	unsigned short m_usPort;

	// The id of this user
	unsigned int m_uiUniqueID;

	// Whether or not the manager is running and needs to parse messages
	bool m_bRunning;

	// Reference to the game that holds this manager
	GameplayState * m_pGameplayState;

	// Keep track of each clients address
	std::map<unsigned int, RakNet::SystemAddress> m_mAddresses;

	// Keep track of how long a client has gone without sending an is alive message
	std::map<unsigned int, bool> m_mAliveFlags;

	// For keeping track of time
	XTime m_xTime;
	float m_fAliveTimer;

	// Used for assignign IDs to game objects
	unsigned int m_uiGameObjectCounter;

public:

	// Startup and assign the ip and port
	bool Initialize(unsigned short _port, unsigned short _maxClients);

	// Cleanup and shutdown
	void Terminate();

	void ParseMessages();
	void CheckConnections();

	// Getter/Setter for running
	bool GetRunning() const { return m_bRunning; }
	void SetRunning(bool _running) { m_bRunning = _running; }

	// Send a message to a specific address
	void SendToAddress(RakNet::BitStream * _bsOut, RakNet::SystemAddress _address);

	// Send a message to all clients connected
	void SendToAll(RakNet::BitStream * _bsOut);

	// Get the unique id for this client
	unsigned int GetID() const { return m_uiUniqueID; }

	// Getter/Setter for gameplay state
	GameplayState * GetGame() const { return m_pGameplayState; }
	void SetGame(GameplayState * _game) { m_pGameplayState = _game; }

};