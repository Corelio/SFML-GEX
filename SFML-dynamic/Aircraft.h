/**
* @file
* Aircraft.h
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
#include "Projectile.h"
#include "Animation.h"

namespace GEX
{
	class CommandQueue;
	class TextNode;

	//Types of aircraft
	enum class AircraftType { Eagle, Raptor, Avenger};

	class Aircraft : public Entity
	{
	public:
		explicit		Aircraft(AircraftType type, TextureManager& textures);

		virtual void	drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

		unsigned int	getCategory() const override;

		void			updateTexts(); //update the mini health and missile display

		void			fire();
		void			launchMissile() { isLaunchingMissile_ = true; };

		bool			isAllied() const;

		void			increaseFireRate();
		void			increaseFireSpread();
		void			collectMissiles(unsigned int count);
	
		sf::FloatRect	getBoundingBox() const override;

		bool			isMarkedForRemoval() const override;

		void			remove();

		void			updateLateralRoll(); //check and update the sprite if the airplane is moving on x axis

		void			playLocalSound(CommandQueue& commands, SoundEffectID effect);

	protected:
		void			updateCurrent(sf::Time dt, CommandQueue& commands) override;

	private:
		void			updateMovementPattern(sf::Time dt);
		float			getMaxSpeed() const;

		void			createBullets(SceneNode& node, TextureManager& texture);
		void			createProjectile(
										SceneNode& node, 
										Projectile::Type type, 
										float xOffset, 
										float yOffset, 
										const TextureManager& textures);
		
		void			checkProjectilelaunch(sf::Time dt, CommandQueue& commands);

		void			checkPickupDrop(CommandQueue& command);
		void			createPickup(SceneNode& node, TextureManager& texture);

	private:
		AircraftType	type_;
		sf::Sprite		sprite_;
		Animation		explosion_;
		bool			showExplosion_;

		TextNode*		healthDisplay_;
		TextNode*		missileDisplay_;

		float			travelDistance_;
		std::size_t		directionIndex_;

		bool			isFiring_;
		int				fireRateLevel_;
		int				fireSpreadLevel_;
		sf::Time		fireCountdown_;
		bool			isLaunchingMissile_;
		bool			isMarkedForRemoval_;
		int				missileAmmo_;

		Command			fireCommand_;
		Command			launchMissileCommand_;
		Command			dropPickupCommand_;

		bool			spawnPickup_;

		bool			hasPlayedExplosionSound_;

	};
}
