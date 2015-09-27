//***********************************************************************
//	File:		NetworkView.h
//	Author:		Michael Sciortino
//	Purpose:	For gameobjects to communicate over the network
//***********************************************************************

#pragma once

#include "Component.h"
#include "../RakNetDungeon/MessageTypes.h"
#include <vector>
#include "../Raknet/RakPeerInterface.h"
#include "../Raknet/RakNetTypes.h"  // MessageID

class ClientManager;
class ServerManager;

class NetworkView : public Component
{
private:

	ClientManager * m_pClientManager;
	ServerManager * m_pServerManager;

	std::vector<NWViewMessages> m_vRegisteredMessages;

	bool RegisteredForMessage(NWViewMessages _message);

public:

	// Start, loop, end
	void Initialize();
	void Update();
	void Terminate();

	unsigned int GetType() const { return C_NETWORK; }

	void RegisterForMessage(NWViewMessages _message) { m_vRegisteredMessages.push_back(_message); }
	void ParseMessage(RakNet::Packet * _packet);

	ClientManager * GetClientManager() const { return m_pClientManager; }
	ServerManager * GetServerManager() const { return m_pServerManager; }

	void SetClientManager(ClientManager * _client) { m_pClientManager = _client; }
	void SetServerManager(ServerManager * _server) { m_pServerManager = _server; }

	// Helper methods for creating messages
	void UpdatePositionMessage(int _x, int _y);

};