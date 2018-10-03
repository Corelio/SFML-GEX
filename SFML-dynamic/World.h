/**
* @file
* World.h
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

//
// The Game World
//
#pragma once
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "SceneNode.h"
#include "SpriteNode.h"
#include "TextureManager.h"
#include "Aircraft.h"
#include <memory>
#include <iostream>
#include "CommandQueue.h"

namespace sf  //Forward declaration - This class does not need to know about this class
{
	class RenderWindow;
}

namespace GEX
{

	class World
	{
	public:
		explicit					World(sf::RenderWindow& window);

		void						update(sf::Time dt);
		void						draw();

		CommandQueue&				getCommandQueue();

	private:
		void						loadTextures();
		void						buildScene();
		void						adaptPlayerPosition();
		void						adaptPlayerVelocity();

	private:
		enum Layer
		{
			Background = 0,
			Air,
			LayerCount
		};

	private:
		const float					BORDER_DISTANCE = 40.f;

		sf::RenderWindow&			window_;
		sf::View					worldView_;
		TextureManager				textures_;

		SceneNode					sceneGraph_;
		std::vector<SceneNode*>		sceneLayers_;

		sf::FloatRect				worldBounds_;
		sf::Vector2f				spawnPosition_;
		float						scrollSpeed_;
		int							counter_;
		int							orientation_;
		Aircraft*					player_;
		Aircraft*					leftAircraft_;
		Aircraft*					rightAircraft_;
		SpriteNode*					background_;

		CommandQueue				commandQueue_;

		

	};

}