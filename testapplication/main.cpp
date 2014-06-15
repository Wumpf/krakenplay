#include "../krakenplay/networkserver.h"
#include "../krakenplay/inputmanager.h"
#include <iostream>

int main()
{
	std::cout << "Init server ... ";
	if(Krakenplay::NetworkServer::Instance().InitServer())
		std::cout << "done\n";
	else
	{
		std::cout << "failed\n";
		return 1;
	}


	Krakenplay::InputManager::MouseInfo lastMouseState;
	for(;;)
	{
		Krakenplay::InputManager::Instance().Update();

		const Krakenplay::InputManager::MouseInfo* mouseInfo = Krakenplay::InputManager::Instance().GetMouseState(0);
		if (mouseInfo)
		{
			if (mouseInfo->IsButtonDown(Krakenplay::StateObjects::MouseButton::Left) != lastMouseState.IsButtonDown(Krakenplay::StateObjects::MouseButton::Left))
				std::cout << "Left click state changed\n";
			if (mouseInfo->IsButtonDown(Krakenplay::StateObjects::MouseButton::Middle) != lastMouseState.IsButtonDown(Krakenplay::StateObjects::MouseButton::Middle))
				std::cout << "Middle click state changed\n";

			lastMouseState = *mouseInfo;
		}

		Sleep(10);
	}


	return 0;
}	