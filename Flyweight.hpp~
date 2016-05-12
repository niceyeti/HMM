#include <map>
#include <vector>

/*
Simple mapping class for mapping strings to ints, and ints to strings.
This is useful since it allows strings (words, parts of speech, etc.) to 
be stored as simple ints for algorithmic purposes, then translated back as
needed.
*/
class Flyweight<T>{
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

