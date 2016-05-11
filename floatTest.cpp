#include <limits>
#include <iostream>

using namespace std;

int main(void)
{
	double d1, d2, d3, d4;
	double a = 57.46;

	if(numeric_limits<double>::has_infinity || numeric_limits<double>::is_iec559){
		cout << "This system implements +/- infinity!" << endl;
	}
	else{
		cout << "ERROR this system is not standards-compliant, does not implement infinity" << endl;
	}

	cout << "Infinity: " << numeric_limits<double>::infinity() << endl;
	cout << "-Infinity: " << (-numeric_limits<double>::infinity()) << endl;
	cout << "-Infinity - a: " << (-numeric_limits<double>::infinity() - a) << endl;
	cout << "-Infinity + a: " << (-numeric_limits<double>::infinity() + a) << endl;
	//expect false (0)
	cout << "-Infinity > -a? " << (-numeric_limits<double>::infinity() > -a) << endl;
	//expect true (1)
	cout << "-Infinity < -a? " << (-numeric_limits<double>::infinity() < -a) << endl;
	//expect -inf
	cout << "-Infinity + -Infinity: " << (-numeric_limits<double>::infinity() + -numeric_limits<double>::infinity()) << endl;
	//expcet nan
	cout << "-Infinity + Infinity: " << (-numeric_limits<double>::infinity() + numeric_limits<double>::infinity()) << endl;

	return 0;
}

