#pragma once
#include "../world/blocks.h"
#include "../renderer/renderer.h"
#include "../maths/maths.h"
#include "../renderer/renderData.h"
#include <vector>
#include <memory>
#include "../terrain/terrainGenerator.h"
#include "../world/chunkData.h"
#include "../world/blockInfo.h"

namespace chunks 
{
	int coordinateToIndex(int x, int y, int z)
	{
		return maths::coord::coordinateToIndex(x, y, z, size);
	}

	void indexToCoordinate(int i, int& x, int& y, int& z)
	{
		maths::coord::indexToCoordinate(i, x, y, z, size);
	}

	bool isCoordinateInBounds(int x, int y, int z)
	{
		return maths::is3dCoordInRange(x, y, z, 0, size);
	}

	struct Chunk
	{
		Chunk()
		{
			//this->calculateFaces();
		}

		~Chunk()
		{
			
		}

		bool isVisible = false;

		std::vector<renderData::BlockFace> renderData = std::vector<renderData::BlockFace>();

		maths::Vec3<int> chunkPos;

		blox::ID blocks[maths::cubeof(size)];



		blox::ID getBlock(int x, int y, int z)
		{
			if ( ! isCoordinateInBounds(x, y, z) )
				return blox::air;
			return this-> blocks[ coordinateToIndex(x, y, z) ];
		}

		void setBlock(blox::ID id, int x, int y, int z)
		{
			if (!isCoordinateInBounds(x, y, z))
				return;
			this-> blocks[coordinateToIndex(x, y, z)] = id;
		}

		void placeBlock(blox::ID id, int x, int y, int z)
		{
			this->setBlock(id, x, y, z);
			this->calculateFaces();
		}

		void calculateAndPushBlock(int x, int y, int z)
		{
			bool isSelectedBlockTransparent = blox::isTransparent(this->getBlock(x, y, z));
			bool swapSides = !isSelectedBlockTransparent;

			int aX = x + this->chunkPos.x;
			int aY = y + this->chunkPos.y;
			int aZ = z + this->chunkPos.z;

			calculateAndPushFace(x, y, z, aX, aY, aZ, maths::unitVectors::down, isSelectedBlockTransparent, swapSides, facePos::top);
			calculateAndPushFace(x, y, z, aX, aY, aZ, maths::unitVectors::back, isSelectedBlockTransparent, swapSides, facePos::front);
			calculateAndPushFace(x, y, z, aX, aY, aZ, maths::unitVectors::right, isSelectedBlockTransparent, swapSides, facePos::left);
		}

		void calculateAndPushFace(int x, int y, int z, int actualX, int actualY, int actualZ,
			maths::Vec3<int> offset,
			bool isSelectedBlockTransparent, bool swapSides,
			facePos::FacePosition swappedFacePosition)
		{
			bool isOffsetBlockTransparent = blox::isTransparent(this->getBlock(x + offset.x, y + offset.y, z + offset.z));

			if (isSelectedBlockTransparent ^ isOffsetBlockTransparent)
				this->renderData.push_back(
					calculateFace(x, y, z, actualX, actualY, actualZ,
						offset, swapSides, swappedFacePosition));
		}

		renderData::BlockFace calculateFace(int x, int y, int z, int actualX, int actualY, int actualZ,
			maths::Vec3<int> offset,
			bool swapSides,
			facePos::FacePosition facePosition)
		{
			return renderData::makeFace(
				(swapSides) ? 
				this->getBlock(x, y, z)
				: 
				this->getBlock(x + offset.x, y + offset.y, z + offset.z), 
				actualX, actualY, actualZ, swapSides, facePosition);
		}

		void calculateFaces()
		{
			this->renderData.clear();
			for (int i = 0; i <= size; i++)
				for (int j = 0; j <= size; j++)
					for (int k = 0; k <= size; k++)
					{
						calculateAndPushBlock(i, j, k);
					}
		}

		void Render() //TODO replace loop
		{
			for (auto face : renderData)
				face.render();
		}

	};

	std::shared_ptr<Chunk> initFlatChunk(maths::Vec3<int> chunkPos)
	{
		std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>();
		chunk->chunkPos = chunkPos;
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
				{
					chunk->setBlock(
						(j > 1  ? blox::air : blox::stone),
						i, j, k
					);
				}
		chunk->setBlock(blox::grass, 8, 14, 8);
		chunk->calculateFaces();
		return chunk;
	}

	std::shared_ptr<Chunk> initNormalChunk(maths::Vec3<int> chunkPos)
	{
		std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>();
		chunk->chunkPos = chunkPos;

		if (chunkPos.y < 0 || chunkPos.y >= 16)
		{
			for (blox::ID& id : chunk->blocks)
			{
				id = blox::air;
			}
			chunk->calculateFaces();
			return chunk;
		}

		auto heightMap = terrainGen::createHeightMap( maths::Vec2<float>( float(chunkPos.x), float(chunkPos.z)) );

		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
				{
					float height = heightMap[i * 16 + k];
					chunk->setBlock(
						(height < float(j - 12) ? blox::air : 
						 height < float(j - 11) ? blox::grass :
						 height < float(j - 8) ? blox::dirt : blox::stone),
						i, j, k
					);
				}

		chunk->calculateFaces();
		return chunk;
	}
}