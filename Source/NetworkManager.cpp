////***********************************************************************
////	File:		NetworkManager.cpp
////	Author:		Michael Sciortino
////	Purpose:	For all things networking
////***********************************************************************
//
//#include "NetworkManager.h"
//#include "Game.h"
//#include "GameObject.h"
//#include "Component.h"
//#include "InputController.h"
//#include <sstream>
//#include "../Raknet/BitStream.h"
//
///////////////////////////////
////	Initialize
////	 -Startup
//bool NetworkManager::Initialize(NetworkType _type, std::string _ip, unsigned short _port, unsigned short _maxClients)
//{
//	if (_type != NW_SERVER && _type != NW_CLIENT)
//	{
//		return false;
//	}
//
//	m_nwtType = _type;
//	m_usPort = _port;
//	m_usMaxClients = _maxClients;
//	m_sIPAddress = _ip;
//	m_uiUniqueID = 0;
//	m_uiGameObjectCounter = 0;
//
//	m_pPeer = RakNet::RakPeerInterface::GetInstance();
//
//	if (IsServer())
//	{
//		RakNet::SocketDescriptor sd(m_usPort, 0);
//		RakNet::StartupResult result = m_pPeer->Startup(m_usMaxClients, &sd, 1);
//		m_pPeer->SetMaximumIncomingConnections(m_usMaxClients);
//	}
//	else if (IsClient())
//	{
//		RakNet::SocketDescriptor sd;
//		RakNet::StartupResult result = m_pPeer->Startup(1, &sd, 1);
//		RakNet::ConnectionAttemptResult result2 = m_pPeer->Connect(m_sIPAddress.c_str(), m_usPort, 0, 0);
//	}
//
//	m_bRunning = true;
//
//	return true;
//}
//
///////////////////////////////
////	ParseMessages
////	 -Take in messages and interpret them
//void NetworkManager::ParseMessages()
//{
//	RakNet::Packet *packet;
//	for (packet = m_pPeer->Receive(); packet; m_pPeer->DeallocatePacket(packet), packet = m_pPeer->Receive())
//	{
//		if (IsServer())
//		{
//			ParseServerMessages(packet);
//		}
//		else if (IsClient())
//		{
//			ParseClientMessages(packet);
//		}
//	}
//}
//
///////////////////////////////
////	Terminate
////	 -Cleanup & shutdown
//void NetworkManager::Terminate()
//{
//	m_pPeer->Shutdown(0);
//	RakNet::RakPeerInterface::DestroyInstance(m_pPeer);
//}
//
///////////////////////////////
////	ParseServerMessages
////	 -Parsing for the server side
//void NetworkManager::ParseServerMessages(RakNet::Packet * _packet)
//{
//	Game * game = m_pGame;
//	switch (_packet->data[0])
//	{
//		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
//			game->SetCurrentMessage("Another client has disconnected.\n");
//			break;
//		case ID_REMOTE_CONNECTION_LOST:
//			game->SetCurrentMessage("Another client has lost the connection.\n");
//			break;
//		case ID_REMOTE_NEW_INCOMING_CONNECTION:
//			game->SetCurrentMessage("Another client has connected.\n");
//			break;
//		case ID_CONNECTION_REQUEST_ACCEPTED:
//			game->SetCurrentMessage("Our connection request has been accepted.\n");
//			break;
//		case ID_NEW_INCOMING_CONNECTION:
//		{
//			game->SetCurrentMessage("A connection is incoming.");
//
//			// Increment the id counter and send this to the connected client
//			m_uiUniqueID++;
//			RakNet::BitStream bsOut;
//			bsOut.Write((RakNet::MessageID)ID_ASSIGN_ID);
//			bsOut.Write(m_uiUniqueID);
//			SendToAddress(&bsOut, _packet->systemAddress);
//			m_mAddresses[m_uiUniqueID] = _packet->systemAddress;
//
//			break;
//		}
//		case ID_NO_FREE_INCOMING_CONNECTIONS:
//			game->SetCurrentMessage("The server is full.\n");
//			break;
//		case ID_DISCONNECTION_NOTIFICATION:
//			game->SetCurrentMessage("A client has disconnected.\n");
//			break;
//		case ID_CONNECTION_LOST:
//			game->SetCurrentMessage("A client lost the connection.\n");
//			break;
//		case ID_CREATE_PLAYER:
//		{
//			game->SetCurrentMessage("Creating player.\n");
//
//			// Increment the id counter and send this to the connected client
//			m_uiGameObjectCounter++;
//
//			// Read in the data and create a player
//			RakNet::BitStream bsIn(_packet->data, _packet->length, false);
//			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
//			char sprite;
//			int x, y;
//			unsigned int id;
//			bsIn.Read(sprite);
//			bsIn.Read(x);
//			bsIn.Read(y);
//			bsIn.Read(id);
//			game->CreatePlayer(m_uiGameObjectCounter, sprite, x, y, id);
//
//			// Send the data to all clients so they each have a player
//			RakNet::BitStream bsCreatePlayer;
//			bsCreatePlayer.Write((RakNet::MessageID)ID_CREATE_PLAYER);
//			bsCreatePlayer.Write(sprite);
//			bsCreatePlayer.Write(x);
//			bsCreatePlayer.Write(y);
//			bsCreatePlayer.Write(id);
//			bsCreatePlayer.Write(m_uiGameObjectCounter);
//			SendToAll(&bsCreatePlayer);
//
//			break;
//		}
//		case ID_UPDATE_INPUT:
//		{
//			RakNet::BitStream bsInput(_packet->data, _packet->length, false);
//			bsInput.IgnoreBytes(sizeof(RakNet::MessageID));
//			unsigned int owner;
//			bool W, A, S, D;
//			bsInput.Read(owner);
//			bsInput.Read(W);
//			bsInput.Read(A);
//			bsInput.Read(S);
//			bsInput.Read(D);
//
//			unsigned int numObjects = game->GetNumGameObjects();
//			for (unsigned int i = 0; i < numObjects; ++i)
//			{
//				GameObject * obj = game->GetGameObjectByIndex(i);
//				if (obj->HasComponent(C_INPUT))
//				{
//					InputController * input = (InputController*)obj->GetComponent(C_INPUT);
//					if (input->GetID() == owner)
//					{
//						input->SetKeyDown('W', W);
//						input->SetKeyDown('A', A);
//						input->SetKeyDown('S', S);
//						input->SetKeyDown('D', D);
//					}
//				}
//			}
//
//			break;
//		}
//		case ID_GAME_SNAPSHOT:
//		{
//			RakNet::BitStream bsGameState;
//			bsGameState.Write((RakNet::MessageID)ID_GAME_SNAPSHOT);
//			unsigned int numObjects = game->GetNumGameObjects();
//			bsGameState.Write(numObjects);
//			for (unsigned int i = 0; i < numObjects; ++i)
//			{
//				GameObject * obj = game->GetGameObjectByIndex(i);
//				unsigned int size = obj->GetSerialSize();
//				char * buffer = new char[size];
//				memset(buffer, 0, size);
//				obj->Serialize(buffer);
//				bsGameState.Write(size);
//				bsGameState.Write(buffer, size);
//				delete[] buffer;
//			}
//			SendToAddress(&bsGameState, _packet->systemAddress);
//
//			break;
//		}
//		default:
//		{
//			std::stringstream temp;
//			temp << "Message with identifier ";
//			temp << _packet->data[0];
//			temp << "has arrived.\n";
//			game->SetCurrentMessage(temp.str());
//			break;
//		}
//	}
//}
//
///////////////////////////////
////	ParseClientMessages
////	 -Parsing for the client side
//void NetworkManager::ParseClientMessages(RakNet::Packet * _packet)
//{
//	Game * game = m_pGame;
//	switch (_packet->data[0])
//	{
//		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
//			game->SetCurrentMessage("Another client has disconnected.\n");
//			break;
//		case ID_REMOTE_CONNECTION_LOST:
//			game->SetCurrentMessage("Another client has lost the connection.\n");
//			break;
//		case ID_REMOTE_NEW_INCOMING_CONNECTION:
//			game->SetCurrentMessage("Another client has connected.\n");
//			break;
//		case ID_CONNECTION_REQUEST_ACCEPTED:
//			game->SetCurrentMessage("Our connection request has been accepted.");
//			break;
//		case ID_NEW_INCOMING_CONNECTION:
//			game->SetCurrentMessage("A connection is incoming.\n");
//			break;
//		case ID_NO_FREE_INCOMING_CONNECTIONS:
//			game->SetCurrentMessage("The server is full.\n");
//			break;
//		case ID_DISCONNECTION_NOTIFICATION:
//			game->SetCurrentMessage("We have been disconnected.\n");
//			break;
//		case ID_CONNECTION_LOST:
//			game->SetCurrentMessage("Connection lost.\n");
//			break;
//		case ID_ASSIGN_ID:
//		{
//			game->SetCurrentMessage("Receiving ID...\n");
//
//			// Save the ID this was assigned and save the address of the server
//			RakNet::BitStream bsIn(_packet->data, _packet->length, false);
//			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
//			unsigned int temp;
//			bsIn.Read(temp);
//			m_uiUniqueID = temp;
//			m_mAddresses[SERVER_ADDRESS] = _packet->systemAddress;
//			
//			// Let the server know we want to create a player
//			RakNet::BitStream bsCreatePlayer;
//			bsCreatePlayer.Write((RakNet::MessageID)ID_CREATE_PLAYER);
//			bsCreatePlayer.Write('P');
//			int coord = 0;
//			bsCreatePlayer.Write(coord);
//			bsCreatePlayer.Write((int)m_uiUniqueID);
//			bsCreatePlayer.Write((int)m_uiUniqueID);
//			SendToAddress(&bsCreatePlayer, _packet->systemAddress);
//
//			break;
//		}
//		case ID_CREATE_PLAYER:
//		{
//			game->SetCurrentMessage("Creating player.");
//
//			// Read in the data and create a player
//			RakNet::BitStream bsIn(_packet->data, _packet->length, false);
//			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
//			char sprite;
//			int x, y;
//			unsigned int id;
//			bsIn.Read(sprite);
//			bsIn.Read(x);
//			bsIn.Read(y);
//			bsIn.Read(id);
//			game->CreatePlayer(id, sprite, x, y, id);
//
//			RakNet::BitStream bsGameState;
//			bsGameState.Write((RakNet::MessageID)ID_GAME_SNAPSHOT);
//			SendToAddress(&bsGameState, _packet->systemAddress);
//
//			break;
//		}
//		case ID_GAME_SNAPSHOT:
//		{
//			// Read in the packet data and find out how many objects there are
//			RakNet::BitStream bsIn(_packet->data, _packet->length, false);
//			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
//			unsigned int numObjects;
//			bsIn.Read(numObjects);
//
//			for (unsigned int i = 0; i < numObjects; ++i)
//			{
//				// Read in the serialized data into a buffer for this game object
//				unsigned int size;
//				bsIn.Read(size);
//				char * buffer = new char[size];
//				memset(buffer, 0, size);
//				bsIn.Read(buffer, size);
//
//				// Check if the object exists first and if it does, update it
//				unsigned int id;
//				memcpy(&id, &buffer[0], sizeof(id));
//				GameObject * existingObj = game->GetGameObjectByID(id);
//				if (existingObj)
//				{
//					existingObj->DeSerialize(buffer);
//					delete[] buffer;
//					continue;
//				}
//
//				// If it does not exist then make a new one TODO make this at least kind of dynamic
//				int x, y;
//				unsigned int inputID;
//				memcpy(&x, &buffer[4], sizeof(x));
//				memcpy(&y, &buffer[8], sizeof(y));
//				memcpy(&inputID, &buffer[12], sizeof(inputID));
//				game->CreatePlayer(id, 'P', x, y, inputID);
//
//				delete[] buffer;
//			}
//
//			game->SetCurrentMessage("Updated the game state.");
//
//			break;
//		}
//		case ID_UPDATE_ENTITY:
//		{
//			// Read in the packet data and find out how many objects there are
//			RakNet::BitStream bsIn(_packet->data, _packet->length, false);
//			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
//
//			// Read in the serialized data into a buffer for this game object
//			unsigned int size;
//			bsIn.Read(size);
//			char * buffer = new char[size];
//			memset(buffer, 0, size);
//			bsIn.Read(buffer, size);
//
//			// Check if the object exists first and if it does, update it
//			unsigned int id;
//			memcpy(&id, &buffer[0], sizeof(id));
//			GameObject * existingObj = game->GetGameObjectByID(id);
//			if (existingObj)
//			{
//				 existingObj->DeSerialize(buffer);
//				 delete[] buffer;
//				 break;
//			}
//
//			// If it does not exist then make a new one TODO make this at least kind of dynamic
//			int x, y;
//			unsigned int inputID;
//			memcpy(&x, &buffer[4], sizeof(x));
//			memcpy(&y, &buffer[8], sizeof(y));
//			memcpy(&inputID, &buffer[12], sizeof(inputID));
//			game->CreatePlayer(id, 'P', x, y, inputID);
//
//			game->SetCurrentMessage("Updated a game object.");
//			delete[] buffer;
//
//			break;
//		}
//		default:
//		{
//			std::stringstream temp;
//			temp << "Message with identifier ";
//			temp << _packet->data[0];
//			temp << "has arrived.\n"; // figure this shit out
//			game->SetCurrentMessage(temp.str());
//			break;
//		}
//	}
//}
//
///////////////////////////////
////	SendToAddress
////	 -Send the given message to the given address
//void NetworkManager::SendToAddress(RakNet::BitStream * _bsOut, RakNet::SystemAddress _address)
//{
//	m_pPeer->Send(_bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, _address, false);
//}
//
///////////////////////////////
////	SendToAll
////	 -Send the given message to everyone on the server
//void NetworkManager::SendToAll(RakNet::BitStream * _bsOut)
//{
//	for (auto iter = m_mAddresses.begin(); iter != m_mAddresses.end(); ++iter)
//	{
//		SendToAddress(_bsOut, (*iter).second);
//	}
//}