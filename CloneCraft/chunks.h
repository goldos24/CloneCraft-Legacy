#pragma once
#include "blocks.h"
#include "renderer.h"

namespace chunks 
{

	const int size = 16;

	auto coordinateToIndex(int x, int y, int z) -> int
	{
		return x * size * size + y * size + z;
	}

	auto indexToCoordinate(int i, int& x, int& y, int& z)
	{
		x = i / size / size;
		i -= x * size * size;
		y = i / size;
		i -= y * size;
		z = i;
		i -= z;
	}

	auto isCoordinateInBounds(int x, int y, int z)
	{
		return (

			x >= 0 && x < size && // Better Code may be possible
			y >= 0 && y < size &&
			z >= 0 && z < size

			);
	}

	struct Chunk
	{
		blox::ID blocks[size * size * size];

		auto getBlock(int x, int y, int z)
		{
			if ( ! isCoordinateInBounds(x, y, z) )
				return blox::air;
			return this-> blocks[ coordinateToIndex(x, y, z) ];
		}

		auto setBlock(blox::ID id, int x, int y, int z)
		{
			if (!isCoordinateInBounds(x, y, z))
				return;
			this-> blocks[coordinateToIndex(x, y, z)] = id;
		}

		auto renderBlock(int x, int y, int z)
		{
			bool swapSides = !blox::isTransparent(this->getBlock(x, y, z));
			if (blox::isTransparent(this->getBlock(x, y, z)) ^
				blox::isTransparent(this->getBlock(x, y - 1, z)))
				renderer::block::drawBottomFace( 
					(swapSides) ?
					this->getBlock(x, y, z)
					:
					this->getBlock(x, y - 1, z)
					, x, y, z, swapSides);

			if (blox::isTransparent(this->getBlock(x, y, z)) ^
				blox::isTransparent(this->getBlock(x, y, z - 1)))
				renderer::block::drawFrontFace(
					(swapSides) ?
					this->getBlock(x, y, z)
					:
					this->getBlock(x, y, z - 1)
					, x, y, z, swapSides);

			if (blox::isTransparent(this->getBlock(x, y, z)) ^
					blox::isTransparent(this->getBlock(x - 1, y, z)))
				renderer::block::drawLeftFace(
					(swapSides) ?
					this->getBlock(x, y, z)
					:
					this->getBlock(x - 1, y, z)
					, x, y, z, swapSides);
		}

		auto renderBlockWithIndex(int i)
		{
			int x, y, z;
			indexToCoordinate(i, x, y, z);
			this-> renderBlock( x, y, z );
		}

		auto Render() //TODO replace loop
		{
			for (int i = 0; i < (sizeof(this->blocks) / sizeof(blox::ID)); i++)
				this->renderBlockWithIndex(i);
		}

	};

	auto initFlatChunk()
	{
		Chunk chunk;
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
				{
					chunk.setBlock(
						(j > 1 || i > 14 || k > 14 ? blox::air : blox::stone),
						i, j, k
					);
				}
		chunk.setBlock(blox::stone, 8, 14, 8);
		return chunk;
	}
}