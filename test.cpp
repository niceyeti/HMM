#include "Hmm.hpp"


int main(int argc, char** argv)
{
	DiscreteHmm hmm("test.hmm");

	hmm.WriteModel("myHmm.hmm");

	return 0;
}
