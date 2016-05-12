Matrix::Matrix()
{}

Matrix::~Matrix()
{
	Clear();
}

//returns a row of the matrix
vector<T>& Matrix::operator[](const int i)
{
	return _matrix[i];
}
const vector<T>& Matrix::operator[](const int i) const
{
	return _matrix[i];
}

void Matrix::GetSize(int& rows, int& cols)
{
	rows = _matrix.size();
	if(rows > 0){
		cols = _matrix[0].size();
	}
	else{
		cols = 0;
	}
}

int Matrix::NumRows()
{
	return _matrix.size();
}

int Matrix::NumCols()
{
	if(_matrix.size() == 0)
		return 0;
	return _matrix[0].size();
}

void Matrix::Clear()
{
	if(_matrix.size() > 0){
		for(int i = 0; i < _matrix[0].size(); i++){
			_matrix[i].clear();
		}
		_matrix.clear();
	}
}

void Matrix::Print()
{
	for(int i = 0; i < _matrix.size(); i++){
		for(int j = 0; j < _matrix[0].size(); j++){
			cout << setprecision(3);
			cout << _matrix[i][j] << " ";
		}
		cout << endl;
	}
}

//Resets all matrix vals to zero; does not resize.
void Matrix::Reset()
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
void Matrix::Resize(const int rows, const int cols)
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

