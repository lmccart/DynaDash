#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    
    // open an outgoing connection to HOST:PORT
    sender.setup(HOST, PORT);
    
    
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
    gui->setWidth(ofGetWidth());

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
    gui->setHeight(ofGetHeight());
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

void ofApp::sendControlToggleMessage(bool toggle) {
    ofLogNotice() << "sending control toggle message " << toggle;
    ofxOscMessage m;
    m.addIntArg(toggle);
    m.setAddress("/control_toggle");
    sender.sendMessage(m, false);
    
}
void ofApp::sendExpressionMessage(float expression) {
    ofLogNotice() << "sending expression message " << expression;
    ofxOscMessage m;
    m.setAddress("/expression");
    m.addFloatArg(expression);
    sender.sendMessage(m, false);
}

void ofApp::sendDominanceMessage(vector<float> dominance) {
    ofLogNotice() << "sending dominance message";
    ofxOscMessage m;
    m.setAddress("/dominance");
    for (int i=0; i<dominance.size(); i++) {
        m.addFloatArg(dominance[i]);
    }
    sender.sendMessage(m, false);
}

void ofApp::sendInterruptionMessage(int interruptor) {
    ofLogNotice() << "sending interruption message "<< interruptor;
    ofxOscMessage m;
    m.setAddress("/interruption");
    m.addIntArg(interruptor);
    sender.sendMessage(m, false);
}

void ofApp::guiEvent(ofxUIEventArgs &e) {
    
    if (e.getName() == "CONTROL_MODE") {
        int mode = ((ofxUIToggle *) e.widget)->getValue();
        sendControlToggleMessage(mode);
    }
    
    else if (e.getName() == "P1_EXPRESSION") {
        float expression = ((ofxUISlider *) e.widget)->getValue();
        sendExpressionMessage(expression);
    }
    
    else if(e.getName() == "P1_DOMINANCE") {
        dominance[0] = ((ofxUISlider *) e.widget)->getValue();
        sendDominanceMessage(dominance);
    } else if(e.getName() == "P2_DOMINANCE") {
        dominance[1] = ((ofxUISlider *) e.widget)->getValue();
        sendDominanceMessage(dominance);
    } else if(e.getName() == "P3_DOMINANCE") {
        dominance[2] = ((ofxUISlider *) e.widget)->getValue();
        sendDominanceMessage(dominance);
    } else if(e.getName() == "P4_DOMINANCE") {
        dominance[3] = ((ofxUISlider *) e.widget)->getValue();
        sendDominanceMessage(dominance);
    }
    
    else if (e.getName() == "P1_INTERRUPTION") {
        if (((ofxUIButton *)e.widget)->getValue()) {
            sendInterruptionMessage(0);
        }
    } else if (e.getName() == "P2_INTERRUPTION") {
        if (((ofxUIButton *)e.widget)->getValue()) {
            sendInterruptionMessage(1);
        }
    } else if (e.getName() == "P3_INTERRUPTION") {
        if (((ofxUIButton *)e.widget)->getValue()) {
            sendInterruptionMessage(2);
        }
    } else if (e.getName() == "P4_INTERRUPTION") {
        if (((ofxUIButton *)e.widget)->getValue()) {
            sendInterruptionMessage(3);
        }
    }
}
