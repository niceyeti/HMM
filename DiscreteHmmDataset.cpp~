#include "DiscreteHmmDataset.hpp"

DiscreteHmmDataset::DiscreteHmmDataset()
{}

DiscreteHmmDataset::DiscreteHmmDataset(const string& path)
{
	Build(path);
}

DiscreteHmmDataset::~DiscreteHmmDataset()
{
	Clear();
}

void DiscreteHmmDataset::Clear()
{
	TrainingSequence.clear();
	_stateFlyweight.Clear();
	_symbolFlyweight.Clear();
}

int DiscreteHmmDataset::NumInstances()
{
	return TrainingSequence.size();
}

int DiscreteHmmDataset::AddState(const string& state)
{
	return _stateFlyweight.AddItem(state);
}

int DiscreteHmmDataset::AddSymbol(const string& symbol)
{
	return _symbolFlyweight.AddItem(symbol);
}

int DiscreteHmmDataset::NumStates()
{
	return _stateFlyweight.NumItems();
}

int DiscreteHmmDataset::NumSymbols()
{
	return _symbolFlyweight.NumItems();
}

/*
Builds dataset in memory from a file of training sequences formatted as
<emission symbol>\t<hidden state symbol>.
*/
void DiscreteHmmDataset::Build(const string& path)
{
	int stateId, symbolId;
	fstream testFile;
	string line;
	string symbol;
	string state;
	int tabIndex, lineNum;

	testFile.open(path.c_str(),ios::in);
	if(!testFile.is_open()){
		cout << "ERROR could not open dataset file: " << path << endl;
	}

	cout << "Building dataset..." << endl;

	lineNum = 0;
	while(getline(testFile,line)){
		tabIndex = line.find("\t");
		if(tabIndex > 0){
			//get the symbol and its state label
			symbol = line.substr(0,tabIndex);
			state = line.substr(tabIndex+1, line.length()-tabIndex);
			//cout << "symbol/state: " << symbol << "/" << state << endl;
			//and put them in the flyweights
			stateId = _stateFlyweight.AddItem(state);
			symbolId = _symbolFlyweight.AddItem(symbol);
			//add state:symbol pair to the training sequence
			TrainingSequence.push_back(pair<int,int>(stateId,symbolId));
		}
		else{
			cout << "ERROR tabIndex not found in file: " << path << "  for line #" << lineNum << ": " << line << endl;
		}
		lineNum++;
	}
	cout << "Dataset build completed. Dataset has num symbols/states: " << this->NumSymbols() << "/" << this->NumStates() << endl;
}

const string& DiscreteHmmDataset::GetSymbol(const int key)
{
	return _symbolFlyweight.KeyToItem(key);
}

const string& DiscreteHmmDataset::GetState(const int key)
{
	return _stateFlyweight.KeyToItem(key);
}





