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
<hidden symbol>\t<emission symbol>.
*/
void DiscreteHmmDataset::Build(const string& path)
{
	int state, emission;
	fstream testFile;
	string line;
	string word;
	string pos;
	int tabIndex, lineNum;

	testFile.open(path.c_str(),ios::in);
	if(!testFile.is_open()){
		cout << "ERROR could not open dataset file: " << path << endl;
	}

	lineNum = 0;
	while(getline(testFile,line)){
		tabIndex = line.find("\t");
		if(tabIndex > 0){
			//get the word and pos
			word = line.substr(tabIndex,line.length() - tabIndex);
			pos = line.substr(0,tabIndex);
			//put them in the flyweights
			state = _stateFlyweight.AddItem(pos);
			emission = _symbolFlyweight.AddItem(word);
			//add state:symbol pair to the training sequence
			TrainingSequence.push_back(pair<int,int>(state,emission));
		}
		else{
			cout << "ERROR tabIndex not found in file: " << path << "  for line #" << lineNum << ": " << line << endl;
		}
		lineNum++;
	}
}

const string& DiscreteHmmDataset::GetSymbol(const int key)
{
	return _symbolFlyweight.KeyToItem(key);
}

const string& DiscreteHmmDataset::GetState(const int key)
{
	return _stateFlyweight.KeyToItem(key);
}





