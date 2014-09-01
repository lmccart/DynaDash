//
//  Feedback.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "Feedback.h"

void Feedback::setup() {
    
}

void Feedback::draw(float expression[4], float dominance[4]) {
    
    ofPushMatrix();
    ofBackground(ofColor(40));
    ofTranslate(ofGetWidth()*0.25, 0);
    drawArc(expression[0]);
    ofTranslate(ofGetWidth()*0.50, 0);
    //drawArc(dominance[0]);
    
    for (int i=0; i<4; i++) {
        float nextAlpha = 255*dominance[i];
        ofSetColor(ledColor[0], ledColor[1], ledColor[2], nextAlpha*(1-easing)+ledAlpha[i]*easing);
        ledAlpha[i] = nextAlpha;
        ofCircle(ledCenters[i].x, ledCenters[i].y, 15);
    }
    
    ofPopMatrix();

}

void Feedback::drawArc(float amt) {
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

