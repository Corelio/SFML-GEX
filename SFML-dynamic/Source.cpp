/**
* @file
* Source.cpp
* @author
* Marco Corsini Baccaro 2018
* @version 1.0
*
* @section DESCRIPTION
* Assignment #1 - The GexState (Oct, 8th)
*
* @section LICENSE
*
* Copyright 2018
* Permission to use, copy, modify, and/or distribute this software for
* any purpose with or without fee is hereby granted, provided that the
* above copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
* @section Academic Integrity
* I certify that this work is solely my own and complies with
* NBCC Academic Integrity Policy (policy 1111)
*/
#include <SFML/Graphics.hpp>
#include <vector>
#include "Aplication.h"

int main()
{
	Aplication game;
	game.run();

	/*
	sf::RenderWindow window(sf::VideoMode(600, 400), "SFML works!");
	sf::CircleShape shape(10.f);
	shape.setFillColor(sf::Color::Yellow);
	
	sf::CircleShape eye(10.f);
	eye.setFillColor(sf::Color::Black);
	eye.setPosition(110, 35);

	std::vector<sf::VertexArray> trianglesStates;

	sf::VertexArray triangle(sf::Triangles, 3);
	triangle[0].position = sf::Vector2f(100, 100);
	triangle[0].color = sf::Color::Black;
	triangle[1].position = sf::Vector2f(250, 60);
	triangle[1].color = sf::Color::Black;
	triangle[2].position = sf::Vector2f(250, 140);
	triangle[2].color = sf::Color::Black;

	trianglesStates.push_back(triangle);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			int speed = 5;
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Key::Right)
				{
					shape.move(sf::Vector2f(speed, 0));
				}
				else if (event.key.code == sf::Keyboard::Key::Left)
				{
					shape.move(sf::Vector2f(-speed, 0));
				}
				else if (event.key.code == sf::Keyboard::Key::Up)
				{
					shape.move(sf::Vector2f(0, -speed));
				}
				else if (event.key.code == sf::Keyboard::Key::Down)
				{
					shape.move(sf::Vector2f(0, speed));
				}
			}
			
		}

		auto p = shape.getPosition();
		p.x = p.x > 600 ? 0 : p.x;
		p.x = p.x < 0 ? 600 : p.x;
		p.y = p.y > 400 ? 0 : p.y;
		p.y = p.y < 0 ? 400 : p.y;

		shape.setPosition(p);

		window.clear();
		window.draw(shape);
		window.draw(eye);
		window.draw(triangle);
		window.display();
		
	}
	*/

	return 0;
}