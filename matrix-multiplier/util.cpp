#include <iostream>
#include "util.hpp"

using namespace std;

// Handle possible error from MPI
void handleMPIError(int err, string section){
	if ( err != 0 ) {
		cerr << "MPI error: " << section << " failed.\n";
		exit(-1);
	}
}
// Prints the usage chart
void printUsage(){
	string usage = "\nUsage: mpirun [mpi options] mul [options]";
	usage += "\n\nMPI flags:";
	usage += "\n\t-n\t\tNumber of running processes";
	usage += "\n\nFlags:";
	usage += "\n\t-s\t\tMatrix and vector size";
	usage += "\n\t-h\t\tShow usage\n";
	cout << usage << endl;
	exit(1);
}
