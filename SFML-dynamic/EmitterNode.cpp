/**
* @file
* EmitterNode.cpp
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
#include "EmitterNode.h"
#include "CommandQueue.h"
#include "Command.h"

namespace GEX
{

	EmitterNode::EmitterNode(Particle::Type type)
		: SceneNode()
		, type_(type)
		, accumulateTime_(sf::Time::Zero)
		, particleSystem_(nullptr)
	{
	}

	void EmitterNode::updateCurrent(sf::Time dt, CommandQueue & commands)
	{
		if (particleSystem_)
		{
			emitParticle(dt);
		}
		else
		{
			auto finder = [this](ParticleNode& container, sf::Time dt)
			{
				if (container.getParticleType() == type_)
				{
					particleSystem_ = &container;
				}
			};

			Command command;
			command.category = Category::ParticleSystem;
			command.action = derivedAction<ParticleNode>(finder);

			commands.push(command);
		}
	}

	void EmitterNode::emitParticle(sf::Time dt)
	{
		const float EMISSION_RATE = 30.f;
		const sf::Time interval = sf::seconds(1.f) / EMISSION_RATE;

		accumulateTime_ += dt;

		while (accumulateTime_ > interval)
		{
			accumulateTime_ -= interval;
			particleSystem_->addParticle(getWorldPosition());
		}

	}

}