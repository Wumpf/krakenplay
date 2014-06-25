#include "player.h"
#include "../krakenplay/inputmanager.h"

#undef min
#undef max

using namespace Krakenplay;

extern unsigned int windowWidth;
extern unsigned int windowHeight;

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
	case 6:
		return sf::Color(static_cast<sf::Uint8>(val * 255), static_cast<sf::Uint8>(t * 255), static_cast<sf::Uint8>(p * 255));
	case 1:
		return sf::Color(static_cast<sf::Uint8>(q * 255), static_cast<sf::Uint8>(val * 255), static_cast<sf::Uint8>(p * 255));
	case 2:
		return sf::Color(static_cast<sf::Uint8>(p * 255), static_cast<sf::Uint8>(val * 255), static_cast<sf::Uint8>(t * 255));
	case 3:
		return sf::Color(static_cast<sf::Uint8>(p * 255), static_cast<sf::Uint8>(q * 255), static_cast<sf::Uint8>(val * 255));
	case 4:
		return sf::Color(static_cast<sf::Uint8>(t * 255), static_cast<sf::Uint8>(p * 255), static_cast<sf::Uint8>(val * 255));
	case 5:
		return sf::Color(static_cast<sf::Uint8>(val * 255), static_cast<sf::Uint8>(p * 255), static_cast<sf::Uint8>(q * 255));
	}
}


Player::Player() : 
	position(200, 200), effect(false)
{
	color = hsv(rand() % 360, 1.0f, 0.8f);
}

void Player::Draw(sf::RenderWindow& renderWindow)
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

Mouse::Mouse(unsigned int idx) : idx(idx)
{
	mouseStart.x = InputManager::Instance().GetState<InputManager::MouseState>(idx)->state.positionX;
	mouseStart.y = InputManager::Instance().GetState<InputManager::MouseState>(idx)->state.positionY;
}

void Mouse::Update()
{
	position.x = static_cast<float>(InputManager::Instance().GetState<InputManager::MouseState>(idx)->state.positionX - mouseStart.x);
	while(position.x < 0) position.x += windowWidth;
	while(position.x > windowWidth) position.x -= windowWidth;

	position.y = static_cast<float>(InputManager::Instance().GetState<InputManager::MouseState>(idx)->state.positionY - mouseStart.y);
	while(position.y < 0) position.y += windowHeight;
	while(position.y > windowWidth) position.y -= windowHeight;

	effect = InputManager::Instance().GetState<InputManager::MouseState>(idx)->IsButtonDown(MouseButton::LEFT);
}

bool Mouse::IsAlive()
{
	return InputManager::Instance().GetState<InputManager::MouseState>(idx)->connected;
}