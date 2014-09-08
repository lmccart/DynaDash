//
//  AudioInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "AudioInput.h";
#include "StackedPlot.h";

void AudioInput::setup() {   
    volume = vector<float>(4, 0);
    normalizedVolume = vector<float>(4, 0);
	speaking = vector<bool>(4, false);
	interrupting = vector<bool>(4, false);

    int bufferSize = 256;
    soundStream.listDevices();
    
	mics = vector<AudioAnalyzer>(4);
    mics[0].setup(7);
    mics[1].setup(8);
    mics[2].setup(9);
    mics[3].setup(10);
}

void AudioInput::setSmoothing(float amt) {
	AudioAnalyzer::smoothAmount = amt;
}

void AudioInput::update() {
    for (int i=0; i<4; i++) {
		volume[i] = mics[i].getVolume();
    }
	normalizedVolume = StackedPlot::normalize(volume);
	analyzeSpeaking();
}


void AudioInput::analyzeSpeaking() {
	int maxVolumeIndex = 0;
	float maxVolume = 0;
	for(int i = 0; i < 4; i++) {
		if(normalizedVolume[i] > maxVolume) {
			maxVolumeIndex = i;
			maxVolume = normalizedVolume[i];
		}
	}
	
	int lastSpeaker = -1;
	for (int i=0; i<4; i++) {
		if (speaking[i]) {
			lastSpeaker = i;
		}
	}

	for (int i=0; i<4; i++) {
		interrupting[i] = false;
		if (maxVolume > speakingNormalizedThresh && i == maxVolumeIndex) {
			speaking[i] = true;
			if (lastSpeaker != -1 && i != lastSpeaker) {
				interrupting[i] = true;
			}
		} else {
			speaking[i] = false;
		}
	}
}