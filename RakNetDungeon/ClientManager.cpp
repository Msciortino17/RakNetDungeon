//***********************************************************************
//	File:		ClientManager.cpp
//	Author:		Michael Sciortino
//	Purpose:	For all things networking related to the client side
//***********************************************************************

// TODO clean this shit up
#include "ClientManager.h"
#include "MessageTypes.h"
#include "../Source/Game.h"
#include <sstream>
#include "../Source/GameObject.h"
#include "../Source/Component.h"
#include "../Source/InputController.h"
#include "../Source/NetworkView.h"
#include "GameplayState.h"

/////////////////////////////
//	Initialize
//	 -Startup
bool ClientManager::Initialize(std::string _ip, unsigned short _port)
{
	m_usPort = _port;
	m_sIPAddress = _ip;
	m_uiUniqueID = 0;

	m_pPeer = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd;
	RakNet::StartupResult result = m_pPeer->Startup(1, &sd, 1);
	RakNet::ConnectionAttemptResult result2 = m_pPeer->Connect(m_sIPAddress.c_str(), m_usPort, 0, 0);

	m_bRunning = true;

	return true;
}

/////////////////////////////
//	ParseMessages
//	 -Take in messages and interpret them
void ClientManager::ParseMessages()
{
	RakNet::Packet *packet;
	for (packet = m_pPeer->Receive(); packet; m_pPeer->DeallocatePacket(packet), packet = m_pPeer->Receive())
	{
		Game * game = Game::GetInstance();
		GameplayState * gpState = m_pGameplayState;
		switch (packet->data[0])
		{
			case ID_DISCONNECTION_NOTIFICATION:
			{
				gpState->SetCurrentMessage("We have lost connection.");
				gpState->SetRunning(false);
				game->SetRunning(false);

				break;
			}
			case ID_ASSIGN_ID:
			{
				gpState->SetCurrentMessage("Receiving ID...\n");

				// Save the ID this was assigned and save the address of the server
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				unsigned int temp;
				bsIn.Read(temp);
				m_uiUniqueID = temp;
				m_aServerAddress = packet->systemAddress;

				// Let the server know we want to create a player
				RakNet::BitStream bsCreatePlayer;
				bsCreatePlayer.Write((RakNet::MessageID)ID_CREATE_PLAYER);
				bsCreatePlayer.Write('P');
				int coord = 0;
				bsCreatePlayer.Write(coord);
				bsCreatePlayer.Write((int)m_uiUniqueID); // y position
				bsCreatePlayer.Write((int)m_uiUniqueID); // id
				SendToServer(&bsCreatePlayer);

				break;
			}
			case ID_SERVER_SHUTDOWN:
			{
				gpState->SetRunning(false);
				break;
			}
			case ID_CREATE_PLAYER:
			{
				gpState->SetCurrentMessage("Creating player.");

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
				gpState->CreatePlayer(id, sprite, x, y, id);

				RakNet::BitStream bsGameState;
				bsGameState.Write((RakNet::MessageID)ID_GAME_SNAPSHOT);
				SendToServer(&bsGameState);

				break;
			}
			case ID_DESTROY_PLAYER:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				unsigned int disconnectID;
				bsIn.Read(disconnectID);

				// Destroy the player object related to this player
				gpState->DestroyPlayer(disconnectID);

				gpState->SetCurrentMessage("Destroyed a player.");

				break;
			}
			case ID_GAME_SNAPSHOT:
			{
				// Read in the packet data and find out how many objects there are
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				unsigned int numObjects;
				bsIn.Read(numObjects);

				for (unsigned int i = 0; i < numObjects; ++i)
				{
					 // Read in the serialized data into a buffer for this game object
					 unsigned int size;
					 bsIn.Read(size);
					 char * buffer = new char[size];
					 memset(buffer, 0, size);
					 bsIn.Read(buffer, size);

					 // Check if the object exists first and if it does, update it
					 unsigned int id;
					 memcpy(&id, &buffer[0], sizeof(id));
					 GameObject * existingObj = gpState->GetGameObjectByID(id);
					 if (existingObj)
					 {
						 existingObj->DeSerialize(buffer);
						 delete[] buffer;
						 continue;
					 }

					 // If it does not exist then make a new one TODO make this at least kind of dynamic
					 int x, y;
					 unsigned int inputID;
					 memcpy(&x, &buffer[4], sizeof(x));
					 memcpy(&y, &buffer[8], sizeof(y));
					 memcpy(&inputID, &buffer[12], sizeof(inputID));
					 gpState->CreatePlayer(id, 'P', x, y, inputID);

					 delete[] buffer;
				}

				gpState->SetCurrentMessage("Updated the game state.");

				break;
			}
			case ID_UPDATE_ENTITY:
			{
				// Read in the packet data and find out how many objects there are
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				// Read in the serialized data into a buffer for this game object
				unsigned int size;
				bsIn.Read(size);
				char * buffer = new char[size];
				memset(buffer, 0, size);
				bsIn.Read(buffer, size);

				// Check if the object exists first and if it does, update it
				unsigned int id;
				memcpy(&id, &buffer[0], sizeof(id));
				GameObject * existingObj = gpState->GetGameObjectByID(id);
				if (existingObj)
				{
					 existingObj->DeSerialize(buffer);
					 delete[] buffer;
					 break;
				}

				// If it does not exist then make a new one TODO make this at least kind of dynamic
				int x, y;
				unsigned int inputID;
				memcpy(&x, &buffer[4], sizeof(x));
				memcpy(&y, &buffer[8], sizeof(y));
				memcpy(&inputID, &buffer[12], sizeof(inputID));
				gpState->CreatePlayer(id, 'P', x, y, inputID);

				gpState->SetCurrentMessage("Updated a game object.");
				delete[] buffer;

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
			case ID_NO_FREE_INCOMING_CONNECTIONS:
			{
				gpState->SetCurrentMessage("The server is full. Could not connect.");
				game->SetRunning(false);
			}
			default:
			{
				char id = packet->data[0];
				std::stringstream temp;
				temp << "Message with identifier ";
				temp << packet->data[0];
				temp << "has arrived.\n";
				break;
			}
		}
	}
}


/////////////////////////////
//	SendToServer
//	 -Send a message to the server
void ClientManager::SendToServer(RakNet::BitStream * _bsOut)
{
	m_pPeer->Send(_bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_aServerAddress, false);
}


/////////////////////////////
//	SendToAddress
//	 -Send a message to a specific address
void ClientManager::SendToAddress(RakNet::BitStream * _bsOut, RakNet::SystemAddress _address)
{
	m_pPeer->Send(_bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, _address, false);
}


/////////////////////////////
//	Terminate
//	 -Cleanup & shutdown
void ClientManager::Terminate()
{
	RakNet::BitStream bsDisconnect;
	bsDisconnect.Write((RakNet::MessageID)ID_DISCONNECT);
	bsDisconnect.Write(m_uiUniqueID);
	SendToServer(&bsDisconnect);

	m_pPeer->Shutdown(10);
	RakNet::RakPeerInterface::DestroyInstance(m_pPeer);
}