//***********************************************************************
//	File:		ServerManager.cpp
//	Author:		Michael Sciortino
//	Purpose:	For all things networking related to the server side
//***********************************************************************

#include <sstream>
#include "ServerManager.h"
#include "MessageTypes.h"
#include "GameplayState.h"
#include "../Source/Game.h"
#include "../Source/GameObject.h"
#include "../Source/Component.h"
#include "../Source/InputController.h"
#include "../Source/NetworkView.h"
#include "../Raknet/BitStream.h"

/////////////////////////////
//	Initialize
//	 -Startup
bool ServerManager::Initialize(unsigned short _port, unsigned short _maxClients)
{
	m_usPort = _port;
	m_usMaxClients = _maxClients;
	m_uiUniqueID = 0;
	m_uiGameObjectCounter = 0;
	m_fAliveTimer = 0.0f;

	m_pPeer = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd(m_usPort, 0);
	RakNet::StartupResult result = m_pPeer->Startup(m_usMaxClients, &sd, 1);
	m_pPeer->SetMaximumIncomingConnections(m_usMaxClients);

	m_bRunning = true;

	return true;
}


/////////////////////////////
//	ParseMessages
//	 -Take in messages and interpret them
void ServerManager::ParseMessages()
{
	RakNet::Packet *packet;
	for (packet = m_pPeer->Receive(); packet; m_pPeer->DeallocatePacket(packet), packet = m_pPeer->Receive())
	{
		Game * game = Game::GetInstance();
		GameplayState * gpState = m_pGameplayState;
		switch (packet->data[0])
		{
			case ID_IS_ALIVE:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				unsigned int aliveID;
				bsIn.Read(aliveID);
				m_mAliveFlags[aliveID] = true;

				break;
			}
			case ID_NEW_INCOMING_CONNECTION:
			{
				gpState->SetCurrentMessage("A connection is incoming.");

				// Increment the id counter and send this to the connected client
				m_uiUniqueID++;
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_ASSIGN_ID);
				bsOut.Write(m_uiUniqueID);
				SendToAddress(&bsOut, packet->systemAddress);
				m_mAddresses[m_uiUniqueID] = packet->systemAddress;
				m_mAliveFlags[m_uiUniqueID] = true;
				m_fAliveTimer = 0.0f;

				break;
			}
			case ID_DISCONNECT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				// Stop keeping track of this clients address
				unsigned int disconnectID;
				bsIn.Read(disconnectID);
				m_mAddresses.erase(disconnectID);
				m_mAliveFlags.erase(disconnectID);

				// Destroy the player object related to this player
				gpState->DestroyPlayer(disconnectID);

				RakNet::BitStream bsDestroyPlayer;
				bsDestroyPlayer.Write((RakNet::MessageID)ID_DESTROY_PLAYER);
				bsDestroyPlayer.Write(disconnectID);
				SendToAll(&bsDestroyPlayer);

				break;
			}
			case ID_CREATE_PLAYER:
			{
				gpState->SetCurrentMessage("Creating player.\n");

				// Increment the id counter and send this to the connected client
				m_uiGameObjectCounter++;

				// Read in the data and create a player
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				char sprite;
				int x, y;
				unsigned int id;
				bsIn.Read(sprite);
				bsIn.Read(x);
				bsIn.Read(y);
				bsIn.Read(id);
				gpState->CreatePlayer(m_uiGameObjectCounter, sprite, x, y, id);

				// Send the data to all clients so they each have a player
				RakNet::BitStream bsCreatePlayer;
				bsCreatePlayer.Write((RakNet::MessageID)ID_CREATE_PLAYER);
				bsCreatePlayer.Write(sprite);
				bsCreatePlayer.Write(x);
				bsCreatePlayer.Write(y);
				bsCreatePlayer.Write(id);
				bsCreatePlayer.Write(m_uiGameObjectCounter);
				SendToAll(&bsCreatePlayer);

				break;
			}
			case ID_UPDATE_INPUT:
			{
				RakNet::BitStream bsInput(packet->data, packet->length, false);
				bsInput.IgnoreBytes(sizeof(RakNet::MessageID));
				unsigned int owner;
				bool W, A, S, D;
				bsInput.Read(owner);
				bsInput.Read(W);
				bsInput.Read(A);
				bsInput.Read(S);
				bsInput.Read(D);

				unsigned int numObjects = gpState->GetNumGameObjects();
				for (unsigned int i = 0; i < numObjects; ++i)
				{
					GameObject * obj = gpState->GetGameObjectByIndex(i);
					if (obj->HasComponent(C_INPUT))
					{
						InputController * input = (InputController*)obj->GetComponent(C_INPUT);
						if (input->GetID() == owner)
						{
							input->SetKeyDown('W', W);
							input->SetKeyDown('A', A);
							input->SetKeyDown('S', S);
							input->SetKeyDown('D', D);
						}
					}
				}

				break;
			}
			case ID_GAME_SNAPSHOT:
			{
				RakNet::BitStream bsGameState;
				bsGameState.Write((RakNet::MessageID)ID_GAME_SNAPSHOT);
				unsigned int numObjects = gpState->GetNumGameObjects();
				bsGameState.Write(numObjects);
				for (unsigned int i = 0; i < numObjects; ++i)
				{
					 GameObject * obj = gpState->GetGameObjectByIndex(i);
					 unsigned int size = obj->GetSerialSize();
					 char * buffer = new char[size];
					 memset(buffer, 0, size);
					 obj->Serialize(buffer);
					 bsGameState.Write(size);
					 bsGameState.Write(buffer, size);
					 delete[] buffer;
				}
				SendToAddress(&bsGameState, packet->systemAddress);

				break;
			}
			case ID_ENTITY_MESSAGE:
			{
				// Read in the packet data and find out how many objects there are
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				// Read in the id of the entity to update
				unsigned int entityID;
				bsIn.Read(entityID);

				// Find this object and parse the message if it exists
				GameObject * entity = gpState->GetGameObjectByID(entityID);
				if (entity)
				{
				  NetworkView * nwView = (NetworkView*)entity->GetComponent(C_NETWORK);
				  nwView->ParseMessage(packet);
				}

				break;
			}
			default:
			{
				std::stringstream temp;
				temp << "Message with identifier ";
				temp << packet->data[0];
				temp << "has arrived.\n";
				gpState->SetCurrentMessage(temp.str());
				break;
			}
		}
	}
}

/////////////////////////////
//	CheckConnections
//	 -Make sure no clients have timed out and remove the ones that have
void ServerManager::CheckConnections()
{
	m_xTime.Signal();
	m_fAliveTimer += (float)m_xTime.Delta();
	if (m_fAliveTimer > MAX_TIMEOUT)
	{
		for (auto iter = m_mAliveFlags.begin(); iter != m_mAliveFlags.end(); ++iter)
		{
			if (!(*iter).second)
			{
				// Stop keeping track of this clients address
				float time = (*iter).second;
				unsigned int disconnectID = (*iter).first;
				m_mAddresses.erase(disconnectID);

				// Destroy the player object related to this player
				m_pGameplayState->DestroyPlayer(disconnectID);

				RakNet::BitStream bsDestroyPlayer;
				bsDestroyPlayer.Write((RakNet::MessageID)ID_DESTROY_PLAYER);
				bsDestroyPlayer.Write(disconnectID);
				SendToAll(&bsDestroyPlayer);

				m_pGameplayState->SetCurrentMessage("A player has lost connection.");

				--iter;
				m_mAliveFlags.erase(disconnectID);
				continue;
			}
			(*iter).second = false;
		}
		m_fAliveTimer = 0.0f;
	}
}

/////////////////////////////
//	SendToAddress
// Send a message to a specific address
void ServerManager::SendToAddress(RakNet::BitStream * _bsOut, RakNet::SystemAddress _address)
{
	m_pPeer->Send(_bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, _address, false);
}

/////////////////////////////
//	SendToAll
// Send a message to all clients connected
void ServerManager::SendToAll(RakNet::BitStream * _bsOut)
{
	for (auto iter = m_mAddresses.begin(); iter != m_mAddresses.end(); ++iter)
	{
		SendToAddress(_bsOut, (*iter).second);
	}
}


/////////////////////////////
//	Terminate
//	 -Cleanup & shutdown
void ServerManager::Terminate()
{
	RakNet::BitStream bsShutdown;
	bsShutdown.Write((RakNet::MessageID)ID_SERVER_SHUTDOWN);
	SendToAll(&bsShutdown);

	m_pPeer->Shutdown(10);
	RakNet::RakPeerInterface::DestroyInstance(m_pPeer);
}