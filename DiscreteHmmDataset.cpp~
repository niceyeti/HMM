#include "DiscreteHmmDataset.hpp"

DiscreteHmmDataset::DiscreteHmmDataset()
{}

DiscreteHmmDataset::DiscreteHmmDataset(const string& path, bool isLabeledData)
{
	if(isLabeledData){
		BuildLabeledDataset(path);
	}
	else{
		BuildUnlabeledDataset(path);
	}
}

DiscreteHmmDataset::~DiscreteHmmDataset()
{
	Clear();
}

void DiscreteHmmDataset::Clear()
{
	LabeledDataSequence.clear();
	UnlabeledDataSequence.clear();
	_stateFlyweight.Clear();
	_symbolFlyweight.Clear();
}

int DiscreteHmmDataset::NumInstances()
{
	return LabeledDataSequence.size();
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
void DiscreteHmmDataset::BuildUnlabeledDataset(const string& path)
{
	int symbolId;
	fstream testFile;
	string line;
	int lineNum;

	//clear any existing data
	Clear();

	testFile.open(path.c_str(),ios::in);
	if(!testFile.is_open()){
		cout << "ERROR could not open dataset file: " << path << endl;
	}

	cout << "Building dataset..." << endl;
	lineNum = 0;
	while(getline(testFile,line)){
		if(line.find("\t") == string::npos){
			//put symbol in the flyweight
			symbolId = _symbolFlyweight.AddItem(line);
			//add symbol to the training sequence
			UnlabeledDataSequence.push_back(symbolId);
		}
		else{
			cout << "ERROR tab found in unsupervised data example, for file: " << path << endl;
		}
		lineNum++;
	}
	cout << "Dataset build completed. Dataset has num symbols/states: " << this->NumSymbols() << "/" << this->NumStates() << endl;
}

/*
Builds dataset in memory from a file of training sequences formatted as
<emission symbol>\t<hidden state symbol>.
*/
void DiscreteHmmDataset::BuildLabeledDataset(const string& path)
{
	int stateId, symbolId;
	fstream testFile;
	string line;
	string symbol;
	string state;
	int tabIndex, lineNum;

	//clear any existing data
	Clear();

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
			LabeledDataSequence.push_back(pair<int,int>(stateId,symbolId));
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





