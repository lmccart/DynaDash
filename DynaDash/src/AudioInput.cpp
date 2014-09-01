//
//  AudioInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "AudioInput.h"


void AudioInput::setup(ofBaseApp *app) {
    
    
    int bufferSize = 256;
    soundStream.listDevices();
    soundStream.setup(app, 0, 2, 44100, bufferSize, 4);
    
    //soundStream.start();
    
    decayRate = 0.05;
    minimumThreshold = 0.1;
    threshold = minimumThreshold;
}

void AudioInput::update() {
    for (int i=0; i<4; i++) {
        status[i] = ofRandom(1.0) >= 0.5 ? 0 : 1;
    }
}
