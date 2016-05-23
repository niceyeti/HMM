#include "Hmm.hpp"

int main(int argc, char** argv)
{
	DiscreteHmm hmm("test.hmm");

	//Verify model can be written
	hmm.WriteModel("myHmm.hmm",false);
	//Verify model can cleared
	hmm.Clear();
	//Verify model can be read
	hmm.ReadModel("myHmm.hmm");
	//Verify the model can be written, and is the same as what was written/read previously above
	hmm.WriteModel("myHmm2.hmm",false);





	return 0;
}

