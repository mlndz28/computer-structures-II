package main

// coherence manages the coherence bits in the L1 cache lines
func coherence(cache *cache, tag, index uint32, request memoryRequest, stats *stats) {
	line := ((*cache).l1)[request.cpu][index][0]
	if request.write {
		((*cache).l1)[request.cpu][index][0].coherenceState = m
	} else {
		if line.coherenceState == i {
			((*cache).l1)[request.cpu][index][0].coherenceState = map[bool]uint8{true: e, false: s}[isExclusive(cache, tag, index, request.cpu)]
		}
	}
	busIssue(cache, tag, index, request.cpu, request.write, stats)
}

// isExclusive validates a memory address as exclusive in a memory array
func isExclusive(cache *cache, tag, index uint32, requestOrigin int) bool {
	for i1 := 0; i1 < CPUs; i1++ {
		for i2 := 0; i2 < associativity && i1 != requestOrigin; i2++ {
			if tag == ((*cache).l1)[i1][index][i2].tag && ((*cache).l1)[i1][index][i2].coherenceState != i {
				return false // meaning there is a valid copy in other L1 cache
			}
		}
	}
	return true
}

// busIssue manages the coherence states of matching memory lines, if present, when some cache either reads or writes into it
func busIssue(cache *cache, tag, index uint32, requestOrigin int, write bool, stats *stats) {
	for i1 := 0; i1 < CPUs; i1++ {
		for i2 := 0; i2 < associativity && i1 != requestOrigin; i2++ {
			if tag == ((*cache).l1)[i1][index][i2].tag && ((*cache).l1)[i1][index][i2].coherenceState != i { // check if a valid line is in the cache
				if write {
					((*cache).l1)[i1][index][i2].coherenceState = i // invalidate the line if another cache has a modified copy
					(*stats).invalidations[i2]++
				} else {
					((*cache).l1)[i1][index][i2].coherenceState = s // set on share if some other cache requests to read this cache's copy
				}
			}
		}
	}
}
