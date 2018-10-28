/**
* @file
* World.cpp
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
#include "World.h"
#include <set>
#include "Pickup.h"
#include <memory>
#include "ParticleNode.h"

namespace GEX
{

	World::World(sf::RenderWindow& window)
		: window_(window)
		, worldView_(window.getDefaultView())
		, textures_()
		, sceneGraph_()
		, sceneLayers_()
		, worldBounds_(0.f, 0.f, worldView_.getSize().x, 5000.f)
		, spawnPosition_(worldView_.getSize().x/2.f, worldBounds_.height - (worldView_.getSize().y / 2.f))
		, scrollSpeed_(-150.f)
		, counter_(1)
		, orientation_(1)
		, commandQueue_()
	{
		loadTextures();

		//prepare the view

		worldView_.setCenter(spawnPosition_);

		buildScene();
	}

	void World::update(sf::Time dt, CommandQueue& commands)
	{
		// For fun!! Replacing the background when it world bounds ends
		/*if (worldView_.getCenter().y - (worldView_.getSize().y / 2 - 50) < 50)
		{
			worldView_.setCenter(spawnPosition_);
			player_->setPosition(player_->getPosition().x, player_->getPosition().y + worldBounds_.height - (worldView_.getSize().y));
			// background
			sf::Texture&		texture = textures_.get(TextureID::Space);
			sf::IntRect			textureRect(worldBounds_);
			texture.setRepeated(true);
			background_->setSprite(texture, textureRect);
		}*/
		// scroll the world
		worldView_.move(0.f, scrollSpeed_*dt.asSeconds());

		//Remove previous velocity
		player_->setVelocity(0.f, 0.f);

		//Remove entities that are out of view
		destroyEntitiesOutOfView();

		//Process guided missile
		guideMissiles();

		// run all commands in the command queue
		while (!commandQueue_.isEmpty())
		{
			sceneGraph_.onCommand(commandQueue_.pop(), dt);
		}

		//Handleling collisions
		handleCollision();

		//remove all wrecks from the scene graph
		sceneGraph_.removeWrecks();

		adaptPlayerVelocity();
		sceneGraph_.update(dt, commands);
		adaptPlayerPosition();

		//check if there are any enemy inside of the battlefield and spawn it
		spawnEmenies();

	}

	void World::adaptPlayerPosition()
	{
		sf::FloatRect viewBounds(worldView_.getCenter()-worldView_.getSize() / 2.f, worldView_.getSize());

		sf::Vector2f position = player_->getPosition();
		position.x = std::max(position.x, viewBounds.left + BORDER_DISTANCE);
		position.x = std::min(position.x, viewBounds.left + viewBounds.width - BORDER_DISTANCE);

		position.y = std::max(position.y, viewBounds.top + BORDER_DISTANCE);
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - BORDER_DISTANCE);

		player_->setPosition(position);
	}

	void World::adaptPlayerVelocity()
	{
		sf::Vector2f velocity = player_->getVelocity();
		if (velocity.x != 0.f && velocity.y != 0.f)
		{
			player_->setVelocity(velocity / std::sqrt(2.f));
		}

		player_->accelerate(0.f, scrollSpeed_);
	}

	void World::addEnemies()
	{
		addEnemy(AircraftType::Raptor, -250.f, 600.f);
		addEnemy(AircraftType::Raptor, 0.f, 600.f);
		addEnemy(AircraftType::Raptor, +250.f, 600.f);

		addEnemy(AircraftType::Raptor, -250.f, 900.f);
		addEnemy(AircraftType::Raptor, 0.f, 900.f);
		addEnemy(AircraftType::Raptor, +250.f, 900.f);

		addEnemy(AircraftType::Avenger, -70.f, 800.f);
		addEnemy(AircraftType::Avenger, 70.f, 800.f);

		addEnemy(AircraftType::Avenger, -70.f, 1200.f);
		addEnemy(AircraftType::Avenger, 70.f, 1200.f);

		addEnemy(AircraftType::Avenger, -170.f, 1800.f);
		addEnemy(AircraftType::Avenger, 170.f, 1800.f);
		
		//Sort the enemy vector by Y position
		std::sort(enemySpawnPoints_.begin(), enemySpawnPoints_.end(),
			[] (SpawnPoint lhs, SpawnPoint rhs)
			{
				return lhs.y < rhs.y;
			}
			);

	}

	void World::addEnemy(AircraftType type, float relX, float relY)
	{
		enemySpawnPoints_.push_back(SpawnPoint(type, spawnPosition_.x + relX, spawnPosition_.y - relY));
	}

	void World::spawnEmenies()
	{
		//Check if the vector is not empty and the last element is inside of the battlegrounds
		while (!enemySpawnPoints_.empty() && enemySpawnPoints_.back().y > getBattlefieldBounds().top) {

			auto spawnPoint = enemySpawnPoints_.back();
			std::unique_ptr<Aircraft> enemy(new Aircraft(spawnPoint.type, textures_));
			enemy->setPosition(spawnPoint.x, spawnPoint.y);
			enemy->setVelocity(0.f, -scrollSpeed_);
			enemy->rotate(180);
			sceneLayers_[UpperAir]->attachChild(std::move(enemy));
			enemySpawnPoints_.pop_back();

		}
	}

	sf::FloatRect World::getViewBounds() const
	{
		return sf::FloatRect(worldView_.getCenter() - worldView_.getSize() / 2.f, worldView_.getSize());
	}

	sf::FloatRect World::getBattlefieldBounds() const
	{
		sf::FloatRect bounds = getViewBounds();
		bounds.top -= 100.f;
		bounds.height += 100.f;
		return bounds;
	}

	void World::guideMissiles()
	{
		// Build a list of active Enemies
		Command enemyCollector;
		enemyCollector.category = Category::EnemyAircraft;
		enemyCollector.action = derivedAction<Aircraft>([this](Aircraft& enemy, sf::Time dt)
		{
			if (!enemy.isDestroyed())
			{
				activeEnemies_.push_back(&enemy);
			}
		});

		Command missileGuider;
		missileGuider.category = Category::AlliedProjectile;
		missileGuider.action = derivedAction<Projectile>([this](Projectile& missile, sf::Time dt)
		{
			if (!missile.isGuided())  //ignore bullets
			{
				return;
			}

			float minDistance = std::numeric_limits<float>::max();
			Aircraft* closestEnemy = nullptr;

			for (auto* e : activeEnemies_)
			{
				auto d = distance(missile, *e);
				if (d < minDistance)
				{
					minDistance = d;
					closestEnemy = e;
				}
			}

			if (closestEnemy)
			{
				missile.guidedTowards(closestEnemy->getWorldPosition());
			}

		});

		commandQueue_.push(enemyCollector);
		commandQueue_.push(missileGuider);
		activeEnemies_.clear();
	}

	bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
	{
		unsigned int category1 = colliders.first->getCategory();
		unsigned int category2 = colliders.second->getCategory();

		//Make sure first pair entry has category type1 and seconde has type2
		if (type1 & category1 && type2 & category2)
		{
			return true;
		}
		else if(type1 & category2 && type2 & category1)
		{
			std::swap(colliders.first, colliders.second);
			return true;
		}
		
		return false;
	
	}

	void World::handleCollision()
	{
		// Build a list of collinding Pairs of SceneNode
		std::set<SceneNode::Pair> collisionPairs;

		sceneGraph_.checkSceneCollision(sceneGraph_, collisionPairs);

		for (auto collindingPair : collisionPairs)
		{
			if (matchesCategories(collindingPair, Category::Type::PlayerAircraft, Category::Type::EnemyAircraft))
			{
				auto& player = static_cast<Aircraft&>(*collindingPair.first);
				auto& enemy	 = static_cast<Aircraft&>(*collindingPair.second);
				player.damage(enemy.getHitpoints());
				enemy.destroy();
			}
			else if (matchesCategories(collindingPair, Category::Type::PlayerAircraft, Category::Type::Pickup))
			{
				auto& player = static_cast<Aircraft&>(*collindingPair.first);
				auto& pickup = static_cast<Pickup&>(*collindingPair.second);

				pickup.apply(player);
				pickup.destroy();
			}
			else if (matchesCategories(collindingPair, Category::Type::PlayerAircraft, Category::Type::EnemyProjectile) || matchesCategories(collindingPair, Category::EnemyAircraft, Category::AlliedProjectile))
			{
  				auto& aircraft   = static_cast<Aircraft&>(*collindingPair.first);
				auto& projectile = static_cast<Projectile&>(*collindingPair.second);

				aircraft.damage(projectile.getDamage());
				projectile.destroy();

			}
		}
	}

	void World::destroyEntitiesOutOfView()
	{
		Command command;
		command.category = Category::Type::Projectile | Category::Type::EnemyAircraft | Category::Type::Pickup;
		command.action = derivedAction<Entity>([this](Entity& e, sf::Time dt) 
		{
			if (!getBattlefieldBounds().intersects(e.getBoundingBox()))
			{
				e.destroy();
			}
		});

		commandQueue_.push(command);
		
	}

	void World::draw()
	{
		window_.setView(worldView_);
		window_.draw(sceneGraph_);
	}

	CommandQueue & World::getCommandQueue()
	{
		return commandQueue_;
	}

	bool World::hasAlivePlayer() const
	{
		return !player_->isDestroyed();
	}

	bool World::hasPlayerReachedEnd() const
	{
		return !worldBounds_.contains(player_->getPosition());
	}

	void World::loadTextures()
	{
		textures_.load(GEX::TextureID::Entities, "Media/Textures/Entities.png");
		textures_.load(GEX::TextureID::Landscape, "Media/Textures/Desert.png");
		textures_.load(GEX::TextureID::Space, "Media/Textures/Space.png");
		textures_.load(GEX::TextureID::Jungle, "Media/Textures/JungleBig.png");
		textures_.load(GEX::TextureID::Particle, "Media/Textures/Particle.png");
		textures_.load(GEX::TextureID::Explosion, "Media/Textures/Explosion.png");
		textures_.load(GEX::TextureID::FinishLine, "Media/Textures/FinishLine.png");
	}

	void World::buildScene()
	{
		// Initialize layers
		for (int i = 0; i < LayerCount; ++i)
		{
			auto category = (i == UpperAir) ? Category::Type::AirSceneLayer : Category::Type::None;
			SceneNode::Ptr layer(new SceneNode(category));
			sceneLayers_.push_back(layer.get());
			sceneGraph_.attachChild(std::move(layer));
		}

		//Particle System
		std::unique_ptr<ParticleNode> smoke(new ParticleNode(Particle::Type::Smoke, textures_));
		sceneLayers_[LowerAir]->attachChild(std::move(smoke));

		std::unique_ptr<ParticleNode> fire(new ParticleNode(Particle::Type::Propellant, textures_));
		sceneLayers_[LowerAir]->attachChild(std::move(fire));

		// background
		sf::Texture&		texture = textures_.get(TextureID::Jungle);
		sf::IntRect			textureRect(worldBounds_);
		texture.setRepeated(true);

		std::unique_ptr<SpriteNode>	backgroundSprite(new SpriteNode(texture, textureRect));

		backgroundSprite->setPosition(worldBounds_.left, worldBounds_.top);
		background_ = backgroundSprite.get();
		sceneLayers_[Background]->attachChild(std::move(backgroundSprite));

		// add player aircraft & game objects
 
		std::unique_ptr<Aircraft> leader(new Aircraft(AircraftType::Eagle, textures_));

		leader->setPosition(spawnPosition_);
		leader->setVelocity(50.f, scrollSpeed_);
		player_ = leader.get();
		sceneLayers_[UpperAir]->attachChild(std::move(leader));

		addEnemies();

		
	}
}