/**
* @file
* Zombie.h
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
#pragma once
#include "Entity.h"
#include <SFML/Graphics/Sprite.hpp>
#include "TextureManager.h"
#include "Command.h"
#include "Animation2.h"

namespace GEX
{
	class CommandQueue;
	class TextNode;

	enum class ZombieType { Zombie1 };

	class Zombie : public Entity
	{
	public:
		enum class State { Idle, Jump, Attack, Walk, Run, Dead, Rise, count };

		enum class Direction { Left, Right, Up, Down };

		Zombie(ZombieType type, const GEX::TextureManager& textures);
		~Zombie() = default;

		unsigned int							getCategory() const override;
		sf::FloatRect							getBoundingBox() const override;
		float									getMaxSpeed() const;
		void									accelerate(sf::Vector2f velocity) override;

		void									attack();
		void									setState(State state);
		int										attackPoints() const;
		bool									Follows() const;
		void									guidedTowards(sf::Vector2f position);


	private:
		void									updateStates();
		void									updateCurrent(sf::Time dt, CommandQueue& commands) override;
		void									drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
		void									updateMovementPattern(sf::Time dt);
		void									updateTexts();
		bool									isMarkedForRemoval() const override;

	private:
		ZombieType								type_;
		State									state_;
		mutable sf::Sprite						sprite_;
		mutable std::map<State, Animation2>		animations_;
		Direction								direction_;
		TextNode*								healthDisplay_;
		const TextureManager&					textures_;

		float									travelDistance_;
		std::size_t								directionIndex_;

		bool									attack_;
		sf::Vector2f							targetDirection_;
		bool									forceField_;
		bool									godMode_;
		bool									godModeFlicker_;
		sf::Time								godModeTime_;
		sf::Time								godModeElapsedTime_;

	};
}

