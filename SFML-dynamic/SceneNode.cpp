/**
* @file
* SceneNode.cpp
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
#include "SceneNode.h"
#include <cassert>
#include "Command.h"

namespace GEX
{
	SceneNode::SceneNode()
		: children_()
		, parent_(nullptr)
	{

	}

	SceneNode::~SceneNode()
	{
	}

	void SceneNode::attachChild(Ptr child)
	{
		child->parent_ = this;
		children_.push_back(std::move(child));
	}

	SceneNode::Ptr SceneNode::detachChild(const SceneNode & node)
	{
		auto found = std::find_if(children_.begin(), children_.end(), [&](Ptr& p) {
			return &node == p.get();
		});
		assert(found != children_.end());
		if (found != children_.end()) {
			Ptr result = std::move(*found);
			children_.erase(found);
			return result;
		}
		return nullptr;
	}

	void SceneNode::update(sf::Time dt)
	{
		updateCurrent(dt);
		updateChildren(dt);
	}

	void SceneNode::onCommand(const Command & command, sf::Time dt)
	{
		// Command current node, if category matches
		if (command.category & getCategory())
		{
			command.action(*this, dt);
		}

		// Command children
		for (Ptr& child : children_)
		{
			child->onCommand(command, dt);
		}
	}

	unsigned int SceneNode::getCategory() const
	{
		return Category::Type::None;
	}

	sf::Vector2f SceneNode::getWorldPosition() const
	{
		return getWorldTransform() * sf::Vector2f();
	}

	sf::Transform SceneNode::getWorldTransform() const
	{
		sf::Transform transform = sf::Transform::Identity; // Identity Matrix
		
		for (const SceneNode* node = this; node != nullptr; node = node->parent_)
		{
			transform *= node->getTransform();
		}

		return transform;
	}

	void SceneNode::updateCurrent(sf::Time dt)
	{
		// to be overriten
	}

	void SceneNode::updateChildren(sf::Time dt)
	{
		for (Ptr& child : children_)
		{
			child->update(dt);
		}
	}

	void SceneNode::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		states.transform *= getTransform();

		drawCurrent(target, states);
		drawChildren(target, states);
	}

	void SceneNode::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
	{
		// no default
		// must be overwrite
	}

	void SceneNode::drawChildren(sf::RenderTarget & target, sf::RenderStates states) const
	{	
		/*
		for (auto i = children_.begin(); i != children_.end(); ++i)
		{
			(*i)->draw(target, states);
		}
		*/
		/* or */
		for (const Ptr& child : children_)
		{
			child->draw(target, states);
		}
	}


}
