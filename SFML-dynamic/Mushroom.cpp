/**
* @file
* Mushroom.cpp
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
#include "Mushroom.h"
#include "Category.h"
#include "DataTables.h"
#include "TextNode.h"
#include <memory>
#include <string>
#include "Utility.h"
#include "CommandQueue.h"
#include <functional>

namespace GEX
{

	namespace
	{
		const std::map<MushroomType, MushroomData> TABLE = initializeMushroomData();
	}

	Mushroom::Mushroom(MushroomType type, TextureManager & textures)
		: Entity(TABLE.at(type).hitpoints)
		, type_(type)
		, sprite_(textures.get(TABLE.at(type).texture), TABLE.at(type).textureRect)
		, explosion_(textures.get(TextureID::Explosion))
		, moviment_(textures.get(TextureID::Mushroom))
		, showExplosion_(true)
		, travelDistance_(0.f)
		, directionIndex_(0)
		, isMarkedForRemoval_(false)
	{
		//Set up the explosion
		explosion_.setFrameSize(sf::Vector2i(256, 256));
		explosion_.setNumFrames(16);
		explosion_.setDuration(sf::seconds(1));
		centerOrigin(explosion_);

		centerOrigin(sprite_);

		//Set up the moviment animation
		moviment_.setFrameSize(sf::Vector2i(64, 64));
		moviment_.setNumFrames(18);
		moviment_.setDuration(sf::seconds(1));
		moviment_.setRepeating(true);
		centerOrigin(moviment_);

	}

	void Mushroom::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
	{
		if (isDestroyed() && showExplosion_)
		{
			target.draw(explosion_, states);
		}
		else
		{
			target.draw(moviment_, states);
		}
	}

	unsigned int Mushroom::getCategory() const
	{
		return Category::Mushroom;
	}

	sf::FloatRect Mushroom::getBoundingBox() const
	{
		return getWorldTransform().transformRect(sprite_.getGlobalBounds());
	}

	bool Mushroom::isMarkedForRemoval() const
	{
		return (isDestroyed() && (explosion_.isFinished() || !showExplosion_));
	}

	void Mushroom::remove()
	{
		Entity::remove();
		showExplosion_ = false;
	}

	void Mushroom::updateCurrent(sf::Time dt, CommandQueue & commands)
	{
		if (isDestroyed())
		{
			explosion_.update(dt);
			return;
		}
		Entity::updateCurrent(dt, commands);

		updateMovementPattern(dt);
		moviment_.update(dt);

	}

	void Mushroom::updateMovementPattern(sf::Time dt)
	{
		// Movement pattern
		const std::vector<Direction>& directions = TABLE.at(type_).directions;
		if (!directions.empty())
		{
			if (travelDistance_ > directions.at(directionIndex_).distance)
			{
				directionIndex_ = (directionIndex_ + 1) % directions.size();
				travelDistance_ = 0;
			}

			float radians = toRadian(directions.at(directionIndex_).angle + 90.f);
			float vx = getMaxSpeed() * std::cos(radians);
			float vy = getMaxSpeed() * std::sin(radians);

			setVelocity(vx, vy);
			travelDistance_ += getMaxSpeed() * dt.asSeconds();
		}
	}

	float Mushroom::getMaxSpeed() const
	{
		return TABLE.at(type_).speed;
	}
}
