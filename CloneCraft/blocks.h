#pragma once
#include <string>
#include <iostream>
#include "textures.h"

namespace blox 
{
	enum ID : unsigned char
	{
		air,
		stone,
		grass,
		dirt,

		enumSize
	};

	void eliminateFalseID(ID& id)
	{
		if (id >= enumSize)  id = air; 
	}

	auto isTransparent(ID blockId) -> bool
	{
		return blockId == air;
	}

	struct Block
	{
		Block() {}
		Block(ID id, std::string name, textures::BlockTexture* texture)
		{
			this->id = id;
			this->name = name;
			this->texture = texture;
		}

		textures::BlockTexture* texture;
		ID id;
		std::string name;
	};

	namespace 
	{
		Block* InitBlocks()
		{
			auto* blocks = new Block[256];
			blocks[air] = Block(air, "air", textures::blockTextures::stone); //TODO nullptr
			
			blocks[stone] = Block(stone, "stone", textures::blockTextures::stone);
			blocks[grass] = Block(grass, "grass", textures::blockTextures::grass);
			blocks[dirt] = Block(dirt, "dirt", textures::blockTextures::dirt);

			return blocks;
		}

		Block* blocks = InitBlocks();
	}

	Block& getByID(ID id)
	{
		return blocks[id];
	}

	Block& getByName(std::string name)
	{
		for (int i = 0; i < enumSize; i++)
		{
			if (blocks[i].name == name)
			{
				return blocks[i];
			}
		}
		std::cout << "Couldn't find block with ID: " << name << std::endl;
		return blocks[air];
	}
}

std::ostream& operator << (std::ostream& output, blox::ID id)
{
	return output << blox::getByID(id).name;
}
