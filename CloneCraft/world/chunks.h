#pragma once
#include <vector>
#include <memory>
#include <future>
#include <mutex>
#include "../entity/Entity.h"
#include "../game/forwardDeclarations.h"
#include "../world/blocks.h"
#include "../renderer/renderer.h"
#include "../maths/maths.h"
#include "../renderer/renderData.h"
#include "../terrain/terrainGenerator.h"
#include "../world/chunkData.h"
#include "../world/blockInfo.h"

namespace chunks 
{
	int coordinateToIndex(int x, int y, int z);

	void indexToCoordinate(int i, int& x, int& y, int& z);

	bool isCoordinateInBounds(int x, int y, int z);

	struct Chunk
	{
		Chunk();

		~Chunk();

		std::mutex faceMutex;

		bool isVisible = false;

		std::vector<renderData::BlockFace> renderData = std::vector<renderData::BlockFace>();

		maths::Vec3<int> chunkPos;

		blox::ID blocks[maths::cubeof(size)];

		std::vector<std::shared_ptr<Entity>> entities;

		blox::ID getBlock(int x, int y, int z);

		void updateEntities(Game&, float elapsedTime);

		void renderEntities(maths::Vec3<float>, maths::Vec3<float>);

		void setBlock(blox::ID id, int x, int y, int z);

		void setBlockUnsafely(blox::ID id, int x, int y, int z);

		void placeBlock(blox::ID id, int x, int y, int z);

		void calculateAndPushBlock(int x, int y, int z, std::vector<renderData::BlockFace>& renderData);

		void calculateAndPushFace(int x, int y, int z, int actualX, int actualY, int actualZ,
			maths::Vec3<int> offset,
			bool isSelectedBlockTransparent, bool swapSides,
			facePos::FacePosition swappedFacePosition, std::vector<renderData::BlockFace>& renderData);

		renderData::BlockFace calculateFace(int x, int y, int z, int actualX, int actualY, int actualZ,
			maths::Vec3<int> offset,
			bool swapSides,
			facePos::FacePosition facePosition, std::vector<renderData::BlockFace>& renderData);

		void calculateFaces();

		void Render(texStorage::TextureAtlas& texAtlas);
	};

	std::shared_ptr<Chunk> initFlatChunk(maths::Vec3<int> chunkPos);

	std::shared_ptr<Chunk> initNormalChunk(maths::Vec3<int> chunkPos, float seed);
}