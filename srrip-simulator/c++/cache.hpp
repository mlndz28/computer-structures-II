#ifndef CACHE_H
#define CACHE_H

#include <vector>

namespace cache{
	// Request to memory from the CPU
	struct memoryRequest{
		unsigned int address;
		bool write;
	};

	// Individual line of a cache structure
	struct cacheLine{
		unsigned int tag;
		unsigned int rrpv = 3;
		bool dirty = false;
	};

	// Simulation statistics
	struct stats {
		unsigned int evictions = 0;
		unsigned int loadHits = 0;
		unsigned int storeHits = 0;
		unsigned int loadMisses = 0;
		unsigned int storeMisses = 0;
		unsigned int totalEntries = 0;
	};

	// Cache characteristics
	struct specs{
		unsigned int lineSize;
		unsigned int cacheBlocks;
		unsigned int associativity;
		unsigned int cacheSize;
		unsigned int mRrpvMax;

	};

	void initializeCache(std::vector<std::vector<cacheLine>> *cache, int associativity);
	void requestCache(std::vector<std::vector<cacheLine>> *cache, memoryRequest request, stats *stats, specs specs);
};

#endif
