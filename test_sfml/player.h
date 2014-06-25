#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class Player
{
public:
	Player();

	virtual void Update() = 0;
	virtual bool IsAlive() = 0;

	void Draw(sf::RenderWindow& window);

protected:
	sf::Vector2f position;
	sf::Color color;
	bool effect;
};

class Mouse : public Player
{
public:
	Mouse(unsigned int idx);

	void Update() override;
	bool IsAlive() override;

private:
	sf::Vector2i mouseStart;
	unsigned int idx;
};