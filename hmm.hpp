#ifndef HMM_HPP
#define HMM_HPP

#include "matrix.hpp"
#include <map>
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
		
	private:
		DiscreteHmmDataset _dataset;
		Matrix _alphaMatrix;
		Matrix _betaMatrix;
		vector<double> _pi;
		Matrix _stateMatrix;
		Matrix _transitionMatrix;
};










#endif
