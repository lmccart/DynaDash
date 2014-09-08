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


class Feedback {
    
public:
    void setup();
	void update(vector<float>volume);
    void draw(vector<float> expression, vector<float> normVolume, vector<bool> speaking, vector<float> dominance);
    
private:
    void drawArc(float amt);
    ofColor ledColor;
    vector<float> ledAlpha;
    vector<ofVec2f> ledCenters;
    float easing;

	StackedPlot audioPlot;

};