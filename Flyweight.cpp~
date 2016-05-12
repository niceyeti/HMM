#include "Flyweight.hpp"

Flyweight<T>::Flyweight()
{
	_numKeys = 0;
}

Flyweight<T>::~Flyweight()
{
	Clear();
}

int Flyweight<T>::AddItem(const T& item)
{
	_forwardTable[item] = _numKeys;
	_reverseTable.resize(_numKeys);
	_reverseTable[_numKeys] = item;
	_numKeys++;
}

void Flyweight<T>::Clear()
{
	_forwardTable.clear();
	_reverseTable.clear();
	_numKeys = 0;
}

T& Flyweight<T>::GetItem(const int key)
{
	if(key < 0 && key >= _numKeys){
		cout << "ERROR itemNum out of range in GetItem()" << endl;
	}

	return _reverseTable[key];
}

int Flyweight<T>::GetItemKey(const T& item)
{
	map<T,int>::iterator key = _forwardTable.find(item);

	if(key != _forwardTable.end()){
		return *key;
	}
	
	return -1;
}

