//
//  Analyzer.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "Analyzer.h"

using namespace ofx;
using namespace IO;
using namespace ESCPOS;



void Analyzer::setup(string comPort) {

    
    curMode = OFF;
    smileThresh = 0.5; // pend, might need to be pp
    
    stats = vector<vector<int> >(4, vector<int>(3, 0)); // [person][dominance, interruptions, expression]
    participantStatus = vector<bool>(4, false);

    
    // floating history
    talkHistoryMinutes = 3;
	talkTime = vector<float>(4, 0);
	talkRatio = vector<float>(4, 0);
	talkHistory = vector<list<float> >(4, list<float>());

    // session history
    interruptions = vector<vector<int> >(4, vector<int>(2, 0)); // [person][giving, receiving]
    totalTalkTime = vector<float>(4,0);
    totalSmileTime = vector<vector<float> >(4, vector<float>(4, 0)); // [person][person]
    
    
    // setup things
    audioInput.setup();
    faceInput.setup();
    feedback.setup();
	debugFeedback.setup(215);
    
    // setup printers
    printers = vector<DefaultSerialPrinter>(4);
    vector<SerialDeviceInfo> printDevices = SerialDeviceUtils::listDevices();
    
    if (printDevices.size() < 4) {
        ofLogError() << "Detected " << printDevices.size() << " printers instead of 4.";
    }
    
    for (int i=0; i<printDevices.size(); i++) {
                //printDevices[i].getDescription();
//                if (!printers[i].setup(printDevices[i].getPort(),
//                                       38400,
//                                       SerialDevice::DATA_BITS_EIGHT,
//                                       SerialDevice::PAR_NONE,
//                                       SerialDevice::STOP_ONE,
//                                       SerialDevice::FLOW_CTRL_HARDWARE)) {
//                    ofLogError("ofApp::setup") << "Unable to connect to: " << printDevices[i].getPort();
//                }
//        
//        
//                // Set up hardware flow control if needed.
//                printers[i].setDataTerminalReady();
//                printers[i].setRequestToSend();
//        
//                // Initialize the printer.
//                printers[i].initialize();
        ofLogNotice() << printDevices[i].getDescription() << " " << printDevices[i].getHardwareId() << " " << printDevices[i].getPort();
    }
    
    // setup serial
    cout << "listening for serial data on port " << comPort << "\n";
    serialCom.setup(comPort.c_str(), 57600, 15, 0xFE, 0xFF);
    
    showDebug = true;
    reset();
}

void Analyzer::update() {
    
    handleSerialMessage(serialCom.update());

    
    if (curMode == PRACTICE || curMode == ANALYSIS) {
        float curUpdate = ofGetElapsedTimef();
        float elapsed = curUpdate - lastUpdate;
        lastUpdate = curUpdate;
        
        // update inputs
        audioInput.update();
        faceInput.update();
        feedback.update(audioInput.normalizedVolume);
        debugFeedback.update(audioInput.normalizedVolume, faceInput.status);
        
        // update talk history arrays and current time
        // update smile time
        talkHistoryTime.push_back(curUpdate);
        for(int i = 0; i < 4; i++) {
            if (participantStatus[i]) {
                float audioDuration = audioInput.curSpeaker == i ? elapsed : 0.0;
                talkHistory[i].push_back(audioDuration);
                talkTime[i] += audioDuration;
                
                // session history stats
                totalTalkTime[i] += audioDuration;
                if (faceInput.status[i] > smileThresh) {
                    float ind = audioInput.curSpeaker == -1 ? i : audioInput.curSpeaker; // stick it in self if no cur speaker
                    totalSmileTime[i][ind] += elapsed;
                } else {
                }
                
                // check for interrupting
                if (audioInput.interrupting[i]) {
                    interruptions[i][0] += 1;
                }
                
                
                // check for interrupted
                if (i == audioInput.curSpeaker) {
                    for (int j=0; j<4; j++) {
                        if (j != i) {
                            int val = audioInput.interrupting[j] ? 1 : 0;
                            interruptions[i][1] += val;
                        }
                    }
                }
            }
        }

        // remove all time that is too old
        float talkHistorySeconds = talkHistoryMinutes * 60;
        while(curUpdate - talkHistoryTime.front() > talkHistorySeconds) {
            talkHistoryTime.pop_front();
            for(int i = 0; i < 4; i++) {
                if (participantStatus[i]) {
                    talkTime[i] -= talkHistory[i].front();
                    talkHistory[i].pop_front();
                }
            }
        }

        // track talk time ratios
        float totalTalkTime = 0;
        for (int i=0; i<4; i++) {
            if (participantStatus[i]) {
                totalTalkTime += talkTime[i];
            }
        }
        for (int i=0; i<4; i++) {
            if (participantStatus[i]) {
                talkRatio[i] = talkTime[i]/totalTalkTime;
            }
        }
        
        // send stats
        for (int i=0; i<4; i++) {
            if (participantStatus[i]) {
                stats[i][0] = talkRatio[i];
                stats[i][1] = interruptions[i][0];
                stats[i][2] = faceInput.status[i] > smileThresh ? 1 : 0;
            }
        }
        serialCom.sendStats(stats);
    }
}

void Analyzer::handleSerialMessage(int msg) {
    if (msg != -1) {
        switch (msg) {
            case 0:
                ofLogNotice() << "Received message: begin practice session";
                setMode(PRACTICE);
                break;
            case 1:
                ofLogNotice() << "Received message: begin analysis session";
                setMode(ANALYSIS);
                break;
            case 4:
                ofLogNotice() << "Received message: end practice session";
                setMode(OFF);
                break;
            case 5:
                ofLogNotice() << "Received message: end analysis session";
                setMode(OFF);
                break;
            default:
                ofLogNotice() << "Received unknown message: " << msg;
                break;
        }
        
    }
}

void Analyzer::draw() {
    feedback.draw(participantStatus, faceInput.status, audioInput.interrupting, talkRatio);
    
    // reset interruptions after drawn
    if (curMode == REMOTE_CONTROL) {
        for (int i=0; i<4; i++) {
            audioInput.interrupting[i] = 0;
        }
    }
    
	if (showDebug) {
		debugFeedback.draw(participantStatus, audioInput.normalizedVolume, audioInput.speaking);
	}
    //expressionInput.draw();
}

void Analyzer::reset() {

    for (int i=0; i<4; i++) {
        talkTime[i] = 0;
        talkRatio[i] = 0;
        talkHistory[i].clear();
    }
    
    // clear session history
    for (int i=0; i<4; i++) {
        interruptions[i][0] = 0;
        interruptions[i][1] = 0;
        totalTalkTime[i] = 0;
        for (int j=0; j<4; j++) {
            totalSmileTime[i][j] = 0;
        }
    }
    
    // clear stats
     stats = vector<vector<int> >(4, vector<int>(3, 0));
    
    lastUpdate = ofGetElapsedTimef();
}

void Analyzer::setMode(int mode) {
    
    if (mode == PRACTICE || mode == ANALYSIS) {
        beginSession();
    }
    
    if (curMode == ANALYSIS && mode != ANALYSIS) {
        endAnalysisSession();
    }
    
    curMode = mode;
    reset();
    ofLogNotice() << "mode set to " << curMode;
    
}

void Analyzer::beginSession() {
    participantStatus = faceInput.detectFaces();
    serialCom.sendParticipants(participantStatus);
    for (int i=0; i<4; i++) {
        ofLog() << i << " " << participantStatus[i];
    }
}


void Analyzer::endAnalysisSession() {
    
    ofLogNotice() << "Processing end of conversation";
    
    // figure out most and least
    int mostInterrupting = -1;
    int mostInterruptingVal = 0;
    int leastInterrupting = -1;
    int leastInterruptingVal = 0;
    
    int mostInterrupted = -1;
    int mostInterruptedVal = 0;
    int leastInterrupted = -1;
    int leastInterruptedVal = 0;
    
    int mostSpeaking = -1;
    float mostSpeakingVal = 0;
    int leastSpeaking = -1;
    float leastSpeakingVal = 0;
    
    int mostSmiling = -1;
    float mostSmilingVal = 0;
    int leastSmiling = -1;
    float leastSmilingVal = 0;
    
    vector<float> individualTotalSmile = vector<float>(4, 0);
    float groupTotalTalk = 0;
    float groupTotalSmile = 0;
    
    for (int i=0; i<4; i++) {
        if (participantStatus[i]) {
        
            // interrupting
            if (interruptions[i][0] > mostInterruptingVal) {
                mostInterrupting = i;
                mostInterruptingVal = interruptions[i][0];
            }
            else if (interruptions[i][0] < leastInterruptingVal || leastInterrupting == -1) {
                leastInterrupting = i;
                leastInterruptingVal = interruptions[i][0];
            }
            
            // interrupted
            if (interruptions[i][1] > mostInterruptedVal) {
                mostInterrupted = i;
                mostInterruptedVal = interruptions[i][1];
            }
            else if (interruptions[i][1] < leastInterruptedVal || leastInterrupted == -1) {
                leastInterrupted = i;
                leastInterruptedVal = interruptions[i][1];
            }
            
            // talking
            if (totalTalkTime[i] > mostSpeakingVal) {
                mostSpeaking = i;
                mostSpeakingVal = totalTalkTime[i];
            }
            else if (totalTalkTime[i] < leastSpeakingVal || leastSpeaking == -1) {
                leastSpeaking = i;
                leastSpeakingVal = totalTalkTime[i];
            }
            groupTotalTalk += totalTalkTime[i];
            
            
            // smiling
            for (int j=0; j<4; j++) {
                if (participantStatus[j]) {
                    individualTotalSmile[i] += totalSmileTime[i][j];
                }
            }
            
            if (individualTotalSmile[i] > mostSmilingVal) {
                mostSmiling = i;
                mostSmilingVal = individualTotalSmile[i];
            }
            else if (individualTotalSmile[i] < leastSmilingVal || leastSmiling == -1) {
                leastSmiling = i;
                leastSmilingVal = individualTotalSmile[i];
            }
            
            groupTotalSmile += individualTotalSmile[i];
        }
    }
    
    vector<string> labels = vector<string>(4, "");
    
    for (int i=0; i<4; i++) {
        if (participantStatus[i]) {
        
            std::stringstream ss;
            ss << "results" << i << ".txt";
            ofFile file(ss.str(), ofFile::WriteOnly);
            
            labels[0] = "Person A";
            labels[1] = "Person B";
            labels[2] = "Person C";
            labels[3] = "Person D";
            labels[i] = "You";
            
            // INTERRUPTED
            file << "You interrupted people " << interruptions[i][0] << " times";
            if (i == mostInterrupting) file << ", the most of anyone.\n";
            else if (i == leastInterrupting) file << ", the least of anyone.\n";
            else file << ".\n";
            
            if (mostInterrupting != -1 && mostInterrupting != i)
                file << labels[mostInterrupting] << " interrupted the most (" << mostInterruptingVal << " times).\n";
            if (leastInterrupting != -1 && leastInterrupting != i)
                file << labels[leastInterrupting] << " interrupted the least (" << leastInterruptingVal << " times).\n";
            
            // WAS INTERRUPTED
            file << "\nYou were interrupted " << interruptions[i][1] << " times";
            if (i == mostInterrupted) file << ", the most of anyone.\n";
            else if (i == leastInterrupted) file << ", the least of anyone.\n";
            else file << ".\n";
            
            if (mostInterrupted != -1 && mostInterrupted != i) {
                file << labels[mostInterrupted] << " was interrupted the most (" << mostInterruptedVal << " times).\n";
            }
            if (leastInterrupted != -1 && leastInterrupted != i) {
                file << labels[leastInterrupted] << " was interrupted the least (" << leastInterruptedVal << " times).\n";
            }
            
            // TALKED
            if (groupTotalTalk > 0) {
                file << "\nYou were talking " << int(100*totalTalkTime[i]/groupTotalTalk) << "% of the time";
                if (i == mostSpeaking) file << ", the most of anyone.\n";
                else if (i == leastSpeaking) file << ", the least of anyone.\n";
                else file << ".\n";
                
                if (mostSpeaking != -1 && mostSpeaking != i)
                    file << labels[mostSpeaking] << " talked the most (" << int(100*mostSpeakingVal/groupTotalTalk) << "% of the time).\n";
                if (leastSpeaking != -1 && leastSpeaking != i)
                    file << labels[leastSpeaking] << " talked the least (" << int(100*leastSpeakingVal/groupTotalTalk) << "% of the time).\n";
            }
            
            // SMILED
            if (groupTotalSmile > 0) {
                file << "\nYou were smiling " << int(100*individualTotalSmile[i]/groupTotalSmile) << "% of the time";
                if (i == mostSpeaking) file << ", the most of anyone.\n";
                else if (i == leastSpeaking) file << ", the least of anyone.\n";
                else file << ".\n";
                
                if (mostSmiling != -1 && mostSmiling != i)
                    file << labels[mostSmiling] << " smiled the most (" << int(100*mostSmilingVal/groupTotalSmile) << "% of the time).\n";
                if (leastSmiling != -1 && leastSmiling != i)
                    file << labels[leastSmiling] << " smiled the least (" << int(100*leastSmilingVal/groupTotalSmile) << "% of the time).\n";
            }
            
            file.close();
        }
    }
}


// remote control
void Analyzer::setDominance(float d0, float d1, float d2, float d3) {
    talkRatio[0] = d0;
    talkRatio[1] = d1;
    talkRatio[2] = d2;
    talkRatio[3] = d3;
}

void Analyzer::setInterruption(int i) {
    audioInput.interrupting[i] = true;
}
void Analyzer::setExpression(float expression) {
    faceInput.status[0] = expression;
}


