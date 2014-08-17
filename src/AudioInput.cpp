//
//  AudioInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "AudioInput.h"


AudioInput::AudioInput() {
}

AudioInput::~AudioInput() {}


void AudioInput::update() {
    for (int i : status) {
        status[i] = ofRandom(1.0) > 0.5 ? 0 : 1;
    }
}
