#include "ofMain.h"

#include "AudioAnalyzer.h"

class ofApp : public ofBaseApp {
public:
    AudioAnalyzer mic0, mic1;
    
	void setup() {
        // looking for C-Media USB Audio Device
        ofSoundStream deviceLister;
        deviceLister.listDevices();
        
        // happens to be on device 3 and 4
        mic0.setup(3);
        mic1.setup(4);
	}
	void draw() {
        ofBackground(0);
        ofSetLineWidth(3);
        
        float radius = 64;
        
        ofTranslate(0, 128);
        
        ofTranslate(128, 0);
        ofFill(); ofSetColor(255);
        ofCircle(0, 0, radius * mic0.getVolume());
        ofNoFill(); ofSetColor(128);
        ofCircle(0, 0, radius);
        
        ofTranslate(256, 0);
        ofFill(); ofSetColor(255);
        ofCircle(0, 0, radius * mic1.getVolume());
        ofNoFill(); ofSetColor(128);
        ofCircle(0, 0, radius);
	}
};

int main() {
	ofSetupOpenGL(512, 256, OF_WINDOW);
	ofRunApp(new ofApp());
}
