#pragma once

#include "FWT.h"
#include <math.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * \
* FWT
* Haar Wavelet Transform - Fast Algorithm
*
* Written with help from https://www.youtube.com/watch?v=H5uY5BmSyJE
* and http://www.bearcave.com/misl/misl_tech/wavelets/haar.html
* http://iem.kug.ac.at/fileadmin/media/iem/projects/2008/poblete_wavlets.pdf
* Cameron Thomas - 04/12/2016 -
* Goldsmiths Advanced Audio-visual Processing
\ * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const unsigned Mod37BitPosition[] =
{ 0, 0, 1, 26, 2, 23, 27, 0, 3, 16, 24, 30, 28, 11, 0, 13, 4,
7, 17, 0, 25, 22, 31, 15, 29, 10, 12, 6, 0, 21, 14, 9, 5,
20, 8, 19, 18
};

unsigned int trailing_zeros(unsigned int n) {
	return Mod37BitPosition[(-n & n) % 37];		//simple table lookup for trailing zero counting
}

bool IsPowerOfTwo(unsigned long x) {
	return (x & (x - 1)) == 0;
}

void inPlaceFastHaarWaveletTransform(std::vector<double> &sample) {
	unsigned long size = sample.size();
	if (!IsPowerOfTwo(size)) { cout << "not power of 2 \n"; return; }

	int nSweeps_ = log2(size);
	inPlaceFastHaarWaveletTransform_nSweeps(sample, size, nSweeps_);
}

//this function can be called without checking if you already know your array/sweeps size for faster speed
void inPlaceFastHaarWaveletTransform_nSweeps(std::vector<double> &sample, int nSamples, int nSweeps) {

	int i = 1;						//index to increment
	int GAP = 2;					//initial number of elements btwn averages

	double a = 0;
	double c = 0;
	for (int j = 1; j <= nSweeps; j++) {
		nSamples *= 0.5;
		for (int k = 0; k < nSamples; k++) {
			a = (sample[GAP*k] + sample[GAP*k + i]) / 2;
			c = (sample[GAP*k] - sample[GAP*k + i]) / 2;
			sample[GAP * k] = a;
			sample[GAP * k + i] = c;
		}
		i = GAP;
		GAP *= 2;
	}
}

void IFHWT();


void inPlaceFastInverseHaarWaveletTransform(std::vector<double> &sample) {
	int size = sample.size();
	int nSweeps_ = log2(size);
	int GAP = (int)(pow(2.0, nSweeps_ - 1));
	int JUMP = 2 * GAP;
	int NUM_FREQS = 1;
	for (int SWEEP_NUM = nSweeps_; SWEEP_NUM >= 1; SWEEP_NUM--) {
		for (int K = 0; K < NUM_FREQS; K++) {
			double aPlus = sample[JUMP * K] + sample[JUMP * K + GAP];
			double aMinus = sample[JUMP * K] - sample[JUMP * K + GAP];
			sample[JUMP * K] = aPlus;
			sample[JUMP * K + GAP] = aMinus;
		}
		JUMP = GAP;
		GAP /= 2;
		NUM_FREQS *= 2;
	}
}

// num_iters should be log(n)/log(2) if in-place fast haar transform is to
// be computed completely.


void displayOrderedFreqsFromInPlaceHaar(vector<double> &in_place_sample) {
	int n = in_place_sample.size();
	//System.out.println("N = " + n);
	if (n < 2 || !IsPowerOfTwo(n)) {
		return;
	}
	if (n == 2) {
		cout << (in_place_sample[0]) << endl;
		cout << (in_place_sample[1]) << endl;
		return;
	}
	cout << (in_place_sample[0]) << endl;
	cout << (in_place_sample[n / 2]) << endl;
	int START_INDEX = n / 4;
	int NUM_FREQS = 2;
	while (START_INDEX > 1) {
		int ODD = 1;
		for (int K = 0; K < NUM_FREQS; K++) {
			cout << in_place_sample[START_INDEX * ODD] << "\t";
			ODD += 2;
		}
		cout << endl;
		START_INDEX /= 2;
		NUM_FREQS *= 2;
	}
	// START_INDEX must be one for the next loop to run
	assert(START_INDEX == 1);
	for (int i = 1; i < n; i += 2) {
		cout << in_place_sample[i] << "\t";
	}
	cout << endl;
}

double hanning(long windowLength, long windowPos) {
	return 0.5 * (1.0 - cos((2.0 * PI * windowPos) / (windowLength - 1)));
}