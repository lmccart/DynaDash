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
	ledWarnColor = ofColor::red;
	ledAlpha = vector<float>(4, 255);
	ledWarnAlpha = vector<float>(4, 255);
	ledCenters = vector<ofVec2f>(4);
	ledCenters[0] = ofVec2f(0, 300);
	ledCenters[1] = ofVec2f(-100, 200);
	ledCenters[2] = ofVec2f(0, 100);
	ledCenters[3] = ofVec2f(100, 200);
	easing = 0.1;

    audioPlot.historyLength = 100;
    audioPlot.addColor(ofColor::fromHex(0xF4C341));
    audioPlot.addColor(ofColor::fromHex(0x05A180));
    audioPlot.addColor(ofColor::fromHex(0xE25D48));
    audioPlot.addColor(ofColor::fromHex(0x055D48));
}

void Feedback::update(vector<float>volume) {
	audioPlot.addData(volume);
}

void Feedback::draw(vector<float> expression, vector<bool> interrupting, vector<float> dominance) {
    
    ofBackground(ofColor(40));

    ofPushMatrix();
	ofTranslate(425, 0);
	
    // facial expression dial
    drawArc(expression[0]);

	ofTranslate(375, 0);

    // DOMINANCE/SPEAKING LED DISPLAY
	ofPushStyle();
	ofSetLineWidth(2.5);
    for (int i=0; i<4; i++) {
        float nextAlpha = 255*dominance[i];
        ofSetColor(ledColor[0], ledColor[1], ledColor[2], 100);
        ofNoFill();
        ofCircle(ledCenters[i].x, ledCenters[i].y, 25);

        ofSetColor(ledColor[0], ledColor[1], ledColor[2], ofLerp(ledAlpha[i], nextAlpha, easing));
        ofFill();
        ledAlpha[i] = nextAlpha;
        ofCircle(ledCenters[i].x, ledCenters[i].y, 25);

		float warnAlpha = 0;
		if (interrupting[i]) {
			warnAlpha = 255.0;
		} else {
			warnAlpha = ofLerp(ledWarnAlpha[i], 0, easing);
		}
		ledWarnAlpha[i] = warnAlpha;
		ofSetColor(ledWarnColor[0], ledWarnColor[1], ledWarnColor[2], warnAlpha);
		ofFill();
		ofCircle(ledCenters[i].x, ledCenters[i].y, 25);
    }
	ofPopStyle();
	
    
    ofPopMatrix();

}

void Feedback::drawArc(float amt) {
	ofPushStyle();
    float unit = ofGetWidth()*0.15;
    float h = ofGetHeight()*0.5;
	ofPoint p(0, h);
	ofColor blue(70, 70, 200);
	ofColor gray(100);

    ofPath expressionCurveBack;
	expressionCurveBack.setCurveResolution(100);
	expressionCurveBack.arc(p, unit, unit, -180, 0);
	expressionCurveBack.setFillColor(gray);
	expressionCurveBack.setFilled(true);
	expressionCurveBack.draw();
	
    ofPath expressionCurve;
	expressionCurve.setCurveResolution(100);
	expressionCurve.arc(p, unit, unit, -180, ofClamp(180*amt-180, -178, 0));
	ofColor c(255, 0,0);
	expressionCurve.setFillColor(blue);
	expressionCurve.setFilled(true);
	expressionCurve.draw();

	ofPopStyle();
}

