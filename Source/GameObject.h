//***********************************************************************
//	File:		GameObject.h
//	Author:		Michael Sciortino
//	Purpose:	Base class for all sub game objects
//***********************************************************************

#pragma once

#include <vector>

class Component;
class GameplayState;

class GameObject
{
protected:

	// Each object will have a unique ID
	unsigned int m_uiID = 0;

	// Gameobjects will have a list of components
	std::vector<Component*> m_vComponents;

	// Reference to the local game
	GameplayState * m_pGame;

	void AddComponent(Component* _comp) { m_vComponents.push_back(_comp); }

	void InitializeComponents();
	void UpdateComponents();
	void TerminateComponents();

public:

	// Start, loop, end
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Terminate() = 0;

	virtual void Serialize(char * _buffer) = 0;
	virtual void DeSerialize(char * _buffer) = 0;
	virtual unsigned int GetSerialSize() = 0;

	void SetID(unsigned int _id) { m_uiID = _id; }
	unsigned int GetID() const { return m_uiID; }

	void SetGame(GameplayState * _game) { m_pGame = _game; }
	GameplayState * GetGame() const { return m_pGame; }

	bool HasComponent(unsigned int _comp);
	Component* GetComponent(unsigned int _comp);

};