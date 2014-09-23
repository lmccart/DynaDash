#include "ofMain.h"

class AudioAnalyzer: ofBaseApp {

private:
    ofSoundStream input;
    float volume, smoothedVolume;
public:
	static float smoothAmount;
    void setup(int deviceID);
    float getVolume();
	float getSmoothedVolume();
    void audioIn(float* buffer, int bufferSize, int nChannels);
};