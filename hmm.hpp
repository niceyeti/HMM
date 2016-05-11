#ifndef HMM_HPP
#define HMM_HPP

#include "discrete_hmm_dataset.hpp"
#include "matrix.hpp"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

/*
A simple, discrete hidden markov model implementation.
*/
class DiscreteHmm{
	public: 
		DiscreteHmm(const string& path);
		~DiscreteHmm();
		
		void Train(DiscreteHmmDataset& dataset);
		void BaumWelch(const vector<int>& observations);
		double Viterbi(const vector<int>& observations, const int t, vector<int>& output);
		double ForwardAlgorithm(const vector<int>& observations, const int t);
		double BackwardAlgorithm(const vector<int>& observation, const int t);
	private:
		DiscreteHmmDataset _dataset;
		ColumnMatrix _alphaLattice;
		ColumnMatrix _betaLattice;
		ColumnMatrix _viterbiLattice;
		ColumnMatrix _ptrLattice;
		vector<double> _pi;
		Matrix _stateMatrix;
		Matrix _transitionMatrix;

		double _logSumExp(const vector<double>& vec, double b);
};










#endif
