#pragma once

#include <iostream>
#include <memory>
#include <map>
#include <cstdint>
#include "../world/chunkData.h"
#include "../world/chunks.h"
#include "../maths/maths.h"
#include "../files/files.h"
#include "../world/blockInfo.h"
#include "../glu/oldFunctions.h"

namespace world
{
	struct World
	{
		World()
		{
			mgr.loadAll();
			for (auto chonk : mgr.chunks)
			{
				chonk->calculateFaces();
				this->chunks[chunks::createKeyFromPosition(chonk->chunkPos).num] = chonk;
			}
			size = maths::cubeof(this->chunkRenderDistance);
			for (int i = 0; i < size; i++)
			{
				int x, y, z;
				maths::coord::indexToCoordinate(i, x, y, z, this->chunkRenderDistance);
				this->loadChunk(maths::Vec3<int>(x * 16, y * 16, z * 16));
			}
		}

		maths::Vec3<int> worldPos = maths::Vec3<int>(0, 0, 0);
		int size;
		int chunkRenderDistance = 11;
		std::map<uint64_t, std::shared_ptr<chunks::Chunk>> chunks;
		saveData::Manager mgr;

		void moveTo(maths::Vec3<int> destination) {
			// Rounding the movement
			destination = chunks::convertToChunkPos(destination);

			// Loading new Chunks
			auto destEnd = destination + maths::Vec3<int>(this->chunkRenderDistance * chunks::size, this->chunkRenderDistance * chunks::size, this->chunkRenderDistance * chunks::size);

			for (int i = destination.x; i < destEnd.x; i += chunks::size)
				for (int j = destination.y; j < destEnd.y; j += chunks::size)
					for (int k = destination.z; k < destEnd.z; k += chunks::size)
					{
						auto currentChunkPos = maths::Vec3<int>(i, j, k);
							this->loadChunk(currentChunkPos);
					}

			this->worldPos = destination;
			if(this->chunks.size() > maths::cubeof(this->chunkRenderDistance) * 100)this->unloadGarbageChunks();
		}

		std::shared_ptr<chunks::Chunk> getChunk(maths::Vec3<int> chunkPos)
		{
			if(!this->containsChunk(chunkPos))
				return chunks::initNormalChunk(chunkPos);
			return this->chunks[chunks::createKeyFromPosition(chunkPos).num];
		}

		blox::ID getBlockID(maths::Vec3<int> blockPos)
		{
			auto blockPosF = maths::convertVec3<int, float>(blockPos);
			auto chunk = this->getChunk(chunks::convertToChunkPos(blockPosF));
			auto positionInsideChunk = maths::convertVec3<float, int>(this->getPlayerPositionInsideCurrentChunk(blockPosF));
			return chunk->getBlock(positionInsideChunk.x, positionInsideChunk.y, positionInsideChunk.z);
		}

		blox::ID getBlockID(maths::Vec3<float> blockPosF)
		{
			auto chunk = this->getChunk(chunks::convertToChunkPos(blockPosF));
			auto positionInsideChunk = maths::convertVec3<float, int>(this->getPlayerPositionInsideCurrentChunk(blockPosF));
			return chunk->getBlock(positionInsideChunk.x, positionInsideChunk.y, positionInsideChunk.z);
		}

		void setBlockID(maths::Vec3<int> blockPos, blox::ID id)
		{
			auto blockPosF = maths::convertVec3<int, float>(blockPos);
			auto chunk = this->getChunk(chunks::convertToChunkPos(blockPosF));
			auto positionInsideChunk = maths::convertVec3<float, int>(this->getPlayerPositionInsideCurrentChunk(blockPosF));
			return chunk->placeBlock(id, positionInsideChunk.x, positionInsideChunk.y, positionInsideChunk.z);
		}

		void setBlockID(maths::Vec3<float> blockPosF, blox::ID id)
		{
			auto chunk = this->getChunk(chunks::convertToChunkPos(blockPosF));
			auto positionInsideChunk = maths::convertVec3<float, int>(this->getPlayerPositionInsideCurrentChunk(blockPosF));
			return chunk->placeBlock(id, positionInsideChunk.x, positionInsideChunk.y, positionInsideChunk.z);
		}

		std::shared_ptr<chunks::Chunk> findChunkFromPlayerPosition(maths::Vec3<float> playerPosition)
		{
			return getChunk(chunks::convertToChunkPos(playerPosition));
		}

		maths::Vec3<float> getPlayerPositionInsideCurrentChunk(maths::Vec3<float> playerPosition)
		{
			maths::Vec3<int> chunkPos = chunks::convertToChunkPos(playerPosition);

			float sX = chunkPos.x;
			float sY = chunkPos.y;
			float sZ = chunkPos.z;

			float pX = maths::mapFromRangeToRange(playerPosition.x, sX, sX + chunks::size, 0, chunks::size);
			float pY = maths::mapFromRangeToRange(playerPosition.y, sY, sY + chunks::size, 0, chunks::size);
			float pZ = maths::mapFromRangeToRange(playerPosition.z, sZ, sZ + chunks::size, 0, chunks::size);

			if (pX < 0.f) { pX += float(chunks::size); }
			if (pY < 0.f) { pY += float(chunks::size); }
			if (pZ < 0.f) { pZ += float(chunks::size); }

			return maths::Vec3<float>(pX, pY, pZ);
		}

		void loadChunk(maths::Vec3<int> chunkPos)
		{
			if (this->containsChunk(chunkPos)) return;

			this->chunks[chunks::createKeyFromPosition(chunkPos).num] =
				this->getChunk(chunkPos)
			;
		}

		void unloadGarbageChunks()
		{
			if(!this->save()) return;

			auto worldSize = maths::Vec3<int>(this->chunkRenderDistance, this->chunkRenderDistance, this->chunkRenderDistance) * chunks::size;
			auto worldEnd = this->worldPos + worldSize * 2;
			auto worldStart = worldPos - worldSize;

			for (auto chunkKeyPair : this->chunks)
			{
				if (!chunkKeyPair.second->chunkPos.isInBounds(worldStart, worldEnd))
				{
					this->chunks.erase(this->chunks.find(chunkKeyPair.first));;
				}
			}

			this->size = this->chunks.size();
		}

		void Render() //TODO replace
		{
			// Making the variable with the best name you've seen in a while
			auto worldEnd = this->worldPos + maths::Vec3<int>(this->chunkRenderDistance * chunks::size, this->chunkRenderDistance * chunks::size, this->chunkRenderDistance * chunks::size);
			for (int i = this->worldPos.x; i < worldEnd.x; i += chunks::size)
				for (int j = this->worldPos.y; j < worldEnd.y; j += chunks::size)
					for (int k = this->worldPos.z; k < worldEnd.z; k += chunks::size)
					{
						auto chunk = this->getChunk(maths::Vec3<int>(i, j, k));
						//if (oldf::glu::simpleProjectRelative(maths::convertVec3<int, float>(chunk->chunkPos)).isInBounds(maths::Vec2<float>(0.f, 0.f), maths::Vec2<float>(1.f, 1.f)))
						if(chunk->isVisible)
						chunk->Render();
					}
		}

		void markVisibleChunks(maths::Vec3<float> cameraRotation)
		{
			// Making the variable with the best name you've seen in a while
			auto virtualOffset = maths::positionFromRotation(cameraRotation) * float(chunks::size) * 3.f;
			glTranslatef(virtualOffset.x, virtualOffset.y, virtualOffset.z);
			auto worldEnd = this->worldPos + maths::Vec3<int>(this->chunkRenderDistance * chunks::size, this->chunkRenderDistance * chunks::size, this->chunkRenderDistance * chunks::size);
			for (int i = this->worldPos.x; i < worldEnd.x; i += chunks::size)
				for (int j = this->worldPos.y; j < worldEnd.y; j += chunks::size)
					for (int k = this->worldPos.z; k < worldEnd.z; k += chunks::size)
					{
						auto chunk = this->getChunk(maths::Vec3<int>(i, j, k));
						chunk->isVisible =
							oldf::glu::simpleProjectRelative(maths::convertVec3<int, float>(chunk->chunkPos)).isInBounds(maths::Vec2<float>(0.f, 0.f), maths::Vec2<float>(1.f, 1.f));
					}
			glTranslatef(-virtualOffset.x, -virtualOffset.y, -virtualOffset.z);
		}

		bool save() 
		{
			std::cout << "Saving the world..." << std::endl;
			for (auto keyChunkPair : this->chunks)
			{
				mgr.addChunk(keyChunkPair.second);
			}
			mgr.saveAll();
			
			std::cout << "Saved the world." << std::endl;
			return true;
		}

		bool containsChunk(maths::Vec3<int> chunkPos)
		{
			auto key = chunks::createKeyFromPosition(chunkPos).num;
			if (this->chunks.find(key) == this->chunks.end()) return false;
			return this->chunks[key]->chunkPos == chunkPos;
		}

		private:

		// Only for state modifications corrupting the data
	};
}