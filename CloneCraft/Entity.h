#pragma once

#include "world.h"

struct Entity
{
	Entity()
	{}

	maths::Vec3<float> position, rotation, movement;
	maths::Vec3<float> hitbox;

	bool isStandingOnASurface(world::World& world)
	{
		for (float i = -this->hitbox.x / 2.f; i < this->hitbox.x / 2.f; i += this->hitbox.x / 2.f)
			for (float k = -this->hitbox.z / 2.f; k < this->hitbox.z / 2.f; k += this->hitbox.z / 2.f)
			{
				if (world.getBlockID(this->position + maths::Vec3<float>(i, -this->hitbox.y, k)) != blox::air) return true;
			}
		return false;
	}


	void applyMovement(float elapsedTime)
	{
		this->position += this->movement * elapsedTime;
	}

	void applyAcceleration(float elapsedTime, maths::Vec3<float> acceleration)
	{
		this->movement += acceleration * elapsedTime;
	}

	void applyFriction(float elapsedTime, float friction)
	{
		float frictionFactor = (1 < friction * elapsedTime) ? 0.f : 1 - friction * elapsedTime;
		this->movement = this->movement * frictionFactor;
	}

	bool isColliding(world::World& world)
	{
		for (float i = -this->hitbox.x / 2.f; i < this->hitbox.x / 2.f; i++)
			for (float j = 0; j < this->hitbox.y; j++)
				for (float k = -this->hitbox.z / 2.f; k < this->hitbox.z / 2.f; k++)
				{
					if (world.getBlockID(this->position + maths::Vec3<float>(
						i > this->hitbox.x / 2.f ? this->hitbox.x / 2.f : i,
						-(j + 1 > this->hitbox.y ? this->hitbox.y : j),
						k > this->hitbox.z / 2.f ? this->hitbox.z / 2.f : k
						)))
						return true;
				}
		return false;
	}

	void clipMovement(float elapsedTime, world::World& world)
	{
		auto appliedMovementVector = this->movement * elapsedTime;
		for (float j = 0; j < this->hitbox.y; j++)
		{
			if (world.getBlockID(this->position + maths::Vec3<float>(appliedMovementVector.x + this->movement.x > 0 ? 0.35 : -0.35, -j, 0.f)) != blox::air) this->movement.x = 0.f;
			// TODO fix bug with ze collision
			for (float i = -this->hitbox.x / 2.f; i < this->hitbox.x / 2.f; i += this->hitbox.x / 2.f)
				for (float k = -this->hitbox.z / 2.f; k < this->hitbox.z / 2.f; k += this->hitbox.z / 2.f)
					if (world.getBlockID(this->position + maths::Vec3<float>(i, appliedMovementVector.y + this->movement.y > 0 ? 0.f : -(j + 1 > this->hitbox.y ? this->hitbox.y : j), k)) != blox::air) this->movement.y = 0.f;
			if (world.getBlockID(this->position + maths::Vec3<float>(0.f, -j, appliedMovementVector.z + this->movement.z > 0 ? 0.35 : -0.35)) != blox::air) this->movement.z = 0.f;
		}
	}
};