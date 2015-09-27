//***********************************************************************
//	File:		NetworkView.cpp
//	Author:		Michael Sciortino
//	Purpose:	For gameobjects to communicate over the network
//***********************************************************************

#include "NetworkView.h"
#include "../RakNetDungeon/ClientManager.h"
#include "../RakNetDungeon/ServerManager.h"
#include "Transform.h"
#include "GameObject.h"
#include "../RakNetDungeon/GameplayState.h"

//////////////////////
//	Initialize
//	 -Startup
void NetworkView::Initialize()
{
}

//////////////////////
//	Update
//	-Main loop
void NetworkView::Update()
{
}

//////////////////////
//	Terminate
//	 -Cleanup and shutdown
void NetworkView::Terminate()
{
	delete this;
}

//////////////////////
//	ParseMessage
//	 -Processes the message
void NetworkView::ParseMessage(RakNet::Packet * _packet)
{
	NWViewMessages messageType = (NWViewMessages)_packet->data[5];
	if (RegisteredForMessage(messageType))
	{
		switch (messageType)
		{
			case ID_NWV_UPDATE_POSITION:
			{
				GameObject * gameObj = GetOwner();
				RakNet::BitStream bsIn(_packet->data, _packet->length, false);
				bsIn.IgnoreBytes((sizeof(RakNet::MessageID)) + sizeof(NWViewMessages) + sizeof(unsigned int));

				int x, y;
				bsIn.Read(x);
				bsIn.Read(y);

				Transform * transform = (Transform*)gameObj->GetComponent(C_TRANSFORM);
				transform->SetX(x);
				transform->SetY(y);

				gameObj->GetGame()->SetRefresh(true);

				break;
			}
		}
	}
}

//////////////////////
//	RegisteredForMessage
//	 -Returns whether or not this nwview is registered for the given message
bool NetworkView::RegisteredForMessage(NWViewMessages _message)
{
	for (unsigned int i = 0; i < m_vRegisteredMessages.size(); ++i)
	{
		if (_message == m_vRegisteredMessages[i])
		{
			return true;
		}
	}

	return false;
}


//////////////////////
//	UpdatePositionMessage
//	 -Used to keep the position synched up
void NetworkView::UpdatePositionMessage(int _x, int _y)
{
	RakNet::BitStream bsPosition;
	bsPosition.Write((RakNet::MessageID)ID_ENTITY_MESSAGE);
	unsigned int id = GetOwner()->GetID();
	bsPosition.Write(id);
	bsPosition.Write(ID_NWV_UPDATE_POSITION);
	bsPosition.Write(_x);
	bsPosition.Write(_y);

	if (m_pClientManager)
	{
		m_pClientManager->SendToServer(&bsPosition);
	}
	else if (m_pServerManager)
	{
		m_pServerManager->SendToAll(&bsPosition);
	}
}