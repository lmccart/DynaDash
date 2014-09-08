//
//  AudioAnalyzer.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "AudioAnalyzer.h"

float AudioAnalyzer::smoothAmount = 0;

void AudioAnalyzer::setup(int deviceID) {
	volume = 0;
	smoothedVolume = 0;
    input.setDeviceID(deviceID);
    int outChannels = 0, inChannels = 1;
    int sampleRate = 44100, bufferSize = 256, nBuffers = 4;
    input.setup(this, outChannels, inChannels, sampleRate, bufferSize, nBuffers);
}


float AudioAnalyzer::getVolume()  {
	return volume;
}

float AudioAnalyzer::getSmoothedVolume() {
	 return smoothedVolume;
}

void AudioAnalyzer::audioIn(float* buffer, int bufferSize, int nChannels) {
    float sumSquared = 0;
    for(int i = 0; i < bufferSize; i++) {
        sumSquared += buffer[i] * buffer[i];
    }
    // http://en.wikipedia.org/wiki/Root_mean_square
    float rms = sqrt(sumSquared / bufferSize);
    volume = rms;

	// smoothing
	if (volume >= smoothedVolume) {
		smoothedVolume = volume;
	} else {
		smoothedVolume = ofLerp(smoothedVolume, volume, (1.0-smoothAmount));
	}
}