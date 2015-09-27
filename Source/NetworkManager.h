////***********************************************************************
////	File:		NetworkManager.h
////	Author:		Michael Sciortino
////	Purpose:	For all things networking
////***********************************************************************
//
//#pragma once
//
//#include <string>
//#include <map>
//#include "../Raknet/RakPeerInterface.h"
//#include "../Raknet/RakNetTypes.h"  // MessageID
//#include "../Raknet/MessageIdentifiers.h"
//
//
//
//class Game;
//
//class NetworkManager
//{
//private:
//
//	// Keeping track if this is a client/server
//	NetworkType m_nwtType;
//
//	// Holds most of the raknet functions this class will be using
//	RakNet::RakPeerInterface * m_pPeer;
//
//	// IP address of the server to connect to for clients
//	std::string m_sIPAddress;
//
//	// The id of this user
//	unsigned int m_uiUniqueID;
//
//	// The port of the server
//	unsigned short m_usPort;
//
//	// Maximum number of clients allowed to connect for server
//	unsigned short m_usMaxClients;
//
//	// Used for assignign IDs to game objects
//	unsigned int m_uiGameObjectCounter;
//
//	// Whether or not the manager is running and needs to parse messages
//	bool m_bRunning;
//
//	// Reference to the game that holds this manager
//	Game * m_pGame;
//
//	// Keep track of each client for servers and the server itself for clients
//	std::map<unsigned int, RakNet::SystemAddress> m_mAddresses;
//
//	// Helper methods to organize parsing for clients/servers
//	void ParseServerMessages(RakNet::Packet * _packet);
//	void ParseClientMessages(RakNet::Packet * _packet);
//
//public:
//
//	// Set the network type and startup everything for raknet
//	bool Initialize(NetworkType _type, std::string _ip, unsigned short _port, unsigned short _maxClients);
//
//	// Both clients and servers will need to parse messages
//	void ParseMessages();
//
//	// Cleanup and shutdown
//	void Terminate();
//
//	// For quickly identifying the type
//	bool IsClient() const { return m_nwtType == NW_CLIENT; }
//	bool IsServer() const { return m_nwtType == NW_SERVER; }
//
//	// Getter/Setter for running
//	bool GetRunning() const { return m_bRunning; }
//	void SetRunning(bool _running) { m_bRunning = _running; }
//
//	// Getter/Setter for game
//	Game * GetGame() const { return m_pGame; }
//	void SetGame(Game * _game) { m_pGame = _game; }
//
//	void SendToAddress(RakNet::BitStream * _bsOut, RakNet::SystemAddress _address);
//	void SendToAll(RakNet::BitStream * _bsOut);
//
//	// Getter for addresses
//	RakNet::SystemAddress GetAddress(unsigned int _id) { return m_mAddresses[_id]; }
//	unsigned int GetID() const { return m_uiUniqueID; }
//};