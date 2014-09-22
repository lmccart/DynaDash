//
//  ExpressionInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "ExpressionInput.h"

void ExpressionInput::setup() {
	status = vector<float>(4, 0);
	status[0] = 0;
    
    cam.initGrabber(640, 480);
    smile.setup();
}

void ExpressionInput::update() {
    for (int i=0; i<status.size(); i++) {
        cam.update();
        if(cam.isFrameNew()) {
            smile.update(cam);
            if(smile.getFaceFound()) {
                float cur = smile.getSmileAmount();
                status[i] = ofClamp(ofMap(cur, 0, 800, 0, 1.0), 0, 1.0);
            }
        }
    }
}