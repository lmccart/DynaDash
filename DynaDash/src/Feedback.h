//
//  Feedback.h
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "ofMain.h"
#include "ofxUI.h"
#include "StackedPlot.h"
#include "ofxESCPOS.h"

class Feedback {
    
public:
    void setup();
	void update(vector<float>volume);
    void draw(vector<float> expression, vector<bool> interrupting, vector<float> dominance);
    void printResults();
    
private:
    void drawArc(float amt);
    ofColor ledColor;
	ofColor ledWarnColor;
    vector<float> ledAlpha;
    vector<float> ledWarnAlpha;
    vector<ofVec2f> ledCenters;
    float easing;

    StackedPlot audioPlot;
    
    vector<ofx::ESCPOS::DefaultSerialPrinter> printers;

};