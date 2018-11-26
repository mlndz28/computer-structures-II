#include <iostream>
#include <mpi.h>
#include <vector>
#include "util.hpp"
#include "matrix.hpp"

using namespace std;

vector<int> masterMult(vector<int>* A, vector<vector<int>>* B, int processes);
void slaveMult(vector<int>* A, int matrixSize);

int main ( int argc, char *argv[] ){
	int i = 1;
	int matrixSize = -1;
	while (i < argc && argv != nullptr) {
    	if (string(argv[i]) == "-s") {
      		matrixSize = atoi(argv[i+1]);
    	} else {
      		printUsage();
    	}
		i+=2;
  	}
	if(matrixSize < 0){
		printUsage();
	}
	int id, err, processes;

	vector<int> A(matrixSize); // shared variable, read only after initialized
	matrix::init(&A, matrixSize);

	err = MPI_Init ( &argc, &argv );
	handleMPIError(err, "MPI_Init");
	err = MPI_Comm_size ( MPI_COMM_WORLD, &processes );
	handleMPIError(err, "MPI_Comm_size");
	err = MPI_Comm_rank ( MPI_COMM_WORLD, &id );

	if(processes == 1){ // if either not run with mpirun or the number of processes implied by mpirun is 1
		cout << "\nUsing a single process. Use -h to know how to use more processes." << endl;
		cout << "\nA:\n"  << endl;
		matrix::print(&A);
		vector<vector<int>> B(matrixSize);
		matrix::init(&B, matrixSize);
		cout << "\nB:\n"  << endl;
		matrix::print(&B);
		vector<int> C = matrix::mult(&A, &B); // do a regular multiplication, no parallel processing
		cout << "\nC:\n"  << endl;
		matrix::print(&C);
		cout << endl;
	}else{
		if(!id){
			cout << "\nUsing " << processes << " processes" << endl;
			cout << "\nA:\n"  << endl;
			matrix::print(&A);
			vector<vector<int>> B(matrixSize);
			matrix::init(&B, matrixSize);
			cout << "\nB:\n"  << endl;
			matrix::print(&B);
			vector<int> C = masterMult(&A, &B, processes);	// the master function will assign work to each
			cout << "\nC:\n"  << endl;						// process and calculate the multiplication
			matrix::print(&C);								// in parallel
			cout << endl;

		}else if (id < matrixSize + 1 ){	// extra processes will not keep waiting
			int contract = 1;
			while(contract){
				MPI_Recv(&contract, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				try{
					slaveMult(&A, matrixSize);	// assign a slave function to each slave process
				}catch(invalid_argument& e){
					cerr << e.what() << endl;
					return(-1);
				}
			}
		}
	}
	MPI_Finalize();
}

// Multiplies two vectors assigned by the master process.
void slaveMult(vector<int>* A, int matrixSize){
	vector<int> column(matrixSize);
	MPI_Recv(&column[0], matrixSize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	int product = matrix::dotProduct(A, &column);
	MPI_Send(&product, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
}

// Manages the workload on each slave process, distributes the columns between
// all the processes and arranges the final result.
vector<int> masterMult(vector<int>* A, vector<vector<int>>* B, int processes){
	int matrixSize = (*A).size();
	vector<vector<int>> temp(matrixSize);
	vector<int> C(matrixSize);
	for(int i = 0; i < matrixSize; i++){
		int slave = i%(processes-1) + 1; // first process will only work as the master process
		vector<int> column(matrixSize);			// to simplify the messaging protocol actioning, the vector
		for(int j = 0; j < matrixSize; j++){	// will be multiplied with a column, this way the slave process
			column[j] = (*B)[i][j]; 			// returns an integer instead of an array.
		}
		int contract = matrixSize - i > processes - 1;	// 1 if the contract is still active (not the last round), 0 the other way
		MPI_Send(&contract, 1, MPI_INT, slave, 2, MPI_COMM_WORLD);	// tell the slave process if it has to keep working after this operation
		MPI_Send(&column[0], matrixSize, MPI_INT, slave, 0, MPI_COMM_WORLD);
	}
	for(int i = 0; i < matrixSize; i++){
		int slave = i%(processes-1) + 1;
		MPI_Recv(&C[i], 1, MPI_INT, slave, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	// receive the result from the slave process
	}
	return C;
}
