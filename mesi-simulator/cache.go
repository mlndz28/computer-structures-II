package main

// MESI states
const (
	m uint8 = 3
	e uint8 = 2
	s uint8 = 1
	i uint8 = 0
)

// Request to memory from the CPU
type memoryRequest struct {
	address uint32
	write   bool
	cpu     int
}

// Individual line of a cache structure
type cacheLine struct {
	tag            uint32
	coherenceState uint8
}

// Whole cache structure
type cache struct {
	l1 [][][]cacheLine
	l2 []cacheLine
}

// initializeCache fills a 2D array with its second dimension component for dy elements
func initializeCache(cache *[][]cacheLine, associativity, blocks int) {
	for i1 := 0; i1 < blocks; i1++ {
		(*cache)[i1] = make([]cacheLine, associativity)
	}
}

// requestCache handles a memory access request by the CPU. The cache uses LRU
func requestCache(cache *cache, request memoryRequest, stats *stats) {
	tag := (request.address / uint32(lineSize)) / uint32(l1Blocks)
	index := (request.address / uint32(lineSize)) % uint32(l1Blocks)

	set, cpu := checkL1(cache, tag, index, request.cpu, stats)
	_ = cpu
	if set != -1 && cpu == request.cpu { //hit
		// bring forward the line from its previous position
		temp := []cacheLine{((*cache).l1)[request.cpu][index][set]}
		temp = append(temp, ((*cache).l1)[request.cpu][index][:set]...)
		((*cache).l1)[request.cpu][index] = append(temp, ((*cache).l1)[request.cpu][index][set+1:]...)
		(*stats).l1Hits[request.cpu]++
	} else { //miss
		// evict the last line on the set and insert the new line
		temp := []cacheLine{cacheLine{tag: tag, coherenceState: i}}
		((*cache).l1)[request.cpu][index] = append(temp, ((*cache).l1)[request.cpu][index][:associativity-1]...)
		(*stats).l1Misses[request.cpu]++
		if set == -1 { // not found in any of the other L1 caches
			checkL2(cache, request, stats)
		}
	}
	coherence(cache, tag, index, request, stats)
}

// checkL1 looks for a valid copy in some of the L1 caches
func checkL1(cache *cache, tag, index uint32, requestOrigin int, stats *stats) (int, int) {
	hit := -1
	for i1 := 0; i1 < associativity && hit == -1; i1++ {
		line := ((*cache).l1)[requestOrigin][index][i1]
		if tag == line.tag && line.coherenceState != i { // check on own L1 (priority)
			return i1, requestOrigin
		}
	}
	for i1 := 0; i1 < associativity && hit == -1; i1++ {
		for i2 := 0; i2 < CPUs && hit == -1; i2++ {
			line := ((*cache).l1)[i2][index][i1]
			if i2 != requestOrigin && tag == line.tag && line.coherenceState != i { // check for the line on L1 from other CPUs
				return i1, i2
			}
		}
	}
	return hit, -1
}

// checkL2 looks for some memory address in the L2 cache
func checkL2(cache *cache, request memoryRequest, stats *stats) {
	tag := (request.address / uint32(lineSize)) / uint32(l2Blocks)
	index := (request.address / uint32(lineSize)) % uint32(l2Blocks)
	// pretty straghtforward functioning
	if ((*cache).l2)[index].tag == tag {
		(*stats).l2Hits++
	} else {
		((*cache).l2)[index].tag = tag
		(*stats).l2Misses++
	}
}
