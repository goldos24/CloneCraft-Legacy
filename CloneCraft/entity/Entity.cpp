#include "Entity.h"
#include "../world/world.h"
#include <cmath>
#include <cstdarg>
#include <sstream>


Entity::Entity() :
	id(ID::invalid)
{}

bool Entity::isStandingOnASurface(world::World& world)
{
	for (float i = -this->hitbox.x / 2.f; i <= this->hitbox.x / 2.f; i += this->hitbox.x / 2.f)
		for (float k = -this->hitbox.z / 2.f; k <= this->hitbox.z / 2.f; k += this->hitbox.z / 2.f)
		{
			if (world.getBlockID(this->position + maths::Vec3<float>(i, -this->hitbox.y - 1.f, k)) != blox::air &&
				world.getBlockID(this->position + maths::Vec3<float>(i, -this->hitbox.y, k)) == blox::air &&
				this->position.y - this->hitbox.y >= floorf(this->position.y - this->hitbox.y) - 0.01f && this->position.y - this->hitbox.y < floorf(this->position.y - this->hitbox.y) + 0.01f
				) return true;
		}
	return false;
}


void Entity::applyMovement(float elapsedTime)
{
	this->position += this->movement * elapsedTime;
}

void Entity::applyAcceleration(float elapsedTime, maths::Vec3<float> acceleration)
{
	this->movement += acceleration * elapsedTime;
}

void Entity::applyFriction(float elapsedTime, float friction)
{
	float frictionFactor = (1 < friction * elapsedTime) ? 0.f : 1 - friction * elapsedTime;
	this->movement = this->movement * frictionFactor;
}

bool Entity::isColliding(world::World& world)
{
	for (float i = -this->hitbox.x / 2.f; i <= this->hitbox.x / 2.f; i += this->hitbox.x / 2.f)
		for (float j = 0; j < this->hitbox.y + 0.5f; j += 0.5f)
			for (float k = -this->hitbox.z / 2.f; k <= this->hitbox.z / 2.f; k += this->hitbox.z / 2.f)
			{
				if (world.getBlockID(this->position + maths::Vec3<float>(
					i > this->hitbox.x / 2.f ? this->hitbox.x / 2.f : i,
					-(j > this->hitbox.y ? this->hitbox.y : j),
					k > this->hitbox.z / 2.f ? this->hitbox.z / 2.f : k
					)) != blox::air)
					return true;
			}
	return false;
}

void Entity::clipMovement(float elapsedTime, world::World& world)
{
	auto appliedMovementVector = this->movement * elapsedTime;
	auto possibleNewPosition = this->position;
	auto initialEntityPosition = this->position;

	this->position.x += appliedMovementVector.x;
	if (this->isColliding(world))
	{
		this->movement.x = 0.f;
		this->position.x -= appliedMovementVector.x;
	}

	this->position.y += appliedMovementVector.y;
	if (this->isColliding(world))
	{
		if(this->movement.y < 0)initialEntityPosition.y = round(initialEntityPosition.y - this->hitbox.y) + this->hitbox.y + 0.000001f;
		this->movement.y = 0.f;
		this->position.y -= appliedMovementVector.y;
	}

	this->position.z += appliedMovementVector.z;
	if (this->isColliding(world))
	{
		this->movement.z = 0.f;
		this->position.z -= appliedMovementVector.z;
	}

	this->position = initialEntityPosition;
}

void Entity::render(maths::Vec3<float> cameraPosition, maths::Vec3<float> cameraRotation)
{
	// Restoring the identity (= transformedn't) matrix 

	glLoadIdentity();


	// Camera Transformations
	glRotatef(cameraRotation.x, 1.f, 0.f, 0.f);
	glRotatef(-cameraRotation.y, 0.f, -1.f, 0.f);
	glTranslatef(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z);

	// Entity Transformations
	glTranslatef(this->position.x, this->position.y - this->hitbox.y, this->position.z);
	glRotatef(this->rotation.y, 0, 1, 0);

	// Finally Drawing the Entity
	this->renderModel();
}

void Entity::update(Game&, float) {}
void Entity::renderModel() {}

Entity::ID Entity::idFromString(std::string str)
{
	return static_cast<Entity::ID>(StringToInt(str));
}

// Including all entity files
#include "../mobs/Sheep.h"

std::shared_ptr<Entity> Entity::createEntityFromID(Entity::ID entityID, ...)
{
	va_list args;
	va_start(args, entityID);
	switch (entityID)
	{
	case Entity::ID::player:
		std::cout << "Spawning players isn't possible\n";
		break;
	case Entity::ID::sheep:
		return std::make_shared<Sheep>();
		break;
	default:
		break;
	}

	return nullptr;
}


void Entity::parseEntities(std::string entityData, std::vector<std::shared_ptr<Entity>>& entityVector)
{
	std::istringstream input(entityData);
	std::string entityType;
	std::shared_ptr<Entity> entity;
	std::vector<std::shared_ptr<Entity>> entVector;
	while (input.good())
	{
		std::string command;
		input >> command;
		if (command.size() == 0)
			break;
		if (command == "entity-end")
		{
			entVector.push_back(entity);
		}
		else if (command == "entity")
		{
			input >> entityType;
			entity = createEntityFromID(idFromString(entityType));
		}
		else
			entity->parseProperty(command, input);
	}

	std::swap<std::shared_ptr<Entity>>(entityVector, entVector);
}

void Entity::parseProperty(std::string propertyName, std::istream& entityParserStream)
{
	switch(StringToInt(propertyName))
	{
	case CptrToInt("position"):
		this->position.loadFromIstream(entityParserStream);
		break;
	case CptrToInt("rotation"):
		this->rotation.loadFromIstream(entityParserStream);
		break;
	default:
		this->parseSpecialProperty(propertyName, entityParserStream);
		break;
	}
}

std::string Entity::encode() 
{
	std::string result;
	std::ostringstream stream(result);

	stream << " entity " << IntToString((uint64_t)this->id);

	stream << " position " << this->position.toParseableString() << " rotation " << rotation.toParseableString();

	stream << " entity-end";

	result = stream.str();

	return result;
}
