#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);

	//set buffersize and SR
	buffersize = 4096;
	samplerate = 44100;

	//fwt parameter - nsweeps
	nSweeps = log2(buffersize);		//do log beforehand for efficiency

	//Init arrays and fill with 0s
	buf1.resize(buffersize);
	buf1b.resize(buffersize);
	buf1c.resize(buffersize);
	buf_display.resize(buffersize);
	fill(buf1.begin(), buf1.end(), 0.);
	fill(buf1b.begin(), buf1b.end(), 0.);
	fill(buf1c.begin(), buf1c.end(), 0.);
	fill(buf_display.begin(), buf_display.end(), 0.);

	soundstream.setup(this, 2, 0, samplerate, buffersize, 4);

	t1 = true;
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {

	float XSCALE = 0.8;
	float YSCALE = 0.5;
	int XBORDER = ofGetWidth() * (1 - XSCALE) / 2;
	int YBORDER = ofGetHeight() * (1 - YSCALE) / 2;
	ofBackground(5);

	for (int i = 0; i < buf_display.size(); i++) {

		int width_div = ((buf_display.size() / 2) >> trailing_zeros(i));
		int height_div = (2 << trailing_zeros(i));

		float height = (float)(ofGetHeight()-YBORDER) / (float)height_div;
		float width = (float)(ofGetWidth()) / (float)width_div;
		int count = ((float)(i + 1) / height_div) + 1;

		ofVec2f intersection = ofVec2f((count*width)-width, ((ofGetHeight()-YBORDER) / (height_div/2)) );

		ofSetRectMode(OF_RECTMODE_CORNER);
		int c = 0;
		if (buf_display[i] > 0)
			c = (int)ofMap(buf_display[i], 0, 1, 0, 255, true);
		else
			c = (int)ofMap(buf_display[i], 0, -1, 0, 255, true);

		ofSetColor(c, 0, 10);
		ofFill();

		ofPushMatrix();
		ofTranslate(ofGetWidth()/4, YBORDER/2);
		ofDrawRectangle(intersection, width, -height);
		ofPopMatrix();
	}

	ofSetColor(255);
	ofFill();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

	freq = ((ofGetHeight()+30) - ofGetMouseY()) * 10;
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	//cout << "actual freq\t" << freq << endl;

	//displayOrderedFreqsFromInPlaceHaar(buf2);
	t1 = !t1;
	cout << t1 << endl;
}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::audioOut(float * output, int bufferSize, int nChannels, int deviceID, long unsigned long tickCount) {

	int j = 0;

	//synthesise..
	for (int i = 0; i < bufferSize; i++) {
		buf1[i] = osc1.sinewave(freq) * 0.9;
		buf1b[i] = osc2.sinewave(500) * 0.9;
		buf1c[i] = osc3.sinewave(500) * 0.9 * osc4.sinewave(freq) * 0.9;
	}

	//transform
	inPlaceFastHaarWaveletTransform_nSweeps(buf1, buffersize, nSweeps);
	inPlaceFastHaarWaveletTransform_nSweeps(buf1b, buffersize, nSweeps);
	inPlaceFastHaarWaveletTransform_nSweeps(buf1c, buffersize, nSweeps);

	//manipulate
	for (int i = 0; i < bufferSize; i++) {
		int width_div = ((bufferSize / 2) >> trailing_zeros(i));
		int height_div = (2 << trailing_zeros(i));
			buf1[i] *= buf1b[i];
		if(t1)
			buf_display[i] = buf1[i];
		else
			buf_display[i] = buf1c[i];
	}
	
	//resynthesise
	if(t1)
		inPlaceFastInverseHaarWaveletTransform(buf1);
	else
		inPlaceFastInverseHaarWaveletTransform(buf1c);

	for (int i = 0; i < bufferSize; i++) {
		if (t1) {
			myoutput[0] = buf1[i];
			myoutput[1] = buf1[i];
		}
		else {
			myoutput[0] = buf1c[i];
			myoutput[1] = buf1c[i];
		}
		output[i*nChannels] = myoutput[0];
		output[i*nChannels + 1] = myoutput[1];
	}
}

void ofApp::exit() {
	soundstream.close();
}