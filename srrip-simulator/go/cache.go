package main

import "math"

// Request to memory from the CPU
type memoryRequest struct {
	address uint32
	write   bool
}

// Individual line of a cache structure
type cacheLine struct {
	tag   uint32
	rrpv  int
	dirty bool
}

// initializeCache fills a 2D array with its second dimension component for dy elements
func initializeCache(cache *[][]cacheLine) {
	for i1 := 0; i1 < cacheBlocks; i1++ {
		(*cache)[i1] = make([]cacheLine, associativity)
		for i2 := 0; i2 < associativity; i2++ {
			(*cache)[i1][i2].rrpv = mRrpvMax
		}
	}
}

// requestCache handles a memory access request by the CPU
func requestCache(cache *[][]cacheLine, request memoryRequest, stats *stats) {
	index := (request.address >> uint32(math.Log2(float64(lineSize)))) % uint32(cacheBlocks) // uses module to define the set location
	tag := (request.address >> uint32(math.Log2(float64(lineSize)))) / uint32(cacheBlocks)
	hit := -1 //index of the set where the hit was triggered
	for i := 0; i < associativity && hit == -1; i++ {
		if tag == (*cache)[index][0].tag {
			hit = i
		}
	}
	if hit != -1 { //hit
		(*cache)[index][hit].rrpv = 0
		if request.write {
			(*stats).storeHits++
			(*cache)[index][hit].dirty = true // line has been modified
		} else {
			(*stats).loadHits++
		}
	} else { //miss
		replaced := false
		for !replaced { // until the line has been inserted into the cache
			for i := 0; i < associativity && !replaced; i++ {
				if (*cache)[index][i].rrpv == mRrpvMax {
					if (*cache)[index][i].tag != 0 && (*cache)[index][i].dirty {
						(*stats).evictions++
					}
					(*cache)[index][i].rrpv = mRrpvMax - 1
					(*cache)[index][i].tag = tag
					(*cache)[index][i].dirty = request.write //set dirty to true if it's a write
					replaced = true                          //finish loop
				}
			}
			if !replaced {
				for i := 0; i < associativity; i++ { //increment all RRPVs
					(*cache)[index][i].rrpv++
				}
			}
		}
		if request.write {
			(*stats).storeMisses++
		} else {
			(*stats).loadMisses++
		}

	}
}
