/**
* @file
* Aircraft.cpp
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
#include "Aircraft.h"
#include "Category.h"
#include "DataTables.h"
#include "TextNode.h"
#include <memory>
#include <string>
#include "Utility.h"
#include "CommandQueue.h"

namespace GEX
{
	//Make it private to this file
	namespace
	{
		const std::map<AircraftType, AircraftData> TABLE = initializeAircraftData();
	}

	//Aircraft Constructor - Get texture based on the type and set airplane position
	Aircraft::Aircraft(AircraftType type, TextureManager & textures)
		: Entity(TABLE.at(type).hitpoints)
		, type_(type)
		, sprite_(textures.get(TABLE.at(type).texture), TABLE.at(type).textureRect)
		, explosion_(textures.get(TextureID::Explosion))
		, showExplosion_(true)
		, healthDisplay_(nullptr)
		, missileDisplay_(nullptr)
		, travelDistance_(0.f)
		, directionIndex_(0)
		, isFiring_(false)
		, isLaunchingMissile_(false)
		, fireRateLevel_(1.f)
		, fireSpreadLevel_(1.f)
		, fireCountdown_(sf::Time::Zero)
		, spawnPickup_(false)
		, fireCommand_()
		, missileAmmo_(TABLE.at(type).missileAmount)
		, isMarkedForRemoval_(false)
	{

		//Set up the explosion
		explosion_.setFrameSize(sf::Vector2i(256, 256));
		explosion_.setNumFrames(16);
		explosion_.setDuration(sf::seconds(1));
		centerOrigin(explosion_);

		//Set up commands
		fireCommand_.category = Category::AirSceneLayer;
		fireCommand_.action = [this, &textures](SceneNode& node, sf::Time dt) 
		{
			createBullets(node, textures);
		};

		launchMissileCommand_.category = Category::AirSceneLayer;
		launchMissileCommand_.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			createProjectile(node, Projectile::Type::Missile, 0.f, 0.5f, textures);
		};

		dropPickupCommand_.category = Category::AirSceneLayer;
		dropPickupCommand_.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			createPickup(node, textures);
		};

		centerOrigin(sprite_);

		//Creating health display and attaching to the graph
		std::unique_ptr<TextNode> health(new TextNode(std::string("")));
		healthDisplay_ = health.get();
		attachChild(std::move(health));

		//Creating missile display and attaching to the graph
		std::unique_ptr<TextNode> missile(new TextNode(""));
		missileDisplay_ = missile.get();
		attachChild(std::move(missile));

	}

	//Draw the current 
	void Aircraft::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
	{
		if (isDestroyed() && showExplosion_)
		{
			target.draw(explosion_, states);
		}
		else
		{
			target.draw(sprite_, states);
		}
	}

	unsigned int Aircraft::getCategory() const
	{
		switch (type_)
		{
		case AircraftType::Eagle:
			return Category::PlayerAircraft;
			break;
		case AircraftType::Raptor:
		case AircraftType::Avenger:
			return Category::EnemyAircraft;
			break;
		default:
			Category::None;
		}
	}

	void Aircraft::updateTexts()
	{
		sf::Color color = sf::Color::White;
		if (getHitpoints() <= 20)
		{
			color = sf::Color::Red;
		}

		healthDisplay_->setText(std::to_string(getHitpoints()) + "HP", color);
		healthDisplay_->setPosition(0.f, 50.f);
		healthDisplay_->setRotation(-getRotation());
		if (isAllied())
		{
			sf::Color color = sf::Color::Green;
			if (missileAmmo_ <= 2) {
				color = sf::Color::Red;
			} 
			missileDisplay_->setText("Missile: " + std::to_string(missileAmmo_), color);
			missileDisplay_->setPosition(0.f, 70.f);
		}
	}

	void Aircraft::fire()
	{
		if (TABLE.at(type_).fireInterval != sf::Time::Zero)
		{
			isFiring_ = true;
		}
	}

	bool Aircraft::isAllied() const
	{
		return type_ == AircraftType::Eagle;
	}

	void Aircraft::increaseFireRate()
	{
		if (fireRateLevel_ < 10)
		{
			fireRateLevel_++;
		}
	}

	void Aircraft::increaseFireSpread()
	{
		if (fireSpreadLevel_ < 3)
		{
			fireSpreadLevel_++;
		}
	}

	void Aircraft::collectMissiles(unsigned int count)
	{
		missileAmmo_ += count;
	}

	sf::FloatRect Aircraft::getBoundingBox() const
	{
		return getWorldTransform().transformRect(sprite_.getGlobalBounds());
	}

	bool Aircraft::isMarkedForRemoval() const
	{
		return (isDestroyed() && (explosion_.isFinished() || !showExplosion_));
	}

	void Aircraft::remove()
	{
		Entity::remove();
		showExplosion_ = false;
	}

	void Aircraft::updateLateralRoll()
	{
		if (TABLE.at(type_).hasRollAnimation)
		{
			auto textureRect = TABLE.at(type_).textureRect;
			if (getVelocity().x < 0.f) // Turn left
			{
				textureRect.left = textureRect.width;
			}
			else if (getVelocity().x > 0.f) // Turn right
			{
				textureRect.left = 2 * textureRect.width;
			}

			sprite_.setTextureRect(textureRect);
		}
	}

	void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
	{
		checkProjectilelaunch(dt, commands);
		if (isDestroyed())
		{
			checkPickupDrop(commands);
			explosion_.update(dt);
			return;
		}
		Entity::updateCurrent(dt, commands);
		if(!isDestroyed())
		{
			updateMovementPattern(dt);
			updateTexts();
			updateLateralRoll();
		}
		
		
	}

	void Aircraft::updateMovementPattern(sf::Time dt)
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

	float Aircraft::getMaxSpeed() const
	{
		return TABLE.at(type_).speed;
	}

	void Aircraft::createBullets(SceneNode & node, TextureManager & texture)
	{
		Projectile::Type type = isAllied() ? Projectile::Type::AlliedBullet : Projectile::Type::EnemyBullet;
		switch (fireSpreadLevel_)
		{
		case 1:
			createProjectile(node, type, 0.f, 0.5f, texture);
			break;
		case 2:
			createProjectile(node, type, -0.33f, 0.5f, texture);
			createProjectile(node, type, 0.33f, 0.5f, texture);
			break;
		case 3:
			createProjectile(node, type, -0.5f, 0.5f, texture);
			createProjectile(node, type, 0.f, 0.5f, texture);
			createProjectile(node, type, 0.5f, 0.5f, texture);
			break;
		}
	}

	void Aircraft::createProjectile(SceneNode & node, Projectile::Type type, float xOffset, float yOffset, const TextureManager & textures)
	{
		//Create the projectile
		std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
		
		//Deal with the projectile position
		sf::Vector2f offset(xOffset * sprite_.getGlobalBounds().width, yOffset * sprite_.getGlobalBounds().height);
		sf::Vector2f velocity(0.f, projectile->getMaxSpeed());
		float sign = isAllied() ? -1.f : 1.f;

		projectile->setPosition(getWorldPosition() + offset * sign);
		projectile->setVelocity(velocity*sign);
		node.attachChild(std::move(projectile));
	}

	void Aircraft::checkProjectilelaunch(sf::Time dt, CommandQueue & commands)
	{
		//Bullets
		//Enemies are always firing
		//at least those that are able to fire
		if (!isAllied())
		{
			fire();
		}
		
		if (isFiring_ && fireCountdown_ <= sf::Time::Zero)
		{
			commands.push(fireCommand_);
			isFiring_ = false;
			fireCountdown_ = TABLE.at(type_).fireInterval / (fireRateLevel_ + 1.f);
		}
		else if(fireCountdown_ > sf::Time::Zero)
		{
			fireCountdown_ -= dt;
		}

		//Missile
		if (isLaunchingMissile_)
		{
			if (missileAmmo_ > 0)
			{
				commands.push(launchMissileCommand_);
				isLaunchingMissile_ = false;
				missileAmmo_--;
			}
			
		}

	}

	void Aircraft::checkPickupDrop(CommandQueue & command)
	{
		if (!isAllied() && randomInt(2) == 0 && !spawnPickup_)
		{
			command.push(dropPickupCommand_);
		}

		spawnPickup_ = true;
	}

	void Aircraft::createPickup(SceneNode & node, TextureManager & texture)
	{
		auto type = static_cast<Pickup::Type>(randomInt(static_cast<int>(Pickup::Type::Count)));

		std::unique_ptr<Pickup> pickup(new Pickup(type, texture));
		pickup->setPosition(getWorldPosition());
		pickup->setVelocity(0.f, 0.f);
		node.attachChild(std::move(pickup));
	}

}