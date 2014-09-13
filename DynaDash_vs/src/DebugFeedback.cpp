//
//  DebugFeedback.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 9/11/14.
//
//

#include "DebugFeedback.h"

void DebugFeedback::setup(float width) {
	ledColor = ofColor::yellow;

    audioPlot.historyLength = 100;
    audioPlot.addColor(ofColor::fromHex(0xF4C341));
    audioPlot.addColor(ofColor::fromHex(0x05A180));
    audioPlot.addColor(ofColor::fromHex(0xE25D48));
    audioPlot.addColor(ofColor::fromHex(0x055D48));

	w = width;
}

void DebugFeedback::update(vector<float>volume) {
	audioPlot.addData(volume);
}

void DebugFeedback::draw(vector<float> normVolume, vector<bool> speaking) {

	ofPushStyle();
    ofPushMatrix();

	ofSetColor(ofColor::black);
	ofFill();
	ofRect(0, 0, w, ofGetHeight());

    ofTranslate(0, 170);
    
    // SPEAKING LED
	ofPushStyle();
	float x;
    for (int i=0; i<4; i++) {
        ofSetColor(ledColor[0], ledColor[1], ledColor[2], 100);
        ofNoFill();
		ofSetLineWidth(normVolume[i] * 12);
		x = 45*i + 40;
        ofCircle(x, 30, 15);
        if (speaking[i]) {
            ofSetColor(ledColor);
            ofFill();
            ofCircle(x, 30, 15);
        }
    }
	ofPopStyle();

	ofTranslate(0, 100);
	audioPlot.draw(w, 100);
	
    ofPopMatrix();
	ofPopStyle();
}


