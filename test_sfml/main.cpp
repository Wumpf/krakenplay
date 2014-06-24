#include <algorithm>

#include "../krakenplay/networkserver.h"
#include "../krakenplay/inputmanager.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#undef min
#undef max

using namespace Krakenplay;

const unsigned int windowWidth = 800;
const unsigned int windowHeight = 600;

template<typename T>
T clamp(T v, T min, T max)
{
	return std::min(std::max(min, v), max);
}

// http://en.sfml-dev.org/forums/index.php?topic=7313.0
// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f
sf::Color hsv(int hue, float sat, float val)
{
	hue %= 360;
	while(hue < 0) hue += 360;

	if(sat<0.f) sat = 0.f;
	if(sat>1.f) sat = 1.f;

	if(val<0.f) val = 0.f;
	if(val>1.f) val = 1.f;

	int h = hue / 60;
	float f = float(hue) / 60 - h;
	float p = val*(1.f - sat);
	float q = val*(1.f - sat*f);
	float t = val*(1.f - sat*(1 - f));

	switch(h)
	{
	default:
	case 0:
	case 6: return sf::Color(static_cast<sf::Uint8>(val * 255), static_cast<sf::Uint8>(t * 255), static_cast<sf::Uint8>(p * 255));
	case 1: return sf::Color(static_cast<sf::Uint8>(q * 255), static_cast<sf::Uint8>(val * 255), static_cast<sf::Uint8>(p * 255));
	case 2: return sf::Color(static_cast<sf::Uint8>(p * 255), static_cast<sf::Uint8>(val * 255), static_cast<sf::Uint8>(t * 255));
	case 3: return sf::Color(static_cast<sf::Uint8>(p * 255), static_cast<sf::Uint8>(q * 255), static_cast<sf::Uint8>(val * 255));
	case 4: return sf::Color(static_cast<sf::Uint8>(t * 255), static_cast<sf::Uint8>(p * 255), static_cast<sf::Uint8>(val * 255));
	case 5: return sf::Color(static_cast<sf::Uint8>(val * 255), static_cast<sf::Uint8>(p * 255), static_cast<sf::Uint8>(q * 255));
	}
}


class Player
{
public:
	Player() : position(200, 200), effect(false)
	{
		color = hsv(rand() % 360, 1.0f, 0.8f);
	}

	virtual void Update() = 0;
	virtual bool IsAlive() = 0;

	void Draw(sf::RenderWindow& renderWindow)
	{
		const float circleRadius = 10;
		sf::CircleShape circle(circleRadius, 16);
		circle.setOutlineColor(sf::Color::White);
		circle.setFillColor(color);
		circle.setPosition(position - sf::Vector2f(circleRadius + effect ? 1.0f : 0.0f, circleRadius + effect ? 1.0f : 0.0f));
		if(effect)
			circle.setOutlineThickness(2.0f);
		else
			circle.setOutlineThickness(0.0f);
		renderWindow.draw(circle);
	}

protected:
	sf::Vector2f position;
	sf::Color color;
	bool effect;
};

class Mouse : public Player
{
public:
	Mouse(unsigned int idx) : idx(idx)
	{
		mouseStart.x = InputManager::Instance().GetState<InputManager::MouseState>(idx)->state.positionX;
		mouseStart.y = InputManager::Instance().GetState<InputManager::MouseState>(idx)->state.positionY;
	}

	void Update() override
	{
		position.x = static_cast<float>(InputManager::Instance().GetState<InputManager::MouseState>(idx)->state.positionX - mouseStart.x);
		while(position.x < 0) position.x += windowWidth;
		while(position.x > windowWidth) position.x -= windowWidth;

		position.y = static_cast<float>(InputManager::Instance().GetState<InputManager::MouseState>(idx)->state.positionY - mouseStart.y);
		while(position.y < 0) position.y += windowHeight;
		while(position.y > windowWidth) position.y -= windowHeight;

		effect = InputManager::Instance().GetState<InputManager::MouseState>(idx)->IsButtonDown(MouseButton::LEFT);
	}

	bool IsAlive() override
	{
		return InputManager::Instance().GetState<InputManager::MouseState>(idx)->connected;
	}

private:
	sf::Vector2i mouseStart;
	unsigned int idx;
};


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