#include <iostream>
#include <bitset>
#include <vector>
#include <string>
#include "cache.hpp"
using namespace std;

cache::specs specs = [&]{
	cache::specs specs;
	specs.associativity = 2;
	specs.cacheSize = 16* 1024;
	specs.lineSize = 32;
	return specs;
}();

// Prints the simulation result
void printStats(cache::stats stats){
	cout << "\nCache parameters:" << endl;
	cout << "\tCache size (KB): " << specs.cacheSize/1024 << endl;
	cout << "\tCache Associativity: " << specs.associativity << endl;
	cout << "\tCache Block Size (bytes): " << specs.lineSize << endl;
	cout << "Simulation result:" << endl;
	cout << "\tOverall miss rate: " << ((float)(stats.loadMisses + stats.storeMisses))/stats.totalEntries << endl;
	cout << "\tRead miss rate: " << ((float)(stats.loadMisses))/(stats.loadHits+stats.loadMisses) << endl;
	cout << "\tDirty evictions: " << stats.evictions << endl;
	cout << "\tLoad misses: " << stats.loadMisses << endl;
	cout << "\tStore misses: " << stats.storeMisses << endl;
	cout << "\tLoad hits: " << stats.loadHits << endl;
	cout << "\tStore hits: " << stats.storeHits << endl;
	cout << "\tTotal hits: " << stats.storeHits + stats.loadHits << endl;
}

// Prints the usage chart
void printUsage(){
	string usage = "\nUsage: gunzip -c <compressedTraceFile.gz> | cache [options]";
	usage += "\n\n\t-a\t\tAssociativity";
	usage += "\n\t-t\t\tCache size in KB";
	usage += "\n\t-l\t\tLine size in bytes";
	usage += "\n\t-h\t\tShow usage\n";
	cout << usage << endl;
	exit(1);
}

int main(int argc, char* argv[]) {
	int i = 1;
	while (i < argc && argv != nullptr) {
		cout << atoi(argv [i+1]) << endl;
    	if (string(argv[i]) == "-a") {
      		specs.associativity = atoi(argv[i+1]);
    	} else if (string(argv[i]) == "-l") {
      		specs.lineSize = atoi(argv [i+1]);
    	} else if (string(argv[i]) == "-t") {
      		specs.cacheSize = atoi(argv [i+1])*1024;
    	} else {
      		printUsage();
    	}
		i+=2;
  	}
	specs.cacheBlocks = specs.cacheSize / specs.lineSize / specs.associativity;
	specs.mRrpvMax = specs.associativity > 2 ? 3 : 1;

	string address;
	char marker;
	int write, iCount;
	cache::memoryRequest request;
	vector<vector<cache::cacheLine>> cache(specs.cacheBlocks);
	cache::initializeCache(&cache, specs.associativity);
	cache::stats stats;
	while (cin >> marker >> write >> address >> iCount){
		request.address = stoul(address, nullptr, 16);
		request.write = write;
		cache::requestCache(&cache, request, &stats, specs);
		stats.totalEntries++;
	}
	
	printStats(stats);
    return 0;
}
