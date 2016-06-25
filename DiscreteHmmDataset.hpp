#ifndef DISCRETE_HMM_DATASET
#define DISCRETE_HMM_DATASET

#include "Flyweight.hpp"
#include "Flyweight.cpp"

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

/*
Primitive data object class for consuming files containing training sequences
formatted as follows, tab delimited:
	<hidden state symbol>\t<discrete emission symbol>

So a hmm with part-of-speech latent variables and word emission values
would look as follows:
	NN[TAB]cat

Where each line is a single example of state+emission_symbol, and the file
is expected to contain lines of such examples.

TODO: Template the Dataset class (for string, char, etc) for the class labels.
*/
class DiscreteHmmDataset{
	public:
		DiscreteHmmDataset();
		DiscreteHmmDataset(const string& dataPath);
		~DiscreteHmmDataset();
		void Build(const string& path);
		void Clear();
		int NumStates();
		int NumSymbols();
		int AddState(const string& state);
		int AddSymbol(const string& symbol);
		const string& GetSymbol(const int key);
		const string& GetState(const int key);
		int NumInstances();
		vector<pair<int,int> > TrainingSequence;
	private:
		Flyweight<string> _stateFlyweight;
		Flyweight<string> _symbolFlyweight;
};

#endif
