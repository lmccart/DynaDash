#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofShowCursor();
    //    gui = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());           // create a canvas at (0,0) using the default width
    //    gui->addLabelToggle("RECORDING", false);
    //    gui->autoSizeToFitWidgets();
    //    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    
    ofSetVerticalSync(true);
    ofSetFrameRate(120);
    ofEnableSmoothing();
    
    
    dominance = vector<float>(4, 0);
    
    gui = new ofxUISuperCanvas("TEST CONTROLLER");
    gui->setWidth(400);

    ofxUISpacer *spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    gui->addLabelToggle("CONTROL_MODE", false);
    
    spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    gui->addSlider("P1_EXPRESSION", 0, 1.0, 0.0);
    
    spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    gui->addSlider("P1_DOMINANCE", 0, 1.0, 0.0);
    gui->addSlider("P2_DOMINANCE", 0, 1.0, 0.0);
    gui->addSlider("P3_DOMINANCE", 0, 1.0, 0.0);
    gui->addSlider("P4_DOMINANCE", 0, 1.0, 0.0);
    
    spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    gui->addButton("P1_INTERRUPTION", true);
    gui->addButton("P2_INTERRUPTION", true);
    gui->addButton("P3_INTERRUPTION", true);
    gui->addButton("P4_INTERRUPTION", true);
    
    spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
}


//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == ' ') {
        
    }
    ofLogNotice() << "key pressed: " << key;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
    
}

void ofApp::exit() {
    delete gui;
}

void ofApp::guiEvent(ofxUIEventArgs &e) {
    
    if (e.getName() == "CONTROL_MODE") {
        int mode = ((ofxUIToggle *) e.widget)->getValue();
        
    }
    
    else if (e.getName() == "P1_EXPRESSION") {
        float expression = ((ofxUISlider *) e.widget)->getValue();
    }
    
    else if(e.getName() == "P1_DOMINANCE") {
        dominance[0] = ((ofxUISlider *) e.widget)->getValue();
    } else if(e.getName() == "P2_DOMINANCE") {
        dominance[1] = ((ofxUISlider *) e.widget)->getValue();
    } else if(e.getName() == "P3_DOMINANCE") {
        dominance[2] = ((ofxUISlider *) e.widget)->getValue();
    } else if(e.getName() == "P4_DOMINANCE") {
        dominance[3] = ((ofxUISlider *) e.widget)->getValue();
    }
    
    else if (e.getName() == "P1_INTERRUPTION") {
        
    } else if (e.getName() == "P2_INTERRUPTION") {
        
    } else if (e.getName() == "P3_INTERRUPTION") {
        
    } else if (e.getName() == "P4_INTERRUPTION") {
        
    }
}
