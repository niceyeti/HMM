#include "Matrix.hpp"

ColumnMatrix::ColumnMatrix()
{}

ColumnMatrix::~ColumnMatrix()
{
	Clear();
}

//Note these operators are highly unconventional, returning a column of a matrix before the row,
//as in the notation [col][row].
vector<T>& ColumnMatrix::operator[](const int i)
{
	return _matrix[i];
}
const vector<T>& ColumnMatrix::operator[](const int i) const
{
	return _matrix[i];
}

void ColumnMatrix::GetSize(int& rows, int& cols)
{
	cols = _matrix.size();
	if(cols > 0){
		rows = _matrix[0].size();
	}
	else{
		rows = 0;
	}
}

void ColumnMatrix::Clear()
{
	if(_matrix.size() > 0){
		for(int i = 0; i < _matrix[0].size(); i++){
			_matrix[i].clear();
		}
		_matrix.clear();
	}
}

vector<T>& ColumnMatrix::GetColumn(const int i)
{
	return _matrix[i];
}

const vector<T>& ColumnMatrix::GetColumn(const int i)
{
	return _matrix[i];
}

//Resets all matrix vals to zero; does not resize.
void ColumnMatrix::Reset()
{
	for(int i = 0; i < _matrix.size(); i++){
		memset((void*)_matrix[i].data(), 0, _matrix[i].size());
	}
}

/*
Resizes the matrix' internal storage per the parameters. Note that
this function is cumulative: If matrix is presently larger than needed,
this will not shrink the internal matrix size, only its representation.
*/
void ColumnMatrix::Resize(const int rows, const int cols)
{
	if(_matrix.size() < cols){
		_matrix.resize(cols);
		_matrix.shrink_to_fit();
	}
	if(_matrix[0].size() < rows){
		for(int i = 0; i < _matrix.size(); i++){
			_matrix[i].resize(rows);
			_matrix[i].shrink_to_fit();
		}
	}
}
