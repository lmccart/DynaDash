//
//  VideoInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "VideoInput.h"

void VideoInput::setup() {
    
}

void VideoInput::update() {
    for (int i : status) {
        status[i] = ofClamp(status[i]+ofRandom(-0.01, 0.01), 0, 1.0);
    }
}
