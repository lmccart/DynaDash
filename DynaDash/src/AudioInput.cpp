//
//  AudioInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "AudioInput.h"

void AudioInput::setup() {
    
    
    int bufferSize = 256;
    soundStream.listDevices();
    
    mics[0].setup(4);
    mics[1].setup(5);
}

void AudioInput::update() {
    for (int i=0; i<4; i++) {
        if (i<2) {
            status[i] = mics[i].getVolume();
        }
    }
}
