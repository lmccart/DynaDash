//
//  DebugFeedback.h
//  DynaDash
//
//  Created by Lauren McCarthy on 9/11/14.
//
//

#include "ofMain.h"
#include "ofxUI.h"
#include "StackedPlot.h"


class DebugFeedback {
    
public:
    void setup(float width);
	void update(vector<float>volume);
    void draw(vector<float> normVolume, vector<bool> speaking);
    
private:
	ofColor ledColor;
	StackedPlot audioPlot;
	float w;

};