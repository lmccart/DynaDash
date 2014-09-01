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
    void draw(float expression[4], float dominance[4]);
    
private:
    void drawArc(float amt);
    ofColor ledColor = ofColor::yellow;
    float ledAlpha[4] = {255, 255, 255, 255};
    ofVec2f ledCenters[4] = { ofVec2f(0, 300), ofVec2f(-100, 200), ofVec2f(0, 100), ofVec2f(100, 200) };
    float easing = 0.8;

};