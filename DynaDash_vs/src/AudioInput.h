//
//  AudioInput.h
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//
#include "ofMain.h"
#include "AudioAnalyzer.h"


class AudioInput {
    
public:
    void setup();
    void update();
    vector<float>lastVolume;
    vector<float>volume;
	vector<float>normalizedVolume;
	vector<bool>speaking;
	float smoothAmt;
	float speakingNormalizedThresh;
    ofSoundStream soundStream;
    AudioAnalyzer mics[4];
    
private:
	void analyzeVolumes();
};