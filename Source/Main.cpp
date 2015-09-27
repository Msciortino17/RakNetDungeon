//***********************************************************************
//	File:		Main.cpp
//	Author:		Michael Sciortino
//	Purpose:	Entry point for program
//***********************************************************************

#include <iostream>
#include "Game.h"

//#include <vld.h>

int main()
{
	Game * pGame = Game::GetInstance();
	pGame->Initialize();

	while (pGame->GetRunning())
	{
		pGame->Update();
	}

	pGame->Terminate();
	Game::DeleteInstance();
	pGame = nullptr;

	std::cout << "\n\n";
	system("pause");
	return 0;
}