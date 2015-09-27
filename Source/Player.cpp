//***********************************************************************
//	File:		Player.cpp
//	Author:		Michael Sciortino
//	Purpose:	Controlled by the current player, can move around
//***********************************************************************

#include "Player.h"
#include "Sprite.h"
#include "Transform.h"
#include "NetworkView.h"
#include "Game.h"
#include "InputController.h"
#include "../RakNetDungeon/MessageTypes.h"
#include "../Raknet/BitStream.h"
#include "../RakNetDungeon/GameplayState.h"
#include "../RakNetDungeon/ServerManager.h"

/////////////////////////////
//	Initialize
//	 -Startup
void Player::Initialize()
{
	Transform * transform = new Transform;
	transform->SetX(0);
	transform->SetY(0);
	transform->SetOwner(this);
	AddComponent((Component*)transform);

	Sprite * sprite = new Sprite;
	sprite->SetVisible(true);
	sprite->SetCharacter('P');
	sprite->SetOwner(this);
	AddComponent((Component*)sprite);

	InputController * controller = new InputController;
	controller->SetOwner(this);
	AddComponent((Component*)controller);

	NetworkView * nwView = new NetworkView;
	nwView->SetOwner(this);
	nwView->RegisterForMessage(ID_NWV_UPDATE_POSITION);
	AddComponent((Component*)nwView);

	m_uiID = 0;

	InitializeComponents();

}

/////////////////////////////
//	Update
//	 -Main loop
void Player::Update()
{
	Transform * playerT = (Transform*)GetComponent(C_TRANSFORM);

	InputController * input = (InputController*)GetComponent(C_INPUT);
	bool changedInput = false;
	if (input->GetKeyDown('W'))
	{
		playerT->SetY(playerT->GetY() - 1);
		changedInput = true;
	}
	if (input->GetKeyDown('A'))
	{
		playerT->SetX(playerT->GetX() - 1);
		changedInput = true;
	}
	if (input->GetKeyDown('S'))
	{
		playerT->SetY(playerT->GetY() + 1);
		changedInput = true;
	}
	if (input->GetKeyDown('D'))
	{
		playerT->SetX(playerT->GetX() + 1);
		changedInput = true;
	}

	if (changedInput)
	{
		NetworkView * nwView = (NetworkView*)GetComponent(C_NETWORK);
		nwView->UpdatePositionMessage(playerT->GetX(), playerT->GetY());
	}

	UpdateComponents();
}

/////////////////////////////
//	Terminate
//	 -Cleanup & shutdown
void Player::Terminate()
{
	TerminateComponents();
}

/////////////////////////////
//	Serialize
//	 -Turn all relevant data about this object into a char buffer
void Player::Serialize(char * _buffer)
{
	Transform * playerT = (Transform*)GetComponent(C_TRANSFORM);
	InputController * playerI = (InputController*)GetComponent(C_INPUT);

	// Write ID
	unsigned int id = m_uiID;
	memcpy(&_buffer[0], &id, sizeof(id));

	// Write X
	int x = playerT->GetX();
	memcpy(&_buffer[4], &x, sizeof(x));

	// Write Y
	int y = playerT->GetY();
	memcpy(&_buffer[8], &y, sizeof(y));

	// Write input controller's ID
	unsigned int inputID = playerI->GetID();
	memcpy(&_buffer[12], &inputID, sizeof(inputID));
}

/////////////////////////////
//	DeSerialize
//	 -Translate the data in the buffer into relevant data for this object
void Player::DeSerialize(char * _buffer)
{
	Transform * playerT = (Transform*)GetComponent(C_TRANSFORM);
	InputController * playerI = (InputController*)GetComponent(C_INPUT);

	// Read ID
	unsigned int id = 0;
	memcpy(&id, &_buffer[0], sizeof(id));
	m_uiID = id;

	// Read X
	int x = 0;
	memcpy(&x, &_buffer[4], sizeof(x));
	playerT->SetX(x);

	// Read Y
	int y = 0;
	memcpy(&y, &_buffer[8], sizeof(y));
	playerT->SetY(y);

	// Read input controller's ID
	unsigned int inputID = 0;// playerI->GetID();
	memcpy(&inputID, &_buffer[12], sizeof(inputID));
	playerI->SetID(inputID);

	m_pGame->SetRefresh(true);
}