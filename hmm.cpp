#include "hmm.hpp"


/*
Implements backward algorithm from Rabiner.
*/
double DiscreteHmm::BackwardAlgorithm(const vector<int>& observation, const int t)
{
	int i, j, k;
	vector<double> temp;
	double sum;

	if(t < 0 || t >= observation.length()){
		cout << "ERROR invalid t value in BackwardAlgorithm()" << t << endl;
		return 1;
	}

	//Resize and reset matrix to all zeroes
	_betaLattice.Resize(this->NumStates(), observation.size());
	_betaLattice.Reset();
	temp.resize(this->NumStates());

	//init last column of beta matrix to 1.0 (which is 0.0, in logarithm land)
	vector<double>& lastCol = *_betaLattice.end();
	for(i = 0 ; i < lastCol.size(); i++){
		lastCol[i] = 0;
	}

	//Induction
	for(i = observation.length() - 2; i >= 0; i--){
		vector<double>& leftCol = _betaLattice[i];
		vector<double>& rightCol = _betaLattice[i+1];
		//foreach state in left state column
		for(j = 0; j < leftCol.size(); j++){
			b = -10000000; //some very large negative number
			//k iterates the right states, given the current left state
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
	vector<double>& firstCol = _betaLattice.front();
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
@t: the number of observations for which to run the forward algorithm from left to right;
on exit, the t-th column (columns counted from 0) will contain inductively defined values of forward alg.

Returns the sum of calculated values in the t-th column. On exit, _alphaLattice will retain all forward probability
values for this observation as well.
*/
double DiscreteHmm::ForwardAlgorithm(const vector<int>& observations, const int t)
{
	int i, j, k;
	vector<double> temp;
	double sum;

	if(t < 1 || t >= observation.length()){
		cout << "ERROR insufficient t value in ForwardAlgorithm() t=" << t << endl;
		return 1;
	}

	//Resize and reset matrix to all zeroes
	_alphaLattice.Resize(this->NumStates(), observation.size());
	_alphaLattice.Reset();
	temp.resize(this->NumStates());

	//init left-most column of alpha matrix to initial probs, given first observation
	for(i = 0 ; i < this->NumStates(); i++){
		_alphaLattice[0][i] = _pi[i] + _transitionMatrix[i][observations[0]];
	}

	//Induction, from 1 to t
	for(i = 1; i <= t; i++){
		vector<double>& leftCol = _alphaLattice[i-1];
		vector<double>& rightCol = _alphaLattice[i];
		//foreach state in right column
		for(j = 0; j < rightCol.size(); j++){
			b = -10000000; //some very large negative number
			//iterate the previous states, given the current state
			for(k = 0; k < leftCol.size(); k++){
				temp[k] = (_stateMatrix[k][j] + leftCol[k]);
				if(temp[k] > b){
					b = temp[k];
				}
			} //end-for: temp now contains all a_i's for log-sum-exp, and we have b, the max of them
			//now run the log-sum-exp trick
			leftCol[j] = _logSumExp(temp,b);
			//lastly, multiply the observation probability back in, which was factored out of forward calculations
			leftCol[j] += _transitionMatrix[j][ observations[i] ];
		}
	}

	//Termination
	vector<double>& lastCol = _alphaLattice.back();
	for(i = 0, sum = 0; i < lastCol.size(); i++){
		sum += lastCol[i];
	}

	return sum;
}


/*
The Viterbi algorithm is nearly identical to the forward algorithm except for using a max() operation
instead of a sum() operation in the inductive step.

@t: The number of states to evaluate. In almost every case, the user would want to evalaute t = |observations|
to get the complete sequence of most likely hidden states corresponding to the observation sequence.

Returns: Probability of the most likely sequence of hidden states corresponding with the observation
sequence. On exit, @output will contain the id's of these hidden states, and the viterbiLattice will contain
all of the calculated viterbi values (Rabiner uses 'delta' for these).
*/
double DiscreteHmm::Viterbi(const vector<int>& observations, const int t, vector<int>& output)
{
	int i, j, k;
	vector<double> temp;
	double sum;
	pair<int,double> max;

	if(t < 0 || t > observations.size()){
		cout << "ERROR t parameter invalid in Viterbi(): " << t << endl;
		return 1.0;
	}

	//Resize and reset matrix to all zeroes
	_viterbiLattice.Resize(this->NumStates(), observation.size());
	_viterbiLattice.Reset();
	output.resize(observations.size());
	//init the backpointer matrix
	_ptrLattice.Resize(this->NumStates(), observation.size());

	//init left-most column of alpha matrix to initial probs, given first observation
	for(i = 0 ; i < this->NumStates(); i++){
		_viterbiLattice[0][i] = _pi[i] + _transitionMatrix[i][observations[0]];
		_ptrLattice[0][i] = -1; //point all initial pointers at <start> null state
	}

	//Induction, from 1 to t
	for(i = 1; i <= t; i++){
		vector<double>& leftCol = _viterbiLattice[i-1];
		vector<double>& rightCol = _viterbiLattice[i];
		vector<int>& ptrCol = _ptrLattice[i];
		//foreach state in right column
		for(j = 0; j < rightCol.size(); j++){
			max = -10000000; //some large negative number
			//iterate the previous states, given the current state
			for(k = 0; k < leftCol.size(); k++){
				temp = (_stateMatrix[k][j] + leftCol[k]);
				if(temp > max.second){
					max.second = temp;
					max.first = k;
				}
			} //end-for: max contains maximum score and a pointer to its argmax state 
			ptrCol[j]  = max.first;
			leftCol[j] = max.second;
			//lastly, multiply the observation probability back in, which was factored out of forward calculations
			leftCol[j] += _transitionMatrix[j][ observations[i] ];
		}
	}

	//Termination
	//get max in last column
	vector<double>& lastCol = _viterbiLattice[t];
	vector<int>& ptrCol = _ptrLattice[t];
	max.second = -1000000;
	for(i = 0; i < lastCol.size(); i++){
		if(max.second < lastCol[i]){
			max.second = lastCol[i];
			max.first = ptrCol[i];
		}
	}
	//backtrack to get optimal state id sequence
	output.back() = max.first;
	for(i = t - 1; i >= 0; i--){
		output[i] = _ptrLattice[i][ output[i+1] ];
	}

	return max.second;
}

/*

*/
void DiscreteHmm::BaumWelch(const vector<int>& observations)
{

}


/*

*/
void DiscreteHmm::Train(DiscreteHmmDataset& dataset)
{




}
