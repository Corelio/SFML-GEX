/**
* @file
* PlayerControl.cpp
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
#include "PlayerControl.h"
#include "Aircraft.h"
#include "Command.h"
#include <functional>

namespace GEX
{
	struct AircraftMover
	{
		AircraftMover(float vx, float vy) : velocity(vx, vy) {}

		void	operator() (Aircraft& aircraft, sf::Time dt) const
		{
			aircraft.accelerate(velocity);
		}

		sf::Vector2f	velocity;
	};

	struct AircraftRotator
	{
		AircraftRotator(int direction) : direction(direction) {}

		void	operator() (Aircraft& aircraft, sf::Time dt) const
		{
			aircraft.rotate(direction);
		}

		int	direction;
	};

	PlayerControl::PlayerControl()
		:currentMissionStatus_(MissionStatus::MissionRunning)
	{
		// Set up bindings
		keyBindings_[sf::Keyboard::Left] = Action::MoveLeft;
		keyBindings_[sf::Keyboard::Right] = Action::MoveRight;
		keyBindings_[sf::Keyboard::Up] = Action::MoveUp;
		keyBindings_[sf::Keyboard::Down] = Action::MoveDown;
		keyBindings_[sf::Keyboard::R] = Action::EnemyRotateRight;
		keyBindings_[sf::Keyboard::L] = Action::EnemyRotateLeft;

		keyBindings_[sf::Keyboard::Space] = Action::Fire;
		keyBindings_[sf::Keyboard::M] = Action::LaunchMissile;

		//Set up actionbindings 
		initializeActions();

	}

	void PlayerControl::handleEvent(const sf::Event & event, CommandQueue & commands)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			auto found = keyBindings_.find(event.key.code);
			if (found != keyBindings_.end() && !isRealTimeAction(found->second))
			{
				commands.push(actionBindings_.at(found->second));
			}
		}
	}

	void PlayerControl::handleRealTimeInput(CommandQueue & commands)
		{
		//travese all assigned keys, look up the action, generate command
		for (auto pair : keyBindings_)
		{
			if (sf::Keyboard::isKeyPressed(pair.first) && isRealTimeAction(pair.second))
			{
				commands.push(actionBindings_.at(pair.second));
			}
		}
	}

	void PlayerControl::setMissionStatus(MissionStatus status)
	{
		currentMissionStatus_ = status;
	}

	MissionStatus PlayerControl::getMissionStatus() const
	{
		return currentMissionStatus_;
	}

	bool PlayerControl::isRealTimeAction(Action action)
	{
		switch (action)
		{
		case Action::MoveLeft:
		case Action::MoveRight:
		case Action::MoveUp:
		case Action::MoveDown:
		case Action::EnemyRotateLeft:
		case Action::EnemyRotateRight:
		case Action::Fire:
			return true;
			break;
		default:
			return false;
		}
	}

	void PlayerControl::initializeActions()
	{
		const float playerSpeed = 200.f;

		actionBindings_[Action::MoveLeft].action = derivedAction<Aircraft>(AircraftMover(-playerSpeed, 0.f));
		actionBindings_[Action::MoveRight].action = derivedAction<Aircraft>(AircraftMover(playerSpeed, 0.f));
		actionBindings_[Action::MoveUp].action = derivedAction<Aircraft>(AircraftMover(0.f, -playerSpeed));
		actionBindings_[Action::MoveDown].action = derivedAction<Aircraft>(AircraftMover(0.f, playerSpeed));
		actionBindings_[Action::Fire].action = derivedAction<Aircraft>(std::bind(&Aircraft::fire, std::placeholders::_1));
		actionBindings_[Action::LaunchMissile].action = derivedAction<Aircraft>(std::bind(&Aircraft::launchMissile, std::placeholders::_1));

		for (auto& pair : actionBindings_)
		{
			pair.second.category = Category::PlayerAircraft;
		}
		actionBindings_[Action::EnemyRotateLeft].action = derivedAction<Aircraft>(AircraftRotator(-1.f));
		actionBindings_[Action::EnemyRotateLeft].category = Category::EnemyAircraft;
		actionBindings_[Action::EnemyRotateRight].action = derivedAction<Aircraft>(AircraftRotator(1.f));
		actionBindings_[Action::EnemyRotateRight].category = Category::EnemyAircraft;
		
		
	}
}