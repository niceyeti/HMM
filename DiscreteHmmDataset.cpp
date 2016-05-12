#include "DiscreteHmmDataset.hpp"

DiscreteHmmDataset::DiscreteHmmDataset(const string& path)
{
	Build(path);
}

DiscreteHmmDataset::Clear()
{
	TrainingSequence.clear();
	_stateFlyweight.clear();
	_symbolFlyweight.clear();
}

int AddState(const string& state)
{
	return _stateFlyweight.AddItem(state);
}

int AddSymbol(const string& symbol)
{
	return _symbolFlyweight(symbol);
}

/*
Builds dataset in memory from a file of training sequences formatted as
<pos>\t<word>.
*/
void DiscreteHmmDataset::Build(const string& path);
{
	int state, emission;
	fstream testFile;
	string line;
	string word;
	string pos;
	int tabIndex, lineNum;

	testFile.open(fname, ios::in);
	if(!testFile.is_open()){
		cout << "ERROR could not open test file: " << fname << endl;
	}

	lineNum = 0;
	while(getline(testFile,line)){
		tabIndex = line.find("\t");
		if(tabIndex > 0){
			//get the word and pos
			word = line.substr(tabIndex,line.length()-tabIndex);
			pos = line.substr(0,tabIndex);
			//put them in the flyweights
			state = _stateFlyweight.AddItem(pos);
			emission = _emissionFlyweight.AddItem(word);
			//add state:symbol pair to the training sequence
			TrainingSequence.add(pair<int,int>(state,emission));
		}
		else{
			cout << "ERROR tabIndex not found in file: " << path << "  for line #" << lineNum << ": " << line << endl;
		}
		lineNum++;
	}
}

const string& DiscreteHmmDataset::GetEmission(const int key)
{
	return _symbolFlyweight.KeyToItem(key);
}

const string& DiscreteHmmDataset::GetState(const int key)
{
	return _stateFlyweight.KeyToItem(key);
}





