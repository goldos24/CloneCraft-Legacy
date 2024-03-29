#pragma once

#include <fstream>
#include "../maths/maths.h"
#include "../world/world.h"
#include "../world/blocks.h"
#include "../entity/Entity.h"

namespace player
{
	const float ROTATION_X_BOUNDS = 89.f;

	struct Player : Entity
	{
		Player();

		static float playerReach;
		float mouseSensitivity = 1.f;

		void rotate(float X, float Y, float mouseSpeed);

		void renderModel();
		void update(Game&, float);

		void saveDataToFile(std::string worldFileName);
		void loadDataFromFile(std::string worldFileName);
		void parseSpecialProperty(std::string propertyName, std::istream& entityParserStream);
	};
}