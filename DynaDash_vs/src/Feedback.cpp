//
//  Feedback.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "Feedback.h"

void Feedback::setup() {
	ledColor = ofColor::yellow;
	ledAlpha = vector<float>(4, 255);
	ledCenters = vector<ofVec2f>(4);
	ledCenters[0] = ofVec2f(0, 300);
	ledCenters[1] = ofVec2f(-100, 200);
	ledCenters[2] = ofVec2f(0, 100);
	ledCenters[3] = ofVec2f(100, 200);
	easing = 0.8;

    audioPlot.historyLength = 100;
    audioPlot.addColor(ofColor::fromHex(0xF4C341));
    audioPlot.addColor(ofColor::fromHex(0x05A180));
    audioPlot.addColor(ofColor::fromHex(0xE25D48));
    audioPlot.addColor(ofColor::fromHex(0x055D48));
}

void Feedback::update(vector<float>volume) {
	audioPlot.addData(volume);
}

void Feedback::draw(vector<float> expression, vector<float> normVolume, vector<bool> speaking, vector<float> dominance) {
    
    ofPushMatrix();
    ofBackground(ofColor(40));
    ofTranslate(100, 0);
    
    // speaking LED
    
   //ofLogNotice() << "volume: " << volume[0] << " " << volume[1] << " " << volume[2] << " " << volume[3];

	ofPushStyle();
    for (int i=0; i<4; i++) {
        ofSetColor(ledColor[0], ledColor[1], ledColor[2], 100);
        ofNoFill();
		ofSetLineWidth(normVolume[i] * 12);
        ofCircle(40*i, ofGetHeight()/2, 15);
        if (speaking[i]) {
            ofSetColor(ledColor);
            ofFill();
            ofCircle(40*i, ofGetHeight()/2, 15);
        }
    }
	ofPopStyle();

	ofTranslate(200, 0);
	audioPlot.draw(200, 100);

    ofTranslate(350, 0);
   
    // facial expression dial
    drawArc(expression[0]);
    ofTranslate(350, 0);
    
    // dominance/interruption LED display
    for (int i=0; i<4; i++) {
        float nextAlpha = 255*dominance[i];
        ofSetColor(ledColor[0], ledColor[1], ledColor[2], 100);
        ofNoFill();
        ofCircle(ledCenters[i].x, ledCenters[i].y, 15);
        ofSetColor(ledColor[0], ledColor[1], ledColor[2], nextAlpha*(1-easing)+ledAlpha[i]*easing);
        ofFill();
        ledAlpha[i] = nextAlpha;
        ofCircle(ledCenters[i].x, ledCenters[i].y, 15);
    }
    
    ofPopMatrix();

}

void Feedback::drawArc(float amt) {
    amt = ofClamp(amt, 0, 1);
    float unit = ofGetWidth()*0.1;
    float h = ofGetHeight()*0.5;
    ofPath expressionCurve;
	ofPoint p(0, h);
	expressionCurve.arc(p, unit, unit, -180, 180*amt-180);
	expressionCurve.arcNegative(p, 0.5*unit, 0.5*unit, 180*amt-180, -180);
	expressionCurve.close();
	expressionCurve.setCurveResolution(60);
	ofColor c(255, 0,0);
	expressionCurve.setFillColor(c);
	expressionCurve.setFilled(true);
	expressionCurve.draw();
}

