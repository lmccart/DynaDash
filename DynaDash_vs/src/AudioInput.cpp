//
//  AudioInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "AudioInput.h"

void AudioInput::setup() {
    
    lastVolume = vector<float>(4, 0);
    volume = vector<float>(4, 0);
    normalizedVolume = vector<float>(4, 0);
	speaking = vector<bool>(4, false);

    int bufferSize = 256;
    soundStream.listDevices();
    
    mics[0].setup(7);
    mics[1].setup(8);
    mics[2].setup(9);
    mics[3].setup(10);
}

void AudioInput::update() {
    for (int i=0; i<4; i++) {
		float v = ofLerp(lastVolume[i], mics[i].getVolume(), (1.0-smoothAmt));
		lastVolume[i] = volume[i];
		volume[i] = v;
    }
	normalizeVolumes();
}

void AudioInput::normalizeVolumes() {
	    
	float volumeSum = 0;
	for(int i = 0; i < 4; i++) {
		volumeSum += volume[i];
	}
	for(int i = 0; i < 4; i++) {
		normalizedVolume[i] = volume[i] / volumeSum;
	}
	int maxVolumeIndex = 0;
	float maxVolume = 0;
	for(int i = 0; i < 4; i++) {
		if(normalizedVolume[i] > maxVolume) {
			maxVolumeIndex = i;
			maxVolume = normalizedVolume[i];
		}
	}
	for (int i=0; i<4; i++) {
		if (maxVolume > speakingNormalizedThresh && i == maxVolumeIndex) {
			speaking[i] = true;
		} else {
			speaking[i] = false;
		}
	}
}