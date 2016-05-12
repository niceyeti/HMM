#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <iostream>
#include <iomanip>

/*
A lite Matrix class. This obeys normal matrix index conventions, indexing by [row][col].
The column matrix class below is nicer for building lattices for sideways traversal.
*/
class Matrix<T>{
	public:
		Matrix();
		~Matrix();
		void Resize(int rows, int cols);
		void Clear();
		void Reset();
		void GetSize(int& rows, int& cols);
		int NumRows();
		int NumCols();
		vector<T>& operator[](const int i);
		const vector<T>& operator[](const int i) const;
	private:
		vector<vector<T> > _matrix;
}


/*
A lite column-matrix template, amenable to algorithms using lattices. Note no thought
has been given to error checking or optimization.

This matrix is specifically for HMM and similar models/algorithms,
for which iterating columns makes more sense than rows. The semantics of [row][col]
are the same, but the internal storage is a vector of column vectors. This allows
functions to pass-in columns of the matrix.

TODO: Could ColumnMatrix be a wrapper for a regular matrix class, just cleverly transposing indices arguments?
*/
class ColumnMatrix<T>{
	public:
		ColumnMatrix();
		~ColumnMatrix();
		void Resize(const int rows, const int cols);
		void Clear();
		void Reset();
		int NumRows();
		int NumCols();
		void GetSize(int& rows, int& cols);
		inline vector<T>& GetColumn(const int i);
		inline const vector<T>& GetColumn(const int i) const;
		vector<T>& Matrix::operator[](const int i);
		const vector<T>& Matrix::operator[](const int i) const;
	private:
		//Note semantics above: each vector is a column of the matrix, and the matrix is built up from a vector of column-vectors
		vector<vector<T> > _matrix;
}

#endif
