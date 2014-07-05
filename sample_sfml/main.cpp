#include <algorithm>

#include "../krakenplay/networkserver.h"
#include "../krakenplay/inputmanager.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "player.h"

using namespace Krakenplay;

unsigned int windowWidth = 800;
unsigned int windowHeight = 600;

class TestApplication
{
public:
	TestApplication() : window(sf::VideoMode(windowWidth, windowHeight), "Krakenplay Demo")
	{
		NetworkServer::Instance().InitServer();
	}
	~TestApplication()
	{
		for(Player* player : playerList)
			delete player;
	}

	void Run()
	{
		while(window.isOpen())
		{
			Update();
			Draw();
		}
	}

private:
	void Draw()
	{
		window.clear(sf::Color::Black);
		for(Player* player : playerList)
			player->Draw(window);
		window.display();
	}

	void Update()
	{
		// Update input & window.
		sf::Event evt;
		while(window.pollEvent(evt))
		{
			if(evt.type == sf::Event::Closed)
				window.close();
		}
		InputManager::Instance().Update();

		// Add mouse players.
		for(unsigned int mouseIdx=0; mouseIdx < InputManager::Instance().GetNumMouses(); ++mouseIdx)
		{
			if(InputManager::Instance().GetState<InputManager::MouseState>(mouseIdx)->WasConnected())
				playerList.push_back(new Mouse(mouseIdx));
		}
		// Add keyboard players.
		for(unsigned int keyboardIdx = 0; keyboardIdx < InputManager::Instance().GetNumKeyboards(); ++keyboardIdx)
		{
			if(InputManager::Instance().GetState<InputManager::KeyboardState>(keyboardIdx)->WasConnected())
				playerList.push_back(new Keyboard(keyboardIdx));
		}
		// Add gamepad players.
		for(unsigned int gamepadIdx = 0; gamepadIdx < InputManager::Instance().GetNumGamepads(); ++gamepadIdx)
		{
			if(InputManager::Instance().GetState<InputManager::GamepadState>(gamepadIdx)->WasConnected())
				playerList.push_back(new Gamepad(gamepadIdx));
		}

		// Update players.
		for(auto it = playerList.begin(); it != playerList.end(); ++it)
		{
			if(!(*it)->IsAlive())
			{
				delete *it;
				it = playerList.erase(it);
				if(playerList.size() == 0)
					break;
			}
			else
			{
				(*it)->Update();
			}
		}
	}

	sf::RenderWindow window;
	std::vector<Player*> playerList;
};


int main()
{
	srand(1234);
	TestApplication application;
	application.Run();
}