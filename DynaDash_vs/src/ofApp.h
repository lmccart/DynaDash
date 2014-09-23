#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "Analyzer.h"

// listen on port 12345
#define PORT 8080


class ofApp : public ofBaseApp{

	public:
		void setup();
		void initSettings();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        void exit();
        void guiEvent(ofxUIEventArgs &e);
    
        ofxOscReceiver receiver;
        ofxUICanvas *gui;
        Analyzer analyzer;
};
