#include "flyweight.hpp"
#include <string>

/*
Primitive data object class for consuming files containing training sequences
formatted as follows, tab delimited:
	<hidden state>\t<discrete emission>

So a hmm with part-of-speech latent variables and word emission values
would look as follows:
	NN[TAB]cat

Where each line is a single example of state+emission_symbol, and the file
is expected to contain lines of such examples.
*/
class DiscreteHmmDataset{
	public:
		DiscreteHmmDataset(const string& path);
		DiscreteHmmDataset() = delete;		
		~DiscreteHmmDataset();
		void Build(const string& path);
		void Clear();
		int NumStates();
		int NumSymbols();
		vector<pair<int,int> > TrainingSequence;
	private:
		Flyweight<string> _stateFlyweight;
		Flyweight<string> _symbolFlyweight;
};

