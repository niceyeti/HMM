#include "Flyweight.hpp"

template<typename T>
Flyweight<T>::Flyweight()
{
	_numKeys = 0;
}

template<typename T>
Flyweight<T>::~Flyweight()
{
	Clear();
}

/*
Adds an item to the tables, if it does not already exist.
*/
template<typename T>
int Flyweight<T>::AddItem(const T& item)
{
	int id = -1;
	typename map< T,int>::iterator key = _forwardTable.find(item);

	//item doesn't already exist, so add it
	if(key == _forwardTable.end()){
		_forwardTable.insert(std::pair<T,int>(item,_numKeys));
		_reverseTable.resize(_numKeys+1);
		_reverseTable[_numKeys] = item;
		id = _numKeys;
		_numKeys++;
	}
	else{
		//item already exists, so just return its id
		id = key->second;
	}

	return id;
}


template<typename T>
int Flyweight<T>::NumItems()
{
	return _numKeys;
}

template<typename T>
void Flyweight<T>::Clear()
{
	_forwardTable.clear();
	_reverseTable.clear();
	_numKeys = 0;
}

template<typename T>
T& Flyweight<T>::KeyToItem(const int key)
{
	if(key < 0 && key >= _numKeys){
		cout << "ERROR itemNum out of range in GetItem()" << endl;
	}

	return _reverseTable[key];
}

template<typename T>
int Flyweight<T>::GetItemKey(const T& item)
{
	typename map< T,int>::iterator key = _forwardTable.find(item);

	if(key != _forwardTable.end()){
		return *key;
	}
	
	return -1;
}

