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

template<typename T>
int Flyweight<T>::AddItem(const T& item)
{
	_forwardTable.insert(std::pair<T,int>(item,_numKeys));
	_reverseTable.resize(_numKeys+1);
	_reverseTable[_numKeys] = item;
	_numKeys++;

	return _numKeys-1;
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

