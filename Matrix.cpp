#include "Matrix.hpp"

template<typename T>
Matrix<T>::Matrix()
{}

template<typename T>
Matrix<T>::~Matrix()
{
	Clear();
}

template<typename T>
//returns a row of the matrix
vector<T>& Matrix<T>::operator[](const int i)
{
	return _matrix[i];
}

template<typename T>
const vector<T>& Matrix<T>::operator[](const int i) const
{
	return _matrix[i];
}

template<typename T>
void Matrix<T>::GetSize(int& rows, int& cols)
{
	rows = _matrix.size();
	if(rows > 0){
		cols = _matrix[0].size();
	}
	else{
		cols = 0;
	}
}

template<typename T>
int Matrix<T>::NumRows()
{
	return _matrix.size();
}

template<typename T>
int Matrix<T>::NumCols()
{
	if(_matrix.size() == 0)
		return 0;
	return _matrix[0].size();
}

template<typename T>
void Matrix<T>::Clear()
{
	if(_matrix.size() > 0){
		for(int i = 0; i < _matrix[0].size(); i++){
			_matrix[i].clear();
		}
		_matrix.clear();
	}
}

template<typename T>
void Matrix<T>::Print()
{
	for(int i = 0; i < _matrix.size(); i++){
		for(int j = 0; j < _matrix[0].size(); j++){
			cout << setprecision(3);
			cout << _matrix[i][j] << " ";
		}
		cout << endl;
	}
}

/*
Given a matrix representing co-event frequency counts,
this normalizes rows, turning them into probabilities.
Working with probabilities in range [0,1] is a very bad idea in floating point land,
so this places them in natural logarithm space instead.
*/
template<typename T>
void Matrix<T>::LnNormalizeRows()
{
	double norm;

	for(int i = 0; i < _matrix.size(); i++){
		norm = 0;
		for(int j = 0; j < _matrix[0].size(); j++){
			norm += _matrix[i][j];
		}
		if(norm <= 0){
			cout << "ERROR norm < 0 (" << norm << ") in Train(), dying by div zero" << endl;
		}
		for(int j = 0; j < _matrix[0].size(); j++){
			if(_matrix[i][j] > 0){
				//This property helps avoid underflowing the (x/y) param to log function: log(x/y) = log(x) - log(y)
				_matrix[i][j] = log(_matrix[i][j]) - log(norm);
			}
			else{
				//Let negative-infinity signal zero probability in log-space
				_matrix[i][j] = -numeric_limits<double>::infinity();
			}
		}
	}
}


//Resets all matrix vals to zero; does not resize.
template<typename T>
void Matrix<T>::Reset()
{
	for(int i = 0; i < _matrix.size(); i++){
		memset((void*)_matrix[i].data(), 0, sizeof(T)*_matrix[i].size());
	}
}

/*
Resizes the matrix' internal storage per the parameters. Note that
this function is cumulative: If matrix is presently larger than needed,
this will not shrink the internal matrix size, only its representation.
*/
template<typename T>
void Matrix<T>::Resize(const int rows, const int cols)
{
	if(_matrix.size() < rows){
		_matrix.resize(rows);
		_matrix.shrink_to_fit();
	}
	if(_matrix[0].size() < cols){
		for(int i = 0; i < _matrix.size(); i++){
			_matrix[i].resize(cols);
			_matrix[i].shrink_to_fit();
		}
	}
}

