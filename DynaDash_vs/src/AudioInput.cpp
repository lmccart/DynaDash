//
//  AudioInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "AudioInput.h"

void AudioInput::setup() {
    
    status = vector<float>(4, 0);

    int bufferSize = 256;
    soundStream.listDevices();
    
    mics[0].setup(5);
    mics[1].setup(6);
}

void AudioInput::update() {
    for (int i=0; i<4; i++) {
        if (i<2) {
            status[i] = mics[i].getVolume();
        }
    }
}
