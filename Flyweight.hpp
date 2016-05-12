#ifndef FLYWEIGHT_HPP
#define FLYWEIGHT_HPP

#include <map>
#include <vector>

using namespace std;

/*
Simple mapping class for mapping strings to ints, and ints to strings.
This is useful since it allows strings (words, parts of speech, etc.) to 
be stored as simple ints for algorithmic purposes, then translated back as
needed.
*/
template<typename T> class Flyweight{
	public:
		~Flyweight();
		Flyweight();		
		int AddItem(const T& item);
		void Clear();
		T& KeyToItem(const int key);
		int GetItemKey(const T& item);
	private:
		map<T,int> _forwardTable;
		vector<T> _reverseTable;
		int _numKeys;
};

#endif
