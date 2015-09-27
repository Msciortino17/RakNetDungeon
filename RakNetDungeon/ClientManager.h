//***********************************************************************
//	File:		ClientManager.h
//	Author:		Michael Sciortino
//	Purpose:	For all things networking related to the client side
//***********************************************************************

#pragma once

#define WIN32_LEAN_AND_MEAN 

#include <string>
#include "../Raknet/RakPeerInterface.h"
#include "../Raknet/RakNetTypes.h"  // MessageID
#include "../Raknet/BitStream.h"

class GameplayState;

class ClientManager
{
private:

	// Holds most of the raknet functions this class will be using
	RakNet::RakPeerInterface * m_pPeer;

	// IP address of the server to connect to for clients
	std::string m_sIPAddress;

	// The port of the server
	unsigned short m_usPort;

	// The id of this user
	unsigned int m_uiUniqueID;

	// Whether or not the manager is running and needs to parse messages
	bool m_bRunning;

	// Reference to the game that holds this manager
	GameplayState * m_pGameplayState;

	// Address of the server
	RakNet::SystemAddress m_aServerAddress;

public:

	// Startup and assign the ip and port
	bool Initialize(std::string _ip, unsigned short _port);

	// Cleanup and shutdown
	void Terminate();

	void ParseMessages();

	// Getter/Setter for running
	bool GetRunning() const { return m_bRunning; }
	void SetRunning(bool _running) { m_bRunning = _running; }
	
	// Send a message to the server
	void SendToServer(RakNet::BitStream * _bsOut);

	// Send a message to a specific address
	void SendToAddress(RakNet::BitStream * _bsOut, RakNet::SystemAddress _address);

	// Get the unique id for this client
	unsigned int GetID() const { return m_uiUniqueID; }

	// Getter/Setter for gameplay state
	GameplayState * GetGame() const { return m_pGameplayState; }
	void SetGame(GameplayState * _game) { m_pGameplayState = _game; }

};