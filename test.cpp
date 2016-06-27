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

	DiscreteHmmDataset dataset;
	dataset.BuildLabeledDataset("./data/data_100000_Points.txt");

	//Build hidden and emission models directly from a labelled dataset
	hmm.DirectTrain(dataset);
	hmm.Clear();
	dataset.Clear();


	//test BaumWelch on unlabeled data
	dataset.BuildUnlabeledDataset("./data/unlabeled_100000_Points.txt");
	hmm.BaumWelch(dataset,2);


	return 0;
}

