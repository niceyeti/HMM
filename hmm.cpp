

/*
Implements
As much as possible, I tried to make this mirror Rabin's hmm tutorial.

*/
float DiscreteHmm::GetLogProbability(const vector<int>& observation)
{
	int i, t;

	//resize the alpha matrix as needed
	_alphaMatrix.resize(observation.size(), this->N);

	//run the forward algorithm over entire length of observation
	ForwardAlgorithm(observation, observation.length());
}

/*
Implements backward algorithm from Rabiner.
*/
double DiscreteHmm::BackwardAlgorithm(const vector<int>& observation, const int t)
{
	int i, j, k;
	vector<double> temp;
	double sum;

	if(t <= 1){
		cout << "ERROR insufficient t value in ForwardAlgorithm()" << endl;
		return;
	}

	//Resize and reset matrix to all zeroes
	_betaMatrix.Resize(this->NumStates(), observation.size());
	_betaMatrix.Reset();
	temp.resize(this->NumStates());

	//init last column of beta matrix to 1.0 (0 in logarithm land)
	vector<double>& lastCol = *_betaMatrix.end();
	for(i = 0 ; i < lastCol.size(); i++){
		lastCol[i] = 0;
	}

	//Induction
	for(i = observation.length() - 2; i > 0; i--){

		vector<double>& leftCol = _betaMatrix[i];
		vector<double>& rightCol = _betaMatrix[i+1];
		//foreach state in left state column
		for(j = 0; j < leftCol.size(); j++){
			//k iterates the right states, given the current left state
			b = -10000000; //some very large negative number
			for(k = 0; k < rightCol.size(); k++){
				temp[k] = (_stateMatrix[j][k] + rightCol[k] + _transitionMatrix[k][observations[i+1]]);
				if(temp[k] > b){
					b = temp[k];
				}
			} //end-for: temp now contains all a_i's for log-sum-exp, and we have b, the max of them
			//now run the log-sum-exp trick and store final result
			leftCol[j] = _logSumExp(temp,b); //note we don't ad observation prob back in, since it can't be factored as it is in forward alg.
		}
	}

	//Termination
	vector<double>& firstCol = _betaMatrix.front();
	for(i = 0, sum = 0; i < firstCol.size(); i++){
		sum += firstCol[i]; 
	}

	return sum;
}

/*
An implementation of the log-sum-exp trick to avoid underflow in probability sums in linear space.
For an explanation, see https://hips.seas.harvard.edu/blog/2013/01/09/computing-log-sum-exp/
*/
double DiscreteHmm::_logSumExp(const vector<double>& vec, double b)
{
	double sum = 0;

	//sum the log-probs in linear space, shifted by b (where b is actually some negative number)
	for(int i = 0; i < vec.size(); i++){
		sum += exp(vec[i] - b); //since b is a negative number, this will be addition.
	}

	//TODO: numerical error checking, output errno if some signal value is returned

	return b + log(sum);
}


/*
Implements forward algorithm from Rabiner.
@observations: a sequence of observations, whose discrete class is designated by integers
@t: the number of observations for which to run the forward algorithm from left to right

*/
double DiscreteHmm::ForwardAlgorithm(const vector<int>& observations, const int t)
{
	int i, j, k;
	vector<double> temp;
	double sum;

	if(t <= 1){
		cout << "ERROR insufficient t value in ForwardAlgorithm()" << endl;
		return;
	}

	//Resize and reset matrix to all zeroes
	_alphaMatrix.Resize(this->NumStates(), observation.size());
	_alphaMatrix.Reset();
	temp.resize(this->NumStates());

	//init left-most column of alpha matrix to initial probs, given first observation
	for(i = 0 ; i < this->NumStates(); i++){
		_alphaMatrix[0][i] = _pi[i] + _transitionMatrix[i][observations[0]];
	}

	//Induction
	for(i = 1; i < observation.length() - 1; i++){
		vector<double>& leftCol = _alphaMatrix[i-1];
		vector<double>& rightCol = _alphaMatrix[i];
		//foreach state in current state column
		for(j = 0; j < rightCol.size(); j++){
			//k iterates the previous states, given the current state
			b = -10000000; //some very large negative number
			for(k = 0; k < leftCol.size(); k++){
				temp[k] = (_stateMatrix[k][j] + leftCol[k]);
				if(temp[k] > b){
					b = temp[k];
				}
			} //end-for: temp now contains all a_i's for log-sum-exp, and we have b, the max of them
			//now run the log-sum-exp trick
			leftCol[j] = _logSumExp(temp,b);
			//lastly, multiply the observation probability back in, which was factored out
			leftCol[j] += _transitionMatrix[j][ observations[i] ];
		}
	}

	//Termination
	vector<double>& lastCol = _alphaMatrix.back();
	for(i = 0, sum = 0; i < lastCol.size(); i++){
		sum += lastCol[i]; 
	}

	return sum;
}


void DiscreteHmm::Viterbi()
{

}

/*

*/
void DiscreteHmm::Train(DiscreteHmmDataset& dataset)
{
	






}

