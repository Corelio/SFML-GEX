/**
* @file
* DataTables.cpp
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
#include "DataTables.h"

namespace GEX
{
	std::map<AircraftType, AircraftData> initializeAircraftData()
	{

		std::map<AircraftType, AircraftData> data;
		//Eagle
		data[AircraftType::Eagle].hitpoints = 100;
		data[AircraftType::Eagle].speed = 200.f;
		data[AircraftType::Eagle].texture = TextureID::Eagle;
		data[AircraftType::Eagle].fireInterval = sf::seconds(1);
		data[AircraftType::Eagle].missileAmount = 12;

		//Raptor
		data[AircraftType::Raptor].hitpoints = 20;
		data[AircraftType::Raptor].speed = 80.f;
		data[AircraftType::Raptor].texture = TextureID::Raptor;
		data[AircraftType::Raptor].directions.emplace_back(Direction(45.f, 80.f)); //Emplace back avooid create a temp object - saves resources
		data[AircraftType::Raptor].directions.emplace_back(Direction(-45.f, 160.f));
		data[AircraftType::Raptor].directions.emplace_back(Direction(45.f, 80.f));
		data[AircraftType::Raptor].fireInterval = sf::Time::Zero;
		data[AircraftType::Raptor].missileAmount = 0;

		//Avenger
		data[AircraftType::Avenger].hitpoints = 40;
		data[AircraftType::Avenger].speed = 50.f;
		data[AircraftType::Avenger].texture = TextureID::Avenger;
		data[AircraftType::Avenger].directions.emplace_back(Direction(45.f, 50.f)); //Emplace back avooid create a temp object - saves resources
		data[AircraftType::Avenger].directions.emplace_back(Direction(0.f, 50.f));
		data[AircraftType::Avenger].directions.emplace_back(Direction(-45.f, 100.f));
		data[AircraftType::Avenger].directions.emplace_back(Direction(0.f, 50.f));
		data[AircraftType::Avenger].directions.emplace_back(Direction(45.f, 50.f));
		data[AircraftType::Avenger].fireInterval = sf::seconds(4);
		data[AircraftType::Avenger].missileAmount = 0;

		return data;
	}
	std::map<Projectile::Type, ProjectileData> initializeProjectileData()
	{
		std::map<Projectile::Type, ProjectileData> data;

		
		data[Projectile::Type::AlliedBullet].damage = 10;
		data[Projectile::Type::AlliedBullet].speed = 500.f;
		data[Projectile::Type::AlliedBullet].texture = TextureID::Bullet;

		data[Projectile::Type::EnemyBullet].damage = 10;
		data[Projectile::Type::EnemyBullet].speed = 500.f;
		data[Projectile::Type::EnemyBullet].texture = TextureID::Bullet;

		data[Projectile::Type::Missile].damage = 200;
		data[Projectile::Type::Missile].speed = 250.f;
		data[Projectile::Type::Missile].texture = TextureID::Missile;

		return data;
	}
	std::map<Pickup::Type, PickupData> initializePickupData()
	{
		std::map<Pickup::Type, PickupData> data;

		data[Pickup::Type::HealthRefill].texture = TextureID::HealthRefill;
		data[Pickup::Type::HealthRefill].action = [](Aircraft& a) {a.repair(25); };

		data[Pickup::Type::MissileRefill].texture = TextureID::MissileRefill;
		data[Pickup::Type::MissileRefill].action = [](Aircraft& a) {a.collectMissiles(3); };

		data[Pickup::Type::FireSpread].texture = TextureID::FireSpread;
		data[Pickup::Type::FireSpread].action = [](Aircraft& a) {a.increaseFireSpread(); };

		data[Pickup::Type::FireRate].texture = TextureID::FireRate;
		data[Pickup::Type::FireRate].action = [](Aircraft& a) {a.increaseFireRate(); };

		return data;
	}
}
