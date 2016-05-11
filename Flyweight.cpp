#include "Flyweight.hpp"

Flyweight::Flyweight()
{

}

Flyweight::~Flyweight()
{
	Clear();
}

int Flyweight::AddItem(const T& item)
{
	_forwardTable[item] = _numKeys;
	_reverseTable.resize(_numKeys);
	_reverseTable[_numKeys] = item;
}

void Flyweight::Clear()
{
	_forwardTable.clear();
	_reverseTable.clear();
	_numKeys = 0;
}

T& Flyweight::GetItem(const int key)
{
	if(key < 0 && key >= _numKeys){
		cout << "ERROR itemNum out of range in GetItem()" << endl;
	}

	return _reverseTable[key];
}

int Flyweight::GetItemKey(const T& item)
{
	if(item in map){
		return _forwardTable[item];
	}
	
	return -1;
}

