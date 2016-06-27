#include "Hmm.hpp"

DiscreteHmm::DiscreteHmm()
{}

/*
Consumes a pre-made model contained in a .hmm file, formatted as:
	#these are from Mark Stamp's HMM intro, for verification
	X=H,C
	Z=S,M,L
	A=0,0.5,0.5;0,0.9,0.1;0,0,1.0
	B=0.5,0.5
	Pi=0,6,0.4
	
*/
DiscreteHmm::DiscreteHmm(const string& modelPath)
{
	ReadModel(modelPath);
}

DiscreteHmm::~DiscreteHmm()
{
	Clear();
}

/*
Reads in a HMM model (lambda) from a file. The order of the X, Y variables
corresponds with the rows/columns of the A and B matrices.

The .hmm file for now is assumed to contain regular probabilities, not log-probs, but this
is very likely to be updated.
*/
bool DiscreteHmm::ReadModel(const string& modelPath)
{
	int i, j;
	bool result = true;
	string line;
	string token, param, argstr;
	fstream modelFile;
	vector<string> args;
	vector<string> row;

	modelFile.open(modelPath, ios::in);
	if(!modelFile.is_open()){
		cout << "ERROR could not open test file: " << modelPath << endl;
	}

	//clear any preceding model and data
	this->Clear();

	while(getline(modelFile,line)){
		if(line.length() > 0 && line[0]!= '#'){
			param = line.substr(0,line.find('='));
			argstr = line.substr(line.find('=')+1, line.length() - line.find('=') - 1);
			cout << "param+args:" << param << ":" << argstr << endl;
			if(param == "X"){
				//get the emission labels
				_split(argstr,',',args);
				for(i = 0; i < args.size(); i++){
					_dataset.AddSymbol(args[i]);
				}
			}
			else if(param == "Z"){
				//get the state labels
				_split(argstr,',',args);
				for(i = 0; i < args.size(); i++){
					_dataset.AddState(args[i]);
				}
			}
			else if(param == "A"){
				//get the A matrix values
				_split(argstr,';',args);
				for(i = 0; i < args.size(); i++){
					_split(args[i],',',row);
					if(_stateMatrix.NumRows() < row.size()){
						_stateMatrix.Resize(args.size(),row.size());
						_stateMatrix.Reset();
					}
					for(j = 0; j < row.size(); j++){
						double a = stod(row[j]);
						if(a >= 0 && a <= 1){ //convert probs to ln-space
							a = log(a);
						}
						_stateMatrix[i][j] = a;
					}
				}
			}
			else if(param == "B"){
				//get the B matrix values
				_split(argstr,';',args);
				for(i = 0; i < args.size(); i++){
					_split(args[i],',',row);
					if(_transitionMatrix.NumRows() < args.size()){
						cout << "Resizing trans dim from " << _transitionMatrix.NumRows() << " to " << row.size() << endl;
						_transitionMatrix.Resize(args.size(),row.size());
						_transitionMatrix.Reset();
					}
					for(j = 0; j < row.size(); j++){
						double b = stod(row[j]);
						if(b >= 0 && b <= 1){ //convert probs to ln-space
							b = log(b);
						}
						_transitionMatrix[i][j] = b;
					}
				}
			}
			else if(param == "Pi"){
				//get the Pi values
				_split(argstr,',',args);
				for(i = 0; i < args.size(); i++){
					double pi = stod(args[i]);
					if(pi >= 0 && pi <= 1){ //convert probs to ln-space
						pi = log(pi);
					}
					_pi.push_back(pi);
				}
			}
		}
	}

	result = this->_validate();
	if(!result){
		cout << "ERROR model may not have imported correctly" << endl;
	}

	PrintModel();

	return result;
}

bool DiscreteHmm::_validate()
{
	bool result = true;

	//some basic validation
	if(_pi.size() != _stateMatrix.NumRows() || _pi.size() != _stateMatrix.NumCols()){
		cout << "ERROR |pi| != |states| " << _pi.size() << " != " << _stateMatrix.NumCols() << endl;
		result = false;
	}
	//verify pi size compared with rows of transition matrix
	if(_pi.size() != _transitionMatrix.NumRows()){
		cout << "ERROR |pi| != |transition rows| " << _pi.size() << " != " << _stateMatrix.NumCols() << endl;
		result = false;
	}
	//make sure state matrix is square
	if(_stateMatrix.NumCols() != _stateMatrix.NumRows()){
		cout << "ERROR state matrix is not square dim: " << _stateMatrix.NumRows() << " x " << _stateMatrix.NumCols() << endl;
		result = false;
	}
	//make sure nothings empty
	if(_pi.size() == 0 || _stateMatrix.NumRows() == 0 || _transitionMatrix.NumRows() == 0){
		cout << "ERROR pi, state matrix, or transition matrix empty" << endl;
		result = false;
	}

	return result;
}

//Builds an output vector of string, just like python split()
void DiscreteHmm::_split(const string& str, const char delim, vector<string>& tokens)
{
	int i, prev;
	string temp;
	vector<int> indices;

	tokens.clear();

	//find all indices of delimiter
	for(i = 0; i < str.length(); i++){
		if(str[i] == delim){
			indices.push_back(i);
		}
	}
	cout << "got " << indices.size() << " indices from " << str << endl;

	for(prev = 0, i = 0; i < indices.size(); i++){
		temp = str.substr(prev,indices[i]-prev);
		tokens.push_back(temp);
		prev = indices[i] + 1;
	}
	//get the last substr
	if(prev < str.length()){
		temp = str.substr(prev, str.length()-prev);
		tokens.push_back(temp);
	}

	cout << "split got " << tokens.size() << " tokens: ";
	for(i = 0; i < tokens.size(); i++){
		cout << tokens[i] << " ";
	}
	cout << endl;
}

/*

*/
void DiscreteHmm::Clear()
{
	_dataset.Clear();
	_alphaLattice.Clear();
	_betaLattice.Clear();
	_viterbiLattice.Clear();
	_ptrLattice.Clear();
	_stateMatrix.Clear();
	_transitionMatrix.Clear();
	_pi.clear();
	_gammaLattice.Clear();
	_xiMatrices.clear();
}

/*
Clears all models and resizes pi, state, and transition matrices.
*/
void DiscreteHmm::_resizeModel(int numStates, int numSymbols)
{
	Clear();
	_pi.resize(numStates);
	_stateMatrix.Resize(numStates, numStates);
	_transitionMatrix.Resize(numStates, numSymbols);
}

/*
Outputs an HMM model (lambda) to a file. The order of the pi, X, and Y variables
corresponds with the rows/columns of A and B matrices.

It is assumed any model in memory is in ln-space.
*/
void DiscreteHmm::WriteModel(const string& path, bool asLogProbs)
{
	int i, j;
	fstream outputFile;

	outputFile.open(path.c_str(),ios::out);
	if(!outputFile.is_open()){
		cout << "ERROR could not open dataset file: " << path << endl;
		return;
	}

	//write the symbols
	outputFile << "X=";
	for(i = 0; i < _dataset.NumSymbols(); i++){
		outputFile << _dataset.GetSymbol(i);
		//cout << "oputptuing " << _dataset.GetSymbol(i) << endl;
		if(i < _dataset.NumSymbols()-1){
			outputFile << ",";
		}
	}
	outputFile << "\n";

	//write the states
	outputFile << "Z=";
	for(i = 0; i < _dataset.NumStates(); i++){
		outputFile << _dataset.GetState(i);
		if(i < _dataset.NumStates()-1){
			outputFile << ",";
		}
	}
	outputFile << "\n";

	//write the A matrix
	outputFile << "A=";
	for(i = 0; i < _stateMatrix.NumRows(); i++){
		for(j = 0; j < _stateMatrix.NumCols(); j++){
			if(!asLogProbs && _stateMatrix[i][j] < 0){
				outputFile << exp(_stateMatrix[i][j]);
			}
			else{
				outputFile << _stateMatrix[i][j];
			}
			if(j < _stateMatrix.NumCols()-1){
				outputFile << ",";
			}
		}
		if(i < _stateMatrix.NumRows()-1){
			outputFile << ";";
		}
	}
	outputFile << "\n";

	//write the B matrix
	outputFile << "B=";
	for(i = 0; i < _transitionMatrix.NumRows(); i++){
		for(j = 0; j < _transitionMatrix.NumCols(); j++){
			if(!asLogProbs && _transitionMatrix[i][j] < 0){
				outputFile << exp(_transitionMatrix[i][j]);
			}
			else{
				outputFile << _transitionMatrix[i][j];
			}
			if(j < _transitionMatrix.NumCols()-1){
				outputFile << ",";
			}
		}
		if(i < _transitionMatrix.NumRows()-1){
			outputFile << ";";
		}
	}
	outputFile << "\n";

	//write the Pi values
	outputFile << "Pi=";
	for(i = 0; i < _pi.size(); i++){
		if(!asLogProbs && _pi[i] < 0){
			outputFile << exp(_pi[i]);
		}
		else{
			outputFile << _pi[i];
		}
		if(i < _pi.size() -1){
			outputFile << ",";
		}
	}
	outputFile << endl;

	outputFile.close();
}

void DiscreteHmm::PrintModel(bool asLogProbs)
{
	int i;
	cout << "\nHMM Models\n" << endl;

	cout << "State transition matrix for states: ";
	for(i = 0; i < _dataset.NumStates(); i++){
		cout << _dataset.GetState(i);
		if(i < _dataset.NumStates() - 1)
			cout << ", ";
	}
	cout << endl;
	_stateMatrix.Print(!asLogProbs);
	cout <<  endl;

	cout << "Emission matrix for symbols: ";
	for(i = 0; i < _dataset.NumSymbols(); i++){
		cout << _dataset.GetSymbol(i);
		if(i < _dataset.NumSymbols() - 1)
			cout << ", ";
	}
	cout << endl;
	_transitionMatrix.Print(!asLogProbs);
	cout <<  endl;

	cout << "Initial distribution (Pi): \n";
	for(i = 0; i < _pi.size(); i++){
		if(_pi[i] < 0 && !asLogProbs)
			cout << exp(_pi[i]);
		else
			cout << _pi[i];

		if(i < _pi.size()-1)
			cout << ", "; 
	}
	cout << endl;

}

/*
Implements backward algorithm from Rabiner.
*/
double DiscreteHmm::BackwardAlgorithm(const vector<int>& observations, const int t)
{
	int i, j, k;
	vector<double> temp;
	double b, pObs;

	if(t < 0 || t >= observations.size()){
		cout << "ERROR invalid t value in BackwardAlgorithm()" << t << endl;
		return 1;
	}

	//Resize and reset matrix to all zeroes
	_betaLattice.Resize(_stateMatrix.NumRows(), observations.size());
	temp.resize(_stateMatrix.NumRows());

	//init last column of beta matrix to 1.0 (which is 0.0, in logarithm land)
	vector<double>& lastCol = _betaLattice.GetColumn(_betaLattice.NumCols()-1);
	for(i = 0; i < lastCol.size(); i++){
		lastCol[i] = 0; //set all to 0 in log-prob space; in linear space, this is probability 1.0
	}

	//Induction
	for(i = observations.size() - 2; i >= 0; i--){
		vector<double>& leftCol = _betaLattice[i];
		vector<double>& rightCol = _betaLattice[i+1];
		//foreach state in left state column
		for(j = 0; j < leftCol.size(); j++){
			b = MIN_DOUBLE; //some very large negative number
			//k iterates the right states, given the current left state
			for(k = 0; k < rightCol.size(); k++){
				temp[k] = (_stateMatrix[j][k] + rightCol[k] + _transitionMatrix[k][observations[i+1]]);
				if(temp[k] > b){
					b = temp[k];
				}
			} //end-for: temp now contains all a_i's for log-sum-exp, and we have b, the max of them

			//now run the log-sum-exp trick
			leftCol[j] = _logSumExp(temp,b); //note unlike forward alg, we don't add observation prob back in, since it can't be factored as it is in forward alg.
		}
	}

	//Termination
	vector<double>& firstCol = _betaLattice.GetColumn(0);
	b = MIN_DOUBLE;
	for(i = 0; i < firstCol.size(); i++){
		if(firstCol[i] > b){
			b = firstCol[i];
		}
	}

	pObs = _logSumExp(firstCol,b);
	cout << "Backward algorithm completed. P(observation): " << pObs << endl;

	return pObs;
}

/*
An implementation of the log-sum-exp trick to avoid underflow in probability sums in linear space.
For an explanation, see https://hips.seas.harvard.edu/blog/2013/01/09/computing-log-sum-exp/

LSE applies in case when we want a sum of exponentials: log(exp(x1) + exp(x2) + ...)
LSE is defined as:
	LSE(X) = x* + log(exp(x1 - x*) + exp(x2 - x*) ...)  where x* is max{x:x subset X}

@vec: The vector X of logarithms to LSE
@b: The max x within X 
*/
double DiscreteHmm::_logSumExp(const vector<double>& vec, double b)
{
	double sum = 0;

	//handle the zero prob exceptions
	if(b == -numeric_limits<double>::infinity()){
		//largest probability is zero (-inf in ln space), so return zero and avert exceptions from exp() function
		return -numeric_limits<double>::infinity();
	}

	//sum the log-probs in linear space, shifted by b (where b is actually some negative number)
	for(int i = 0; i < vec.size(); i++){
		//only sum if the current val is not negative infinity; if it is, just skip it, since -inf is zero in linear space
		if(vec[i] != -numeric_limits<double>::infinity()){
			sum += exp(vec[i] - b); //since b is a negative number, this will be addition.
		}
	}

	//TODO: numerical error checking, output errno if some signal value is returned
	if(sum == 0){
		cout << "ERROR sum == " << sum << " in _logSumExp() function" << endl;
	}

	return b + log(sum);
}


/*
Verifies proper functioning of the log-sum exp function above, which is often poorly documented.
I just generated values from scipy's logsumexp function for verification.

	>>> from scipy import misc
	>>> import math
	>>> X = [math.log(float(x)) for x in range(1,101)]
	>>> misc.logsumexp(X)
	8.5271435222694052
*/
void DiscreteHmm::_testLogSumExp()
{
	double result, expected, maxX, epsilon;
	vector<double> X;

	for(int i = 1; i <= 100; i++){
		X.push_back(log((double)i));
	}
	maxX = log(100.0);

	expected = 	8.5271435222694052;
	result = _logSumExp(X,maxX);

	cout << "LogSumExp result: " << result << endl;
	cout << "LogSumExp expected result: " << expected << endl;
	cout << "Delta: " << fabs(expected - result) << endl;
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
	double b, pObs;

	if(t < 1 || t >= observations.size()){
		cout << "ERROR insufficient t value in ForwardAlgorithm() t=" << t << endl;
		return 1;
	}

	//Resize matrix to all zeroes
	_alphaLattice.Resize(_stateMatrix.NumRows(), observations.size());
	temp.resize(_stateMatrix.NumRows());

	//init left-most column of alpha matrix to initial probs, given first observation
	for(i = 0 ; i < _stateMatrix.NumRows(); i++){
		_alphaLattice[0][i] = _pi[i] + _transitionMatrix[i][observations[0]];
	}

	//Induction, from 1 to t
	for(i = 1; i <= t; i++){
		vector<double>& leftCol = _alphaLattice[i-1];
		vector<double>& rightCol = _alphaLattice[i];
		//foreach state in right column
		for(j = 0; j < rightCol.size(); j++){
			b = MIN_DOUBLE; //some very large negative number
			//iterate the previous states, given the current state
			for(k = 0; k < leftCol.size(); k++){
				temp[k] = (_stateMatrix[k][j] + leftCol[k]); // p(S_k -> S_j) * alpha_k
				if(temp[k] > b){
					b = temp[k];
				}
			} //end-for: temp now contains all a_i's for log-sum-exp, and we have b, the max of them
			//now run the log-sum-exp trick
			rightCol[j] = _logSumExp(temp,b);
			//lastly, multiply the observation probability back in, which was factored out of forward calculations
			rightCol[j] += _transitionMatrix[j][ observations[i] ];
		}
	}

	//Termination
	vector<double>& lastCol = _alphaLattice.GetColumn( _alphaLattice.NumCols()-1 );
	b = MIN_DOUBLE;
	for(i = 0; i < lastCol.size(); i++){
		if(lastCol[i] > b){
			b = lastCol[i];
		}
	}

	pObs = _logSumExp(lastCol,b);
	cout << "Forward algorithm completed. P(observation): " << pObs << endl;

	return pObs;
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
	double temp;
	pair<int,double> max;

	if(t < 0 || t > observations.size()){
		cout << "ERROR t parameter invalid in Viterbi(): " << t << endl;
		return 1.0;
	}

	//Resize and reset matrix to all zeroes
	_viterbiLattice.Resize(_stateMatrix.NumRows(), observations.size());
	_viterbiLattice.Reset();
	//init the backpointer matrix
	_ptrLattice.Resize(_stateMatrix.NumRows(), observations.size());
	_ptrLattice.Reset();

	//init left-most column of alpha matrix to initial probs, given first observation
	for(i = 0 ; i < _stateMatrix.NumRows(); i++){
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
			max.second = MIN_DOUBLE; //some large negative number
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

	//Termination: get max in last column
	vector<double>& lastCol = _viterbiLattice.GetColumn(_viterbiLattice.NumCols()-1);
	max.second = MIN_DOUBLE;
	for(i = 0; i < lastCol.size(); i++){
		if(lastCol[i] > max.second){
			max.second = lastCol[i];
			max.first = i;
		}
	}

	//backtrack to get optimal state id sequence
	output.resize(observations.size());
	output.back() = max.first;
	for(i = output.size()-2; i >= 0; i--){
		output[i] = _ptrLattice[i][ output[i+1] ];
	}

	return max.second;
}

/*
A BaumWelch utility, initializes the pi vector, and the state and emission matrices. How these matrices are initialized
can determine the optimum obtained by the algorithm.

For now, just sets both matrices to uniform distributions of 1/N.
*/
void DiscreteHmm::_initUniformDistribution()
{
	//set the pi vector to a uniform distribution
	for(int i = 0; i < _pi.size(); i++){
		_pi[i] = 1.0 / (double)_pi.size();
	}

	//set the transition matrix to a uniform distribution
	for(int i = 0; i < _transitionMatrix.NumRows(); i++){
		for(int j = 0; j < _transitionMatrix.NumCols(); j++){
			_transitionMatrix[i][j] = 1.0 / (double)_transitionMatrix[i].size();
		}
	}

	//set the state matrix to a uniform distribution
	for(int i = 0; i < _stateMatrix.NumRows(); i++){
		for(int j = 0; j < _stateMatrix.NumCols(); j++){
			_stateMatrix[i][j] = 1.0 / (double)_stateMatrix[i].size();
		}
	}
}


/*
Given an observation sequence, Baum-Welch will maximize the likelihood of the observations
given by the model. The maximization is local, not global, converging to a critical point. But See Rabiner.
Greek letters correspond with Rabiner's notation as well.

As far as implementation, the iterations are basically:
	Let lambda = (A,B,Pi) distributions.

	Init: lambda can be uniform, trained, whatever. The BW algorithm will maximize lambda according
	to the training observations it is passed, thereby modifying it from whatever its initial state.

	BW then uses these steps, in terms of implementation:
		1) determine Xi, gamma from observation sequence, lambda
		2) Set forward variables (alpha and beta models)
		3) use Xi, gamma to determine lambda'
		4) repeat from (1) until convergence

See wikipedia for a decent example. There are many others elsewhere.

@dataset: The unlabelled dataset from which to learn
@numHiddenStates: The number of hidden states to initialize the hmm with

*/
double DiscreteHmm::BaumWelch(DiscreteHmmDataset& dataset, const int numHiddenStates)
{
	int i, maxIterations;
	double obsProb, delta, lastProb;
	const double convergence = 1.0;
	vector<int>& observations = dataset.UnlabeledDataSequence;
	string dummy;

	cout << "TODO: need to handle cases when observations vector includes observations not previously seen" << endl;
	cout << "in which case the model won't have the correct number of states, etc. This needs to be errr-checked" << endl;
	cout << "elsewhere, I just don't want to pollute the code with error checks until the methods are stable" << endl;
	cout << "TODO: define the temporary vars (xi/gamma matrices) recursively to cut down space consumption." << endl;

	//init
	Clear();
	//resize all the required models
	_resizeModel(dataset.NumSymbols(), numHiddenStates); //resize the pi, state, and transition matrices to fit this data
	_xiMatrices.resize(dataset.UnlabeledDataSequence.size());
	for(int i = 0; i < _xiMatrices.size(); i++){
		//resize every matrix in the xi matrices to be a square matrix by the number of hidden states
		_xiMatrices[i].Resize(_stateMatrix.NumRows(), _stateMatrix.NumCols()); 
	}
	//gamma matrix is sized NumStates x NumObservations
	_gammaLattice.Resize(_stateMatrix.NumRows(), observations.size()); 
	//set the hmm-model to uniform initial values
	_initUniformDistribution();

	//until convergence, keep retraining the Xi Model, then using its values to maximize the likelihood of the data
	i = 0; maxIterations = 100; obsProb = 0;
	//initialize the forward and backward values
	obsProb = ForwardAlgorithm(observations,observations.size()-1);
	BackwardAlgorithm(observations,observations.size()-1);
	while(true){
		//expectation step: get the Xi and gamma values (see Rabiner)
		_retrainXiModel(observations);
		//maximization step: based on the Xi and gamma values, reset the state and emission probabilities
		_updateModels(observations);
		lastProb = obsProb;
		//re-run Forward and Backward algorithms to reset alpha/beta matrices; this is necessary RIGHT???!?!
		obsProb = ForwardAlgorithm(observations,observations.size()-1);
		BackwardAlgorithm(observations,observations.size()-1);
		delta = lastProb - obsProb;
		i++;

		PrintModel();
		cout << i << "\tp(obs): " << obsProb << "\tdelta: " << delta << endl;
		cout << "Enter anything to cintunue: " << flush;
		cin >> dummy;
	}
	cout << "BaumWelch completed" << endl;
	PrintModel();

	return obsProb;
}

/*
This updates the state and emission probabilities based on the expectation given by the updated
Xi/gamma models. This assumes the Xi/gamma models were just updated in the expectation step;
this step is the corresponding maximization step.
*/
void DiscreteHmm::_updateModels(const vector<int>& observations)
{
	int t, i, j;
	double maxXi, maxGamma, maxObsGamma;
	vector<double> xiVals, gammaVals, gammaObsVals;

	//init the temp storage vectors for summing log probabilities
	xiVals.resize(observations.size());
	gammaVals.resize(observations.size());

	//cout << "_updateModels()" << endl;
 
	//update the Pi values: these are just the first column of the gamma values
	for(i = 0; i < _gammaLattice.NumRows(); i++){
		_pi[i] = _gammaLattice[i][0];
	}

	//update the A (state transition prob) values
	for(i = 0; i < _stateMatrix.NumRows(); i++){
		for(j = 0; j < _stateMatrix.NumCols(); j++){
			maxXi = MIN_DOUBLE;
			maxGamma = MIN_DOUBLE;
			for(t = 0; t < observations.size(); t++){
				xiVals[t] = _xiMatrices[t][i][j];
				if(xiVals[t] > maxXi){
					maxXi = xiVals[t];
				}
				gammaVals[t] = _gammaLattice[t][i];
				if(gammaVals[t] > maxGamma){
					maxGamma = gammaVals[t];
				}
			}
			//use the log-sum-exp trick to get the sum of these probs, then divide them
			_stateMatrix[i][j] = _logSumExp(xiVals,maxXi) - _logSumExp(gammaVals,maxGamma);
		}
	}

	//update the emission probabilities
	for(i = 0; i < _transitionMatrix.NumRows(); i++){ // rows=hidden states
		for(j = 0; j < _transitionMatrix.NumCols(); j++){ // cols=symbols
			gammaObsVals.clear();
			maxObsGamma = MIN_DOUBLE;
			maxGamma = MIN_DOUBLE;
			for(t = 0; t < observations.size(); t++){
				//only add the gamma value of this time step if the observed matches the kth emission symbol
				if(observations[t] == j){
					gammaObsVals.push_back(_gammaLattice[t][i]);
					if(_gammaLattice[t][i] > maxObsGamma){
						maxObsGamma = _gammaLattice[t][i];
					}
				}
				//running total (denominator)
				gammaVals[t] = _gammaLattice[t][i];
				if(gammaVals[t] > maxGamma){
					maxGamma = gammaVals[t];
				}
			}

			//divide the values calculated from above
			if(gammaObsVals.size() > 0){
				_transitionMatrix[i][j] = _logSumExp(gammaObsVals,maxObsGamma) - _logSumExp(gammaVals, maxGamma);
			}
			else{
				_transitionMatrix[i][j] = MIN_DOUBLE; //no symbol matches, so set log probability to negative infinity
			}
		}
	}
}

/*
Updates the Xi and gamma models given an observation sequence.
*/
void DiscreteHmm::_retrainXiModel(const vector<int>& observations)
{
	int t, i, j;
	double obsProb, b;
	vector<double> normVec;

	//TODO init sizes of XiMatrix per observations size
	normVec.resize(_stateMatrix.NumRows() * _stateMatrix.NumCols());

	_alphaLattice.NumRows();

	for(t = 0; t < observations.size() - 1; t++){
		Matrix<double>& xiMatrix = _xiMatrices[t];
		//cout << "top1... " << t << "/" << _xiMatrices.size() << "/" << observations.size() << endl;
		//set the Xi matrix values
		b = MIN_DOUBLE;
		for(i = 0; i < _stateMatrix.NumRows(); i++){
			for(j = 0; j < _stateMatrix.NumRows(); j++){
				xiMatrix[i][j] = _alphaLattice[t][i] + _stateMatrix[i][j] + _transitionMatrix[j][ observations[t+1] ] + _betaLattice[t+1][j];
				//sum log-probs
				normVec[i * _stateMatrix.NumRows() + j] = xiMatrix[i][j];
				if(xiMatrix[i][j] > b){
					b = xiMatrix[i][j];
				}
			}
		}

		//get the total probability of the observation per the log-sum-exp trick
		obsProb = _logSumExp(normVec, b);
		//normalize all the probs for this state after initializing them
		for(i = 0; i < _stateMatrix.NumRows(); i++){
			for(j = 0; j < _stateMatrix.NumCols(); j++){
				xiMatrix[i][j] -= obsProb;
			}
		}

		//set the gamma values for each state (See Rabiner; for a given state i, its gamma value is the sum over all states j)
		for(i = 0; i < xiMatrix.NumRows(); i++){
			//cout << "summing.." << endl;
			for(j = 0, _gammaLattice[t][i] = 0; j < xiMatrix.NumCols(); j++){
				_gammaLattice[t][i] += xiMatrix[i][j];
			}
		}
	}
}

/*
Normal initialization training: read a bunch of data containing both labelled emissions
and labelled hidden/latent states, and build the A and B matrices (as they're called in Rabiner's tutorial)

TODO: State and Transition matrices could be very large and sparse for some datasets; this could be resolved
by implementing a Matrix class capable of handling such sparseness.

TODO: Smoothing for unobserved transitions. Rather than setting them to zero, use some smoothing method (there
are many).
*/
void DiscreteHmm::DirectTrain(DiscreteHmmDataset& dataset)
{
	//TODO: numerical storage could be wrapped in compiler/maXine specific ifdefs, but I don't want to for now
	//For every maXine/compiler that's worth more than two cents, this should evaluate to true.
	if(!numeric_limits<double>::has_infinity || !std::numeric_limits<double>::is_iec559){ //IEEE 754
		cout << "ERROR double does not have infinity, recompile with some other signal value for zero probabilities in log space" << endl;
		cout << "Consider updating your computer to electric power." << endl;
	}

	cout << "Training directly on " << dataset.LabeledDataSequence.size() << " labelled examples..." << endl;

	//init the A matrix
	_stateMatrix.Resize(dataset.NumStates(), dataset.NumStates());
	_stateMatrix.Reset();
	//init the B matrix (|rows| == |states|, |cols| == |symbols|)
	_transitionMatrix.Resize(dataset.NumStates(), dataset.NumSymbols());
	_transitionMatrix.Reset();

	//count all the frequencies
	for(int i = 1; i < dataset.LabeledDataSequence.size(); i++){
		_stateMatrix[ dataset.LabeledDataSequence[i-1].first ][dataset.LabeledDataSequence[i].first]++;
		_transitionMatrix[ dataset.LabeledDataSequence[i].first ][ dataset.LabeledDataSequence[i].second ]++;
		//if(i % 100 == 99)
		//	cout << i << endl;
	}

	//normalize all state frequencies (making them probabilities in ln space)
	_stateMatrix.LnNormalizeRows();

	//normalize all emission frequencies (making them probabilities in ln space)
	_transitionMatrix.LnNormalizeRows();

	cout << "HMM training completed." << endl;
	this->PrintModel();
}

