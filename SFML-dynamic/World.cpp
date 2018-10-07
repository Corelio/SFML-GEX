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
		buildScene();

		//prepare the view

		worldView_.setCenter(spawnPosition_);
	}

	void World::update(sf::Time dt)
	{
		// For fun!! Replacing the background when it world bounds ends
		if (worldView_.getCenter().y - (worldView_.getSize().y / 2 - 50) < 50)
		{
			worldView_.setCenter(spawnPosition_);
			player_->setPosition(player_->getPosition().x, player_->getPosition().y + worldBounds_.height - (worldView_.getSize().y));
			// background
			sf::Texture&		texture = textures_.get(TextureID::Space);
			sf::IntRect			textureRect(worldBounds_);
			texture.setRepeated(true);
			background_->setSprite(texture, textureRect);
		}
		// scroll the world
		worldView_.move(0.f, scrollSpeed_*dt.asSeconds());

		//Remove previous velocity
		player_->setVelocity(0.f, 0.f);

		// run all commands in the command queue
		while (!commandQueue_.isEmpty())
		{
			sceneGraph_.onCommand(commandQueue_.pop(), dt);
		}
		
		adaptPlayerVelocity();
		sceneGraph_.update(dt);
		adaptPlayerPosition();
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

	void World::loadTextures()
	{
		textures_.load(GEX::TextureID::Eagle, "Media/Textures/Eagle.png");
		textures_.load(GEX::TextureID::Raptor, "Media/Textures/Raptor.png");
		textures_.load(GEX::TextureID::Airplanes, "Media/Textures/Eagles.png");
		textures_.load(GEX::TextureID::Landscape, "Media/Textures/Desert.png");
		textures_.load(GEX::TextureID::Space, "Media/Textures/Space.png");
		textures_.load(GEX::TextureID::Avenger, "Media/Textures/Avenger.png");
	}

	void World::buildScene()
	{
		// Initialize layers
		for (int i = 0; i < LayerCount; ++i)
		{
			SceneNode::Ptr layer(new SceneNode());
			sceneLayers_.push_back(layer.get());
			sceneGraph_.attachChild(std::move(layer));
		}

		// background
		sf::Texture&		texture = textures_.get(TextureID::Landscape);
		sf::IntRect			textureRect(worldBounds_);
		texture.setRepeated(true);

		std::unique_ptr<SpriteNode>	backgroundSprite(new SpriteNode(texture, textureRect));

		backgroundSprite->setPosition(worldBounds_.left, worldBounds_.top);
		background_ = backgroundSprite.get();
		sceneLayers_[Background]->attachChild(std::move(backgroundSprite));

		// add player aircraft & game objects
 
		std::unique_ptr<Aircraft> leader(new Aircraft(AircraftType::Eagle, textures_));

		leader->setPosition(spawnPosition_);
		leader->setVelocity(50.f, scrollSpeed_*.3);
		player_ = leader.get();
		sceneLayers_[Air]->attachChild(std::move(leader));

		// add escort Planes
		std::unique_ptr<Aircraft> leftEscort(new Aircraft(AircraftType::Raptor, textures_));
		leftEscort->setPosition(-80.f, 50.f);
		leftAircraft_ = leftEscort.get();
		player_->attachChild(std::move(leftEscort));

		std::unique_ptr<Aircraft> rightEscort(new Aircraft(AircraftType::Avenger, textures_));
		rightEscort->setPosition(80.f, 50.f);
		rightAircraft_ = rightEscort.get();
		player_->attachChild(std::move(rightEscort));

		//add enemy planes
		//1
		std::unique_ptr<Aircraft> enemy(new Aircraft(AircraftType::Avenger, textures_));
		enemy->setPosition(spawnPosition_.x -100.f, spawnPosition_.y - 600.f);
		enemy->setVelocity(0.f, -scrollSpeed_);
		enemy->rotate(180);
		sceneLayers_[Air]->attachChild(std::move(enemy));

		//2
		enemy = std::move(std::unique_ptr<Aircraft>(new Aircraft(AircraftType::Raptor, textures_)));
		enemy->setPosition(spawnPosition_.x + 100.f, spawnPosition_.y - 600.f);
		enemy->setVelocity(0.f, -scrollSpeed_);
		enemy->rotate(180);
		sceneLayers_[Air]->attachChild(std::move(enemy));

		//3
		enemy = std::move(std::unique_ptr<Aircraft>(new Aircraft(AircraftType::Avenger, textures_)));
		enemy->setPosition(spawnPosition_.x + 400.f, spawnPosition_.y - 600.f);
		enemy->setVelocity(0.f, -scrollSpeed_);
		enemy->rotate(180);
		sceneLayers_[Air]->attachChild(std::move(enemy));
	}
}