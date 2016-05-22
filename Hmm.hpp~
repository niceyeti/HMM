#ifndef HMM_HPP
#define HMM_HPP

#include "Matrix.hpp"
//include the template code; this is how c++ hacks three-file format for template classes
#include "Matrix.cpp"
#include "ColumnMatrix.cpp"
#include "DiscreteHmmDataset.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>


using namespace std;

/*
A simple, discrete hidden markov model implementation.

This implementation requires iec_559 (IEEE 754) compliance.
All internal probabilities are stored in natural-log space, for higher precision, and thus any probability
returned from this class will be the natural log of a probability in linear space. Note that many
state or emission transitions may be zero, which is undefined in log-space. Accordingly, since
the range of any probability in natural log space is negative numbers in the range (-infinity,0],
a linear-space probability of zero will be represented by -1 * std::numeric_limits<float>::infinity().
This is system-dependent, so pay attention to it. A few assumptions are thus required:

	-INF + a = -INF for all a  (where a is any non-special double/float)
	-INF + -INF = -INF
	-INF < a = TRUE for all a
	-INF > a = FALSE for all a

These are required by IEEE 754. So note that -INF returned by any of this class' functions just means zero probability.

TODO: For large datasets, could implement another sparse matrix class for very large, sparse state/transition matrices.
*/
class DiscreteHmm{
	public:
		DiscreteHmm();
		DiscreteHmm(const string& modelPath);
		~DiscreteHmm();
		void Train(DiscreteHmmDataset& dataset);
		void BaumWelch(const vector<int>& observations);
		double Viterbi(const vector<int>& observations, const int t, vector<int>& output);
		double ForwardAlgorithm(const vector<int>& observations, const int t);
		double BackwardAlgorithm(const vector<int>& observations, const int t);
		bool ParseModelFile(const string& modelPath);
		void Clear();
		void PrintModel();
		void WriteModel(const string& path, bool asLogProbs=true);
	private:
		DiscreteHmmDataset _dataset;
		ColumnMatrix<double> _alphaLattice;
		ColumnMatrix<double> _betaLattice;
		ColumnMatrix<double> _viterbiLattice;
		ColumnMatrix<int> _ptrLattice;
		vector<double> _pi;
		Matrix<double> _stateMatrix;
		//transition matrix semantics: rows = states, cols = emissions
		Matrix<double> _transitionMatrix;
		Matrix<double> _gammaMatrix;
		vector<Matrix<double> >	_chiMatrices;


		void _split(const string& str, const char delim, vector<string>& tokens);
		double _logSumExp(const vector<double>& vec, double b);
		bool _validate();
};

#endif
