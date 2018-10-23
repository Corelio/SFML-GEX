/**
* @file
* Aplication.cpp
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
#include "Aplication.h"
#include "State.h"
#include "GameState.h"
#include "TitleState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "GexState.h"
#include "GameOverState.h"
#include "FontManager.h"

const sf::Time Aplication::TimePerFrame = sf::seconds(1.0f / 60.0f);

Aplication::Aplication()
	: window_(sf::VideoMode(1280, 960), "Killer Plane")
	, player_()
	, textures_()
	, stateStack_(GEX::State::Context(window_, textures_, player_))
	, statisticsText_()
	, statisticsUpdateTime_()
	, statisticsNumFrames_(0)
{
	window_.setKeyRepeatEnabled(false);

	GEX::FontManager::getInstance().load(GEX::FontID::Main, "Media/Sansation.ttf");

	textures_.load(GEX::TextureID::TitleScreen, "Media/Textures/TitleScreenBig.png");
	textures_.load(GEX::TextureID::Face, "Media/Textures/Face.png");

	statisticsText_.setFont(GEX::FontManager::getInstance().get(GEX::FontID::Main));
	statisticsText_.setPosition(5.0f, 5.0f);
	statisticsText_.setCharacterSize(12.0f);
	statisticsText_.setString("Frames / Second = \nTime / Update =");

	registerStates();
	stateStack_.pushState(GEX::StateID::Title);
}

void Aplication::run()
{
	sf::Clock clock;

	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	while (window_.isOpen())
	{

		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > TimePerFrame)
		{

			timeSinceLastUpdate -= TimePerFrame;

			processInputs();
			update(TimePerFrame);

			if (stateStack_.isEmpty())
			{
				window_.close();
			}
		}

		updateStatistics(timeSinceLastUpdate);

		render();
	}
}

void Aplication::processInputs()
{
	sf::Event event;
	while (window_.pollEvent(event))
	{
		stateStack_.handleEvent(event);
		if (event.type == sf::Event::Closed)
		{
			window_.close();
		}
	}
}

void Aplication::update(sf::Time dt)
{
	stateStack_.update(dt);
}

void Aplication::render()
{
	window_.clear();
	stateStack_.draw();

	window_.setView(window_.getDefaultView());
	window_.draw(statisticsText_);
	window_.display();
}

void Aplication::updateStatistics(sf::Time dt)
{
	statisticsUpdateTime_ += dt;
	statisticsNumFrames_++;

	if (statisticsUpdateTime_ > sf::seconds(1))
	{
		statisticsText_.setString(
			"Frames / Second = " + std::to_string(statisticsNumFrames_) + "\n" +
			"Time / Update   = " + std::to_string(statisticsUpdateTime_.asMicroseconds() / statisticsNumFrames_) + "ms"
		);
		statisticsNumFrames_ = 0;
		statisticsUpdateTime_ -= sf::seconds(1);
	}
}

void Aplication::registerStates()
{
	stateStack_.registerState<TitleState>(GEX::StateID::Title);
	stateStack_.registerState<MenuState>(GEX::StateID::Menu);
	stateStack_.registerState<GameState>(GEX::StateID::Game);
	stateStack_.registerState<PauseState>(GEX::StateID::Pause);
	stateStack_.registerState<GexState>(GEX::StateID::Gex);
	stateStack_.registerState<GameOverState>(GEX::StateID::GameOver);

}
