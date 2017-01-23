#pragma once

#include "ofMain.h"
#include "maximilian.h"
#include "FWT.h"
#include "Daub.h"
#include <random>

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void audioOut(float * output, int bufferSize, int nChannels, int deviceID, long unsigned long tickCount);
	void exit();

	ofSoundStream soundstream;
	unsigned int samplerate;
	unsigned int buffersize;
	float myoutput[2];
	float freq;

	maxiOsc osc1, osc2, osc3, osc4;
	int testsize, nSweeps;
	ofPolyline line;

	Daubechies daub;

	std::vector<double> buf1, buf1b, buf1c, buf_display;

	std::vector<double> A_440_512;
	std::vector<double> A_440_1024;

	double testbuffer1[512];

	bool t1;
	std::default_random_engine generator;		//for generating gaussian
};
