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
    vector<float>volume;
	vector<float>normalizedVolume;
	vector<bool>speaking;
	vector<bool>interrupting;
	float speakingNormalizedThresh;
    ofSoundStream soundStream;
    vector<AudioAnalyzer> mics;

	void setSmoothing(float amt);
    
private:
    int micsInited;
	void analyzeSpeaking();
};