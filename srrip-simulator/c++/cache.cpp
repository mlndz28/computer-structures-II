#include "cache.hpp"
#include <vector>
#include <iostream>
using namespace std;

// initializeCache fills a 2D vector with its second dimension components for dy elements
void cache::initializeCache(vector<vector<cacheLine>> *cache, int associativity){
	for(int i=0; i<(*cache).size(); i++) {
		vector<cacheLine> set(associativity);
		(*cache)[i] = set;
    }
}

// requestCache handles a memory access request by the CPU
void cache::requestCache( vector<vector<cacheLine>> *cache, memoryRequest request, stats *stats, specs specs ) {
	int tag = (request.address / specs.lineSize) / specs.cacheBlocks;
	int index = (request.address / specs.lineSize) % specs.cacheBlocks;
	int hit = -1;
	for( int i = 0; i < specs.associativity && hit == -1; i++) {
		if( tag == (*cache)[index][i].tag) {
			hit = i;
		}
	}
	if(hit != -1){ //hit
		(*cache)[index][hit].rrpv = 0;
		if(request.write){
			(*stats).storeHits++;
			(*cache)[index][hit].dirty = true; // line has been modified
		}else{
			(*stats).loadHits++;
		}
	}else{ //miss
		bool replaced = false;
		while(!replaced){ // until the line has been inserted into the cache
			for( int i = 0; i < specs.associativity && !replaced; i++ ){
				if ((*cache)[index][i].rrpv >= specs.mRrpvMax) {
					if ((*cache)[index][i].tag != 0 && (*cache)[index][i].dirty) {
						(*stats).evictions++;
					}
					(*cache)[index][i].rrpv = specs.mRrpvMax - 1;
					(*cache)[index][i].tag = tag;
					(*cache)[index][i].dirty = request.write; //set dirty to true if it's a write
					replaced = true;                          //finish loop
				}
			}
			if(!replaced){
				for( int i = 0; i < specs.associativity; i++ ){ //increment all RRPVs
					(*cache)[index][i].rrpv++;
				}
			}
		}
		if(request.write) {
			(*stats).storeMisses++;
		} else {
			(*stats).loadMisses++;
		}
	}
}
