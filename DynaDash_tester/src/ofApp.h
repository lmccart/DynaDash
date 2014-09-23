#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 8080

class ofApp : public ofBaseApp{
    
public:
    void setup();
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
    
    void sendControlToggleMessage(bool toggle);
    void sendExpressionMessage(float expression);
    void sendDominanceMessage(vector<float> dominance);
    void sendInterruptionMessage(int interruptor);
    
    void exit();
    void guiEvent(ofxUIEventArgs &e);
    
    ofxOscSender sender;
    ofxUICanvas *gui;
    vector<float> dominance;
};
