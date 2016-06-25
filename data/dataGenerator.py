"""
Generates data from two separate conditional discrete distributions representing CPG islands, which are
just a convenient application of discrete conditional data sources. This model is nice because
there are only two hidden states, which is a typical use-case for binary classification of sequences.

The following data is from http://web.stanford.edu/class/stats366/exs/HMM1.html

Table of Transition Probabilities for CpG Islands
Model + 	A 	C 	G 	T
			A 	.180 	.274 	.426 	.120
			C 	.171 	.368 	.274 	.188
			G 	.161 	.339 	.375 	.125
			T 	.079 	.355 	.384 	.182
	station 	0.155 	0.341 	0.350 	0.154

Table of Transition Probabilities for non CpG Islands
Model - 	A 	C 	G 	T
			A 	.300 	.205 	.285 	.210
			C 	.322 	.298 	.078 	.302
			G 	.248 	.246 	.298 	.208
			T 	.177 	.239 	.292 	.292
	station 	0.262 	0.246 	0.239 	0.253

This script just generates sequences from each distribution independently, then concatenates them.
The hidden transition matrix is what we are trying to model, hence it is not given.
 
The sequences are formatted for consumption by the DiscreteDataset class, which reads in <hidden,emission> pairs
from a file. So the output of this will be something like:

<+,A>
<+,C>
<+,A>
... (all the rest of the emissions generated from the + model)
<-,A>
<-,T>
<-,T>
... (all the rest of the emissions generated from the - model)

"""

import random
import math

"""
Walk a stochastic matrix n times, emitting symbols at each step


@initialState: an integer for the row index of the initial state
@matrix: square matrix, list of lists of probabilities, where the rows sum to 1.0; hence the rows are the "given" state in p(next|given)
@n: number of steps to walk
@symbols: The symbols corresponding with the row/cols
"""
def walkMatrix(initialState,matrix,n,symbols):
	walk = ""
	steps = 0
	givenState = initialState
	#walk the matrix
	while steps < n:
		#randomly choose a next row and col
		r = float(random.randint(1,10000)) / 10000.0 #generate a random, ten-thousandths precision number in 0.001-1.000
		#check for which probability interval 'r' is in.
		condDistribution = matrix[givenState] #get the row corresponding with the present state's conditional distribution
		#normalize the row
		condDistribution = [(p / float(sum(condDistribution))) for p in condDistribution]
		i = 0
		cumulativeProb = 0.0
		foundNext = False
		while not foundNext:
			cumulativeProb += condDistribution[i]
			if r <= cumulativeProb:
				foundNext = True
			else:
				i += 1
		#post loop: i points to the index of the next state
		givenState = i

		#output the selected symbol
		walk += symbols[givenState]
		steps += 1
	return walk



#rows and cols of both matrices correspond to "acgt"
plusModel = [ \
[0.180, 0.274, 0.426, 0.120], \
[0.171, 0.368, 0.274, 0.188], \
[0.161, 0.339, 0.375, 0.125], \
[0.079, 0.355, 0.384, 0.182]]

minusModel = [ \
[0.300, 0.205, 0.285, 0.210], \
[0.322, 0.298, 0.078, 0.302], \
[0.248, 0.246, 0.298, 0.208], \
[0.177, 0.239, 0.292, 0.292]]

#I made these values up; IIRC, cpg islands are quite small wrt to overall nucleotide sequence length
# row/col correspondence is "+-"

hiddenMatrix = [ \
[0.97, 0.03], \
[0.005, 0.999]]

#symbols = "acgt"
#hiddenSymbols = "+-"
#walk = walkMatrix(1,hiddenMatrix,5000,hiddenSymbols)
#print(walk)

#Generate data by walking the above matrices, switching matrices according to the hiddenMatrix probs.
#To generate more contiguous cpg islands, I clamp the cpg state for at least 50 symbols. I want cpg islands
# to be somewhat rare, like 10% of a sample, but when they occur, want them to be long for good training estimates.
inCpgIsland = False
n = 0
ctr = 0 #num chars output contiguously for the current state. So if the current sequence is: ----++, then ctr is 2, because of ++.
minCpgSymbols = 50 #minimum number of cpg chars to output, or rather how long we clamp the cpg island state
minNonCpgSymbols = 500 #same as prior, but for non-cpg regions
CPG = 1
NON_CPG = 0
while n < 100000:
	#decide which hidden state we are in now
	if inCpgIsland and ctr < minCpgs:
		ctr+=1
		state = CPG
	elif inCpgIsland and ctr >= minCpgs: #emitted enough cpg chars, so randomly decide whether or not to jump states
		r = float(random.randint(1,1000))/1000.0
		cumulativeProb = 0.0
		state = chooseNextState(state,hiddenMatrix)

	




def selectNextState():
	#randomly choose a next state
	r = float(random.randint(1,10000)) / 10000.0 #generate a random, ten-thousandths precision number in 0.001-1.000
	#check for which probability interval 'r' is in.
	condDistribution = matrix[givenState] #get the row corresponding with the present state's conditional distribution
	#normalize the row
	condDistribution = [(p / float(sum(condDistribution))) for p in condDistribution]
	i = 0
	cumulativeProb = 0.0
	foundNext = False
	while not foundNext:
		cumulativeProb += condDistribution[i]
		if r <= cumulativeProb:
			foundNext = True
		else:
			i += 1
	#post loop: i points to the index of the next state
	givenState = i



#outputFile = open("data.txt","w+")

#generate 














