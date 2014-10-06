#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "Analyzer.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void initSettings();
		void update();
		void draw();
        void exit();
		void keyPressed(int key);
        void guiEvent(ofxUIEventArgs &e);
    
        ofxXmlSettings xml;
        ofxOscReceiver receiver;
        ofxUICanvas *gui;
        ofxUILabel *remoteControlLabel;
        Analyzer analyzer;
};
