#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

namespace matrix{
	int dotProduct(std::vector<int>* A, std::vector<int>* B);
	void init(std::vector<int> *A, int size);
	void init(std::vector<std::vector<int>> *B, int size);
	std::vector<int> mult(std::vector<int>* A, std::vector<std::vector<int>>* B);
	void print(std::vector<int>* A);
	void print(std::vector<std::vector<int>>* A);
}

#endif
