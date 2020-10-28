#include <iostream>

#include <assert.h>

#include "SFML/Graphics.hpp"
using namespace std;

int main()
{
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(1280, 720), "My first code");

	// Load a sprite to display
	sf::Texture texture;
	if (!texture.loadFromFile("data/2dsprite.png"))
		assert(false);


	// Start the game loop 
	while (window.isOpen())
	{
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode == 27)
					window.close();
			}
		}

		// Clear screen
		window.clear();

		//instantiate and draw a sprite
		sf::Sprite spr(texture);
		//spr.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
		//spr.setPosition(GC::SCREEN_RES.x / 2.f, GC::SCREEN_RES.y / 2.f);
		window.draw(spr);

		// Update the window
		window.display();
	}

	return EXIT_SUCCESS;
}