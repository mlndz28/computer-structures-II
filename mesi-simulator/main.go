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
	lineSize      int   // how many bytes per cache line
	l1Size        int   // in bytes
	l2Size        int   // in bytes
	associativity int   // size of the set
	helpFlag      *bool // show usage
	l1Blocks      int   // number of sets
	l2Blocks      int   // number of sets
	mRrpvMax      int   // 2^m - 1
	CPUs          int   // number of processing units
)

// Simulation statistics
type stats struct {
	invalidations []int
	l1Hits        []int
	l2Hits        int
	l1Misses      []int
	l2Misses      int
	accesses      int
}

func init() {
	associativityFlag := flag.Int("a", 2, "associativity")
	l1CacheSizeFlag := flag.Int("t1", 16, "cache size (KB)")
	l2CacheSizeFlag := flag.Int("t2", 128, "cache size (KB)")
	lineSizeFlag := flag.Int("l", 32, "line size (bytes)")
	cpuFlag := flag.Int("p", 2, "number of processing units")
	helpFlag = flag.Bool("h", false, "help")
	flag.Parse()
	associativity = *associativityFlag
	l1Size = (*l1CacheSizeFlag) * 1024
	l2Size = (*l2CacheSizeFlag) * 1024
	lineSize = *lineSizeFlag
	CPUs = *cpuFlag
	mRrpvMax = map[bool]int{true: 3, false: 1}[associativity > 2]
	l1Blocks = l1Size / lineSize / associativity
	l2Blocks = l2Size / lineSize
}

func main() {
	info, err := os.Stdin.Stat()
	handleError(err)
	if (info.Mode()&os.ModeCharDevice) != 0 || *helpFlag { //if data has not been piped or usage chart has been requested
		printUsage()
		return
	}

	reader := bufio.NewReader(os.Stdin)
	var output []rune

	l1 := make([][][]cacheLine, CPUs)
	for i := 0; i < CPUs; i++ {
		l1Single := make([][]cacheLine, l1Blocks)
		initializeCache(&l1Single, associativity, l1Blocks)
		l1[i] = l1Single
	}
	l2 := make([]cacheLine, l2Blocks)
	cache := cache{l1: l1, l2: l2}
	l1Hits := make([]int, CPUs)
	l1Misses := make([]int, CPUs)
	invalidations := make([]int, CPUs)
	stats := stats{invalidations: invalidations, l1Hits: l1Hits, l1Misses: l1Misses, accesses: 1}
	for {
		input, _, err := reader.ReadRune()
		if err != nil && err == io.EOF {
			break
		}
		output = append(output, input)

		if input == '\n' {
			handleError(err)
			address, _ := hex.DecodeString(string(output[4:12])) //string with hex representation to actual hex
			requestCache(&cache, memoryRequest{
				address: binary.BigEndian.Uint32(address), // transforms hex to Uint32
				cpu:     map[bool]int{true: 0, false: 1}[stats.accesses%4 == 0],
				write:   output[2] == '1',
			}, &stats)
			stats.accesses++
			output = nil
		}
	}
	fmt.Println("\nCache parameters:")
	fmt.Printf("\tL1 Cache Associativity: %d\n", associativity)
	fmt.Printf("\tL1 Cache size (KB): %d\n", l1Size/1024)
	fmt.Printf("\tL2 Cache size (KB): %d\n", l2Size/1024)
	fmt.Printf("\tBlock Size (bytes): %d\n", lineSize)
	fmt.Println("Simulation result:")
	for i := 0; i < CPUs; i++ {
		fmt.Printf("\tCPU%d:\n", i+1)
		fmt.Printf("\t\tL1 miss rate: %v\n", float64(stats.l1Misses[i])/float64(stats.l1Hits[i]+stats.l1Misses[i]))
		fmt.Printf("\t\tCoherence invalidations: %v\n", stats.invalidations[i])
	}
	fmt.Printf("\n\tOverall miss rate: %v\n", float64(stats.l2Misses)/float64(stats.accesses))
	//fmt.Printf("\tL2 misses: %v\n", stats.l2Misses)
	//fmt.Printf("\tTotal entries: %v\n", stats.accesses)
}

// printUsage prints a chart with the program's input parameters and flags
func printUsage() {
	usage := "\nUsage: gunzip -c <compressedTraceFile.gz> | cache [options]"
	usage += "\n\n\t-a, --a\t\tL1 associativity"
	usage += "\n\n\t-p, --p\t\tNumber of CPUs (each with his own L1)"
	usage += "\n\t-t1, --t1\t\tL1 cache size in KB"
	usage += "\n\t-t2, --t2\t\tL2 cache size in KB"
	usage += "\n\t-l, --l\t\tLine size in bytes"
	usage += "\n\t-h, --h\t\tShow usage\n"
	fmt.Println(usage)
}

// handleError is a common error handling function
func handleError(err error) {
	if err != nil {
		panic(err)
	}
}
