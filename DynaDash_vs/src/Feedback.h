//
//  Feedback.h
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "ofMain.h"
#include "ofxUI.h"


class Feedback {
    
public:
    void setup();
    void draw(vector<float> expression, vector<float> volume, vector<float> dominance);
    
private:
    void drawArc(float amt);
    ofColor ledColor;
    vector<float> ledAlpha;
    vector<ofVec2f> ledCenters;
    float easing;

};