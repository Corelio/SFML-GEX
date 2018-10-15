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
		, sprite_(textures.get(TABLE.at(type).texture))
		, healthDisplay_(nullptr)
		, missileDisplay_(nullptr)
		, travelDistance_(0.f)
		, directionIndex_(0)
		, isFiring_(false)
		, isLaunchingMissile_(false)
		, fireRateLevel_(1.f)
		, fireSpreadLevel_(1.f)
		, fireCountdown_(sf::Time::Zero)
		, fireCommand_()
		, missileAmmo_(TABLE.at(type).missileAmount)
	{

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
		target.draw(sprite_, states);
	}

	unsigned int Aircraft::getCategory() const
	{
		switch (type_)
		{
		case AircraftType::Eagle:
			return Category::PlayerAircraft;
			break;
		case AircraftType::Raptor:
			return Category::EnemyAircraft;
			break;
		default:
			Category::None;
		}
	}

	void Aircraft::updateTexts()
	{
		healthDisplay_->setText(std::to_string(getHitpoints()) + "HP");
		healthDisplay_->setPosition(0.f, 50.f);
		healthDisplay_->setRotation(-getRotation());
		if (type_ == AircraftType::Eagle)
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

	void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
	{
		updateMovementPattern(dt);
		Entity::updateCurrent(dt, commands);
		updateTexts();
		checkProjectilelaunch(dt, commands);
	}

	void Aircraft::updateMovementPattern(sf::Time dt)
	{
		// Movement pattern
		const std::vector<Direction>& directions = TABLE.at(type_).directions;
		if (!directions.empty())
		{
			if (travelDistance_ > directions.at(directionIndex_).distance)
			{
				directionIndex_ = (++directionIndex_) % directions.size();
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
		//Bulletss
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

}