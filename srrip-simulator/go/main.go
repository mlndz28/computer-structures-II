package main

import (
	"bufio"
	"encoding/binary"
	"encoding/hex"
	"flag"
	"fmt"
	"io"
	"os"
)

var (
	lineSize      int //how many bytes per cache line
	cacheSize     int //in bytes
	associativity int //size of the set
	cacheBlocks   int //number of sets
	mRrpvMax      int //2^m - 1
)

// Simulation statistics
type stats struct {
	evictions   int
	loadHits    int
	storeHits   int
	loadMisses  int
	storeMisses int
}

func init() {
	associativityFlag := flag.Int("a", 2, "associativity")
	cacheSizeFlag := flag.Int("t", 16, "cache size (KB)")
	lineSizeFlag := flag.Int("l", 32, "line size (bytes)")
	flag.Parse()
	associativity = *associativityFlag
	cacheSize = (*cacheSizeFlag) * 1024
	lineSize = *lineSizeFlag
	mRrpvMax = map[bool]int{true: 3, false: 1}[associativity > 2]
	cacheBlocks = cacheSize / lineSize / associativity
}

func main() {
	info, err := os.Stdin.Stat()
	handleError(err)
	if (info.Mode() & os.ModeCharDevice) != 0 { //if data has not been piped
		fmt.Println("Usage: gunzip -c <tracename>.trace.gz | cache -a <associativity> -t <cache size in KB> -l <line size in bytes>")
		return
	}

	reader := bufio.NewReader(os.Stdin)
	var output []rune

	cache := make([][]cacheLine, cacheBlocks)
	initializeCache(&cache)
	stats := stats{} //start at 0 by default
	for {
		input, _, err := reader.ReadRune()
		if err != nil && err == io.EOF {
			break
		}
		output = append(output, input)
		if input == '\n' {
			address, _ := hex.DecodeString(string(output[4:12])) //string with hex representation to actual hex
			requestCache(&cache, memoryRequest{
				address: binary.BigEndian.Uint32(address), // transforms hex to Uint32
				write:   output[2] == '1',
			}, &stats)
			output = nil
		}
	}
	totalEntries := stats.loadMisses + stats.storeMisses + stats.loadHits + stats.storeHits
	fmt.Println("\nCache parameters:")
	fmt.Printf("\tCache size (KB): %d\n", cacheSize/1024)
	fmt.Printf("\tCache Associativity: %d\n", associativity)
	fmt.Printf("\tCache Block Size (bytes): %d\n", lineSize)
	fmt.Println("Simulation result:")
	fmt.Printf("\tOverall miss rate: %v\n", float64(stats.loadMisses+stats.storeMisses)/float64(totalEntries))
	fmt.Printf("\tRead miss rate: %v\n", float64(stats.loadMisses)/float64(stats.loadHits+stats.loadMisses))
	fmt.Printf("\tDirty evictions: %d\n", stats.evictions)
	fmt.Printf("\tLoad misses: %d\n", stats.loadMisses)
	fmt.Printf("\tStore misses: %d\n", stats.storeMisses)
	fmt.Printf("\tLoad hits: %d\n", stats.loadHits)
	fmt.Printf("\tStore hits: %d\n", stats.storeHits)
	fmt.Printf("\tTotal hits: %d\n", stats.storeHits+stats.loadHits)
}

// handleError is a common error handling function
func handleError(err error) {
	if err != nil {
		panic(err)
	}
}
