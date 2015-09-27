//***********************************************************************
//	File:		GameObject.cpp
//	Author:		Michael Sciortino
//	Purpose:	Base class for all sub game objects
//***********************************************************************

#include "GameObject.h"
#include "Component.h"

//////////////////////
//	InitializeComponents
//	 -Loops through each component and calls initialize
void GameObject::InitializeComponents()
{
	for (unsigned int i = 0; i < m_vComponents.size(); ++i)
	{
		m_vComponents[i]->Initialize();
	}
}

//////////////////////
//	UpdateComponents
//	 -Loops through each component and calls update
void GameObject::UpdateComponents()
{
	for (unsigned int i = 0; i < m_vComponents.size(); ++i)
	{
		m_vComponents[i]->Update();
	}
}

//////////////////////
//	TerminateComponents
//	 -Loops through each component and calls terminate and then deletes it
void GameObject::TerminateComponents()
{
	for (unsigned int i = 0; i < m_vComponents.size(); ++i)
	{
		m_vComponents[i]->Terminate();
		//delete m_vComponents[i];
	}
}

//////////////////////
//	HasComponent
//	 -Returns if this gameobject has the given component
bool GameObject::HasComponent(unsigned int _comp)
{
	for (unsigned int i = 0; i < m_vComponents.size(); ++i)
	{
		if (m_vComponents[i]->GetType() == _comp)
		{
			return true;
		}
	}

	return false;
}

//////////////////////
//	GetComponent
//	 -Returns the component of the given type or nullptr if not found
Component* GameObject::GetComponent(unsigned int _comp)
{
	for (unsigned int i = 0; i < m_vComponents.size(); ++i)
	{
		if (m_vComponents[i]->GetType() == _comp)
		{
			return m_vComponents[i];
		}
	}

	return nullptr;
}