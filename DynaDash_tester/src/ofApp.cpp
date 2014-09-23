#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    
    //sender.setup(HOST, PORT);
    ofShowCursor();
    //    gui = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());           // create a canvas at (0,0) using the default width
    //    gui->addLabelToggle("RECORDING", false);
    //    gui->autoSizeToFitWidgets();
    //    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    
    ofSetVerticalSync(true);
    ofSetFrameRate(120);
    ofEnableSmoothing();
    
    hideable = vector<ofxUIWidget *>();
    
    dominance = vector<float>(4, 0);
    
    gui = new ofxUISuperCanvas("TEST CONTROLLER");
    gui->setWidth(ofGetWidth());

    ofxUISpacer *spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    gui->addTextInput("HOST", "");
    gui->addButton("CONNECT", false);
    
    spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    hideable.push_back(gui->addLabelToggle("REMOTE CONTROL MODE", false));
    
    spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    hideable.push_back(gui->addSlider("P1 EXPRESSION", 0, 1.0, 0.0));
    
    spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    hideable.push_back(gui->addSlider("P1 DOMINANCE", 0, 1.0, 0.0));
    hideable.push_back(gui->addSlider("P2 DOMINANCE", 0, 1.0, 0.0));
    hideable.push_back(gui->addSlider("P3 DOMINANCE", 0, 1.0, 0.0));
    hideable.push_back(gui->addSlider("P4 DOMINANCE", 0, 1.0, 0.0));
    
    spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    hideable.push_back(gui->addButton("P1 INTERRUPTION", true));
    hideable.push_back(gui->addButton("P2 INTERRUPTION", true));
    hideable.push_back(gui->addButton("P3 INTERRUPTION", true));
    hideable.push_back(gui->addButton("P4 INTERRUPTION", true));
    
    spacer = gui->addSpacer(400, 15);
    spacer->setColorFill(ofColor(120));
    
    for (int i=0; i<hideable.size(); i++) {
        hideable[i]->setVisible(false);
    }
    
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
    
    if (e.getName() == "CONNECT") {
        
        if (((ofxUIButton *)e.widget)->getValue()) {
            string host = ((ofxUITextInput *)gui->getWidget("HOST"))->getTextString();
            
            if (host.length() == 0) {
                host = HOST;
            }
            sender.setup(host, PORT);
            ofLogNotice() << "connected to " << host << ":" << PORT;
            
            // show hidden buttons
            for (int i=0; i<hideable.size(); i++) {
                hideable[i]->setVisible(true);
            }
        }
    }
    
    else if (e.getName() == "REMOTE CONTROL MODE") {
        float value = ((ofxUIToggle *) e.widget)->getValue();
        sendControlToggleMessage(value);
    }
    
    else if (e.getName() == "P1 EXPRESSION") {
        float expression = ((ofxUISlider *) e.widget)->getValue();
        sendExpressionMessage(expression);
    }
    
    else if(e.getName() == "P1 DOMINANCE") {
        dominance[0] = ((ofxUISlider *) e.widget)->getValue();
        sendDominanceMessage(dominance);
    } else if(e.getName() == "P2 DOMINANCE") {
        dominance[1] = ((ofxUISlider *) e.widget)->getValue();
        sendDominanceMessage(dominance);
    } else if(e.getName() == "P3 DOMINANCE") {
        dominance[2] = ((ofxUISlider *) e.widget)->getValue();
        sendDominanceMessage(dominance);
    } else if(e.getName() == "P4 DOMINANCE") {
        dominance[3] = ((ofxUISlider *) e.widget)->getValue();
        sendDominanceMessage(dominance);
    }
    
    else if (e.getName() == "P1 INTERRUPTION") {
        if (((ofxUIButton *)e.widget)->getValue()) {
            sendInterruptionMessage(0);
        }
    } else if (e.getName() == "P2 INTERRUPTION") {
        if (((ofxUIButton *)e.widget)->getValue()) {
            sendInterruptionMessage(1);
        }
    } else if (e.getName() == "P3 INTERRUPTION") {
        if (((ofxUIButton *)e.widget)->getValue()) {
            sendInterruptionMessage(2);
        }
    } else if (e.getName() == "P4 INTERRUPTION") {
        if (((ofxUIButton *)e.widget)->getValue()) {
            sendInterruptionMessage(3);
        }
    }
}
