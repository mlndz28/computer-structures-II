#include <iostream>
#include <vector>
#include "matrix.hpp"

using namespace std;

// Returns the dot product between two integer vectors
int matrix::dotProduct(vector<int>* A, vector<int>* B){
	int result = 0;
	if ((*A).size()!=(*B).size()){
		throw invalid_argument("Vectors must have the same amount of columns");
	}
	for(int i = 0; i < (*A).size(); i++){
		result += (*A)[i]*(*B)[i];
	}
	return result;
}

// Initializes a vector with each item being the position number
void matrix::init(vector<int> *A, int size){
	for(int i = 0; i < size; i++){
		(*A)[i] = i + 1;
	}
}

// Initializes an array with each cell being the sum of the column number and the row number
void matrix::init(vector<vector<int>> *B, int size){
	for(int i1 = 0; i1 < size; i1++){
		vector<int> row(size);
		(*B)[i1] = row;
		for(int i2 = 0; i2 < size; i2++){
			(*B)[i1][i2] = (i1 + 1) + (i2 + 1);
		}
	}
}

// Returns the multiplication result of a vector and a matrix
vector<int> matrix::mult(vector<int>* A, vector<vector<int>>* B){
	if ((*A).size()!=(*B).size()){
		throw invalid_argument("Vector must have the same amount of columns as the matrix have rows");
	}
	int length = (*B)[0].size();
	vector<int> C(length);
	for(int i1 = 0; i1 < length; i1++){
		for(int i2 = 0; i2 < length; i2++){
			C[i1] += (*A)[i2]*(*B)[i2][i1];
		}
	}
	return C;
}

// Prints the content of an integer vector
void matrix::print(vector<int>* A){
	cout << "[ " ;
	for(int i = 0; i < (*A).size(); i++){
		cout << (*A)[i] << ((*A)[i]<10?"  ":" ");
	}
	cout << "]" << endl ;
}

// Prints the content of an integer matrix
void matrix::print(vector<vector<int>>* A){
	for(int i1 = 0; i1 < (*A).size(); i1++){
		cout << ((i1 == 0 && i1 == i1-1 ) ? "[ " : "| ");
		for(int i2 = 0; i2 < (*A)[0].size(); i2++){
			cout << (*A)[i1][i2] << ((*A)[i1][i2]<10?"  ":" ");
		}
		cout << ((i1 == 0 && i1 == i1-1 ) ? "]" : "|") << endl;
	}
}
