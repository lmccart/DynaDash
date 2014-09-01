#include "ofMain.h"

class AudioAnalyzer : public ofBaseApp {
private:
    ofSoundStream input;
    float volume;
public:
    AudioAnalyzer()
    :volume(0) {
    }
    void setup(int deviceID) {
        input.setDeviceID(deviceID);
        int outChannels = 0, inChannels = 1;
        int sampleRate = 44100, bufferSize = 256, nBuffers = 4;
        input.setup(this, outChannels, inChannels, sampleRate, bufferSize, nBuffers);
    }
    float getVolume() {
        return volume;
    }
    void audioIn(float* buffer, int bufferSize, int nChannels) {
        float sumSquared = 0;
        for(int i = 0; i < bufferSize; i++) {
            sumSquared += buffer[i] * buffer[i];
        }
        // http://en.wikipedia.org/wiki/Root_mean_square
        float rms = sqrt(sumSquared / bufferSize);
        volume = rms;
    }
};