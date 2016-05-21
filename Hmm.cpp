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
	ParseModelFile(modelPath);
}

DiscreteHmm::~DiscreteHmm()
{
	Clear();
}

/*
Reads in a HMM model (lambda) from a file. The order of the X, Y variables
corresponds with the rows/columns of the A and B matrices.
*/
bool DiscreteHmm::ParseModelFile(const string& modelPath)
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
					_stateMatrix.Resize(args.size(),row.size());
					_stateMatrix.Reset();
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
					_transitionMatrix.Resize(args.size(),row.size());
					_transitionMatrix.Reset();
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

//Returns vector of string just like python split()
void DiscreteHmm::_split(const string& str, const char delim, vector<string>& tokens)
{
	int i, j, prev;
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
}

/*
Outputs an HMM model (lambda) to a file. The order of the pi, X, and Y variables
corresponds with the rows/columns of A and B matrices.

It is assumed any model in memory is in ln-space.
*/
void DiscreteHmm::WriteModel(const string& path, bool asLogProbs)
{
	fstream outputFile;

	outputFile.open(path.c_str(),ios::out);
	if(!outputFile.is_open()){
		cout << "ERROR could not open dataset file: " << path << endl;
		return;
	}

	//write the symbols
	outputFile << "X=";
	for(int i = 0; i < _dataset.NumSymbols(); i++){
		outputFile << _dataset.GetSymbol(i);
		//cout << "oputptuing " << _dataset.GetSymbol(i) << endl;
		if(i < _dataset.NumSymbols()-1){
			outputFile << ",";
		}
	}
	outputFile << "\n";

	//write the states
	outputFile << "Z=";
	for(int i = 0; i < _dataset.NumStates(); i++){
		outputFile << _dataset.GetState(i);
		if(i < _dataset.NumStates()-1){
			outputFile << ",";
		}
	}
	outputFile << "\n";

	//write the A matrix
	outputFile << "A=";
	for(int i = 0; i < _stateMatrix.NumRows(); i++){
		for(int j = 0; j < _stateMatrix.NumCols(); j++){
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
	for(int i = 0; i < _transitionMatrix.NumRows(); i++){
		for(int j = 0; j < _transitionMatrix.NumCols(); j++){
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
	for(int i = 0; i < _pi.size(); i++){
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

void DiscreteHmm::PrintModel()
{
	cout << "\nHMM Models\n" << endl;

	cout << "State transition matrix for states: ";
	for(int i = 0; i < _dataset.NumStates(); i++){
		cout << _dataset.GetState(i) << ", ";
	}
	cout << endl;
	_stateMatrix.Print();
	cout <<  endl;

	cout << "Emission matrix for symbols: ";
	for(int i = 0; i < _dataset.NumSymbols(); i++){
		cout << _dataset.GetSymbol(i) << ", ";
	}
	cout << endl;
	_transitionMatrix.Print();
	cout <<  endl;

	cout << "Initial distribution (Pi): \n";
	for(int i = 0; i < _pi.size(); i++){
		cout << _pi[i] << ", ";
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
	double sum, b;

	if(t < 0 || t >= observations.size()){
		cout << "ERROR invalid t value in BackwardAlgorithm()" << t << endl;
		return 1;
	}

	//Resize and reset matrix to all zeroes
	_betaLattice.Resize(_dataset.NumStates(), observations.size());
	_betaLattice.Reset();
	temp.resize(_dataset.NumStates());

	//init last column of beta matrix to 1.0 (which is 0.0, in logarithm land)
	vector<double>& lastCol = _betaLattice.GetColumn(_betaLattice.NumCols()-1);
	for(i = 0 ; i < lastCol.size(); i++){
		lastCol[i] = 0;
	}

	//Induction
	for(i = observations.size() - 2; i >= 0; i--){
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
	vector<double>& firstCol = _betaLattice.GetColumn(0);
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

	//handle the zero prob exceptions
	if(b == -numeric_limits<double>::infinity()){
		//largest probability is zero (-inf in ln space), so return zero and avert exceptions from exp() function
		return -numeric_limits<double>::infinity();
	}

	cout << "LogSumExp: DOES VEC INCLUDE B, OR IS B FULLY FACTORED?" << endl;

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
	double sum, b;

	if(t < 1 || t >= observations.size()){
		cout << "ERROR insufficient t value in ForwardAlgorithm() t=" << t << endl;
		return 1;
	}

	//Resize and reset matrix to all zeroes
	_alphaLattice.Resize(_dataset.NumStates(), observations.size());
	_alphaLattice.Reset();
	temp.resize(_dataset.NumStates());

	//init left-most column of alpha matrix to initial probs, given first observation
	for(i = 0 ; i < _dataset.NumStates(); i++){
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
	vector<double>& lastCol = _alphaLattice.GetColumn( _alphaLattice.NumCols()-1 );
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
	double temp;
	pair<int,double> max;

	if(t < 0 || t > observations.size()){
		cout << "ERROR t parameter invalid in Viterbi(): " << t << endl;
		return 1.0;
	}

	//Resize and reset matrix to all zeroes
	_viterbiLattice.Resize(_dataset.NumStates(), observations.size());
	_viterbiLattice.Reset();
	output.resize(observations.size());
	//init the backpointer matrix
	_ptrLattice.Resize(_dataset.NumStates(), observations.size());

	//init left-most column of alpha matrix to initial probs, given first observation
	for(i = 0 ; i < _dataset.NumStates(); i++){
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
			max.second = -10000000; //some large negative number
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
Given an observation sequence, Baum-Welch will maximize the likelihood of the observations
given by the model. The maximization is local, not global, converging to a critical point. But See Rabiner.
Greek letters correspond with Rabiner's notation as well.

As far as implementation, the iterations are basically:
	Let lambda = (A,B,Pi) distributions.

	Init: lambda can be uniform, trained, whatever. The BW algorithm will maximize lambda according
	to the training observations it is passed, thereby modifying it from whatever its initial state.

	BW then uses these steps, in terms of implementation:
		1) determine chi, gamma from observation sequence, lambda
		2) Set forward variables (alpha and beta models)
		3) use chi, gamma to determine lambda'
		4) repeat from (1) until convergence

*/
void DiscreteHmm::BaumWelch(const vector<int>& observations)
{







}

/*
Updates the chi and gamma models given an observation sequence.

@obsLogProb: The probability of the observation sequence, given previous lambda.
*/
void DiscreteHmm::_bw_UpdateChiMatrix(const vector<int>& observations)
{
	int t, i, j;
	double obsProb, b;
	vector<double> temp;

	//TODO init sizes of chiMatrix per observations size

	for(t = 1; t < _chiMatrix.size(); t++){
		vector<double>& chiCol = _chiLattice[t-1];
		vector<double>& rightCol = _chiLattice[t];
		vector<double>& alphaCol = _alphaLattice[t-1];
		vector<double>& betaCol = _betaLattice[t];

		obsProb = 0.0;
		for(i = 0; i < ; i++){
			for(j = 0; j < ; j++){
				chiCol[i] = alphaCol[i] + _stateMatrix[i][j] + _transitionMatrix[i][j] + betaCol[j];
				obsProb += chiCol[i];
			}
		}
		//normalize the probs
		for(i = 0; i < ; i++){
			for(j = 0; j < ; j++){
				chiCol[i] -= obsProb;
			}
		}
	}

	//update the gamma matrix with the new chi model values
	for(t = 0; t < _gammaMatrix.size(); t++){
		for(i = 0; i < _chiMatrix.NumRows(); i++){
			_gammaMatrix[t][i] = 0;
			b = -NEG_INF;
			for(j = 0; j < _stateMatrix; j++){
				if()
			}
		}
	}


}


/*
Normal initialization training: read a bunch of data containing both labelled emissions
and labelled hidden/latent states, and build the A and B matrices (as they're called in Rabiner's tutorial)

TODO: State and Transition matrices could be very large and sparse for some datasets; this could be resolved
by implementing a Matrix class capable of handling such sparseness.
*/
void DiscreteHmm::Train(DiscreteHmmDataset& dataset)
{
	int i, j, nrows, ncols;
	double norm;

	//TODO: numerical storage could be wrapped in compiler/machine specific ifdefs, but I don't want to for now
	//For every machine/compiler that's worth more than two cents, this should evaluate to true.
	if(!numeric_limits<double>::has_infinity || !std::numeric_limits<double>::is_iec559){ //IEEE 754
		cout << "ERROR double does not have infinity, recompile with some other signal value for zero probabilities in log space" << endl;
		cout << "Consider updating your computer from steam to electric power." << endl;
	}

	//init the A matrix
	_stateMatrix.Resize(dataset.NumStates(), dataset.NumStates());
	_stateMatrix.Reset();
	//init the B matrix (|rows| == |states|, |cols| == |symbols|)
	_transitionMatrix.Resize(dataset.NumStates(), dataset.NumSymbols());
	_transitionMatrix.Reset();

	//count all the frequencies
	for(i = 1; i < dataset.TrainingSequence.size(); i++){
		_stateMatrix[ dataset.TrainingSequence[i-1].first ][dataset.TrainingSequence[i].first]++;
		_transitionMatrix[ dataset.TrainingSequence[i].first ][ dataset.TrainingSequence[i].second ]++;
	}

	//normalize all state frequencies (making them probabilities in ln space)
	_stateMatrix.LnNormalizeRows();

	//normalize all emission frequencies (making them probabilities in ln space)
	_transitionMatrix.LnNormalizeRows();
}

