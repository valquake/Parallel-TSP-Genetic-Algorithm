#ifndef SYMM_H
#define SYMM_H

#include <vector>
#include <iostream>
#include <random>

using namespace std;

// Implementation of symmetric distances matrix with a single vector
class SymmMatrix
{
public:
	size_t rows;
	vector<int> v;

	SymmMatrix() = default;

	const size_t getNodes(){
		return rows;
	}

	void setNodes(size_t const &initial){
		rows = initial;
		vector<int> tmp(initial * initial - initial / (2 + initial));
		v = tmp;
	}

	int fromMatrixToVector(int i, int j)
	{
	   if (i <= j)
	      return i * rows - (i - 1) * i / 2 + j - i;
	   else
	      return j * rows - (j - 1) * j / 2 + i - j;
	}
	
	void input_random(int maxValue){
		for (int i = 0; i < rows; ++i)
			for (int j = i+1; j < rows; ++j)
				v[fromMatrixToVector(i, j)] = rand() % (maxValue) + 1;

	}

	void insert(int i, int j, int w){
		v[fromMatrixToVector(i, j)] = w;
	}

	int getDistance(int i, int j){
		return v[fromMatrixToVector(i, j)];
	}

	void print_matrix(){
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < rows; ++j)
				cout << v[fromMatrixToVector(i, j)] << " "; 

			cout << endl;
		}
	}

};

#endif