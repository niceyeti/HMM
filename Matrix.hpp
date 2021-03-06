#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <limits>

using namespace std;

/*
A lite Matrix class. This obeys normal matrix index conventions, indexing by [row][col].
The column matrix class below is nicer for building lattices for sideways traversal.
*/
template<typename T>
class Matrix{
	public:
		Matrix();
		~Matrix();
		void Resize(int rows, int cols);
		void Clear();
		void Reset();
		void GetSize(int& rows, int& cols);
		int NumRows();
		int NumCols();
		void Print(bool convertLogProbs=false);
		void LnNormalizeRows();
		//[] gets a row from the matrix (a std::vector)
		vector<T>& operator[](const int i);
		const vector<T>& operator[](const int i) const;
	private:
		vector<vector<T> > _matrix;
};

//#include "Matrix.cpp"

/*
A lite column-matrix template, amenable to algorithms using lattices. Note no thought
has been given to error checking or optimization.

This matrix is specifically for HMM and similar models/algorithms,
for which iterating columns makes more sense than rows. The semantics of [row][col]
are the same, but the internal storage is a vector of column vectors. This allows
functions to pass-in columns of the matrix.

TODO: Could ColumnMatrix be a wrapper for a regular matrix class, just cleverly transposing indices arguments?
*/
template<typename T>
class ColumnMatrix{
	public:
		ColumnMatrix();
		~ColumnMatrix();
		void Resize(const int rows, const int cols);
		void Clear();
		void Reset();
		int NumRows();
		int NumCols();
		void Print();
		void GetSize(int& rows, int& cols);
		inline vector<T>& GetColumn(const int i);
		inline const vector<T>& GetColumn(const int i) const;
		vector<T>& operator[](const int i);
		const vector<T>& operator[](const int i) const;
	private:
		//Note semantics above: each vector is a column of the matrix, and the matrix is built up from a vector of column-vectors
		vector<vector<T> > _matrix;
};

//#include "ColumnMatrix.cpp"

#endif
