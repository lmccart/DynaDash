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
    totalSmileTime = vector<float>(4,0);
    personSmileTime = vector<vector<float> >(4, vector<float>(4, 0)); // [person][person]
    
    
    // setup things
    audioInput.setup();
    faceInput.setup();
    feedback.setup();
	debugFeedback.setup(215);
    
    // setup printer
    vector<SerialDeviceInfo> printDevices = SerialDeviceUtils::listDevices();
    
	printerAttached = false;
    for (int i=0; i<printDevices.size(); i++) {
                //printDevices[i].getDescription();
		if (printDevices[i].getHardwareId().find("Epson") != -1) {
			if (!printer.setup(printDevices[i].getPort(),
									38400,
									SerialDevice::DATA_BITS_EIGHT,
									SerialDevice::PAR_NONE,
									SerialDevice::STOP_ONE,
									SerialDevice::FLOW_CTRL_HARDWARE)) {
				ofLogError("ofApp::setup") << "Unable to connect to: " << printDevices[i].getPort();
			}
        
        
			// Set up hardware flow control if needed.
			printer.setDataTerminalReady();
			printer.setRequestToSend();
        
			// Initialize the printer.
			printer.initialize();
			printerAttached = true;
			ofLogNotice() << printDevices[i].getDescription() << " " << printDevices[i].getHardwareId() << " " << printDevices[i].getPort();
		}
    }
	
		
	if (!printerAttached) {
		ofLogError() << "Printer not detected";
	}
    
    // setup serial
    cout << "listening for serial data on port " << comPort << "\n";
    serialCom.setup(comPort.c_str(), 250000, 3, 0xFE, 0xFF);
	lastSerialStatsTime = 0;

	modeStart = 0;
	detectDuration = 10.0;
    
    showDebug = true;
    reset();
}

void Analyzer::update() {
    
    handleSerialMessage(serialCom.update());

	// start analysis
	if (curMode == DETECT) {
        faceInput.update();
		detectParticipants(curMode);
		if (ofGetElapsedTimef() - modeStart > detectDuration) {
			ofLogNotice() << ofGetElapsedTimef() << " " << modeStart;
			setMode(ANALYSIS);
		}
	}
    
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
                    if (audioInput.curSpeaker != -1) { // per person
						personSmileTime[i][audioInput.curSpeaker] += elapsed;
					}
					totalSmileTime[i] += elapsed; // total
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
        float groupTalkTime = 0;
        for (int i=0; i<4; i++) {
            if (participantStatus[i]) {
                groupTalkTime += talkTime[i];
            }
        }
		if (groupTalkTime > 0) {
			for (int i=0; i<4; i++) {
				if (participantStatus[i]) {
					talkRatio[i] = talkTime[i]/groupTalkTime;
				}
			}
		}


		// send stats every second while in practice or analysis mode
		float serialElapsed = ofGetElapsedTimef() - lastSerialStatsTime;
		if (serialElapsed > 1.0) {
			
			// add up group smile time
			float groupSmileTime = 0;
			for (int i=0; i<4; i++) {
				groupSmileTime += totalSmileTime[i];
			}
			 
			for (int i=0; i<4; i++) {
				if (participantStatus[i]) {
					stats[i][0] = talkRatio[i]*100 + 0.5;
					stats[i][1] = interruptions[i][0];
					stats[i][2] = groupSmileTime > 0 ? 0.5 + (100*totalSmileTime[i])/groupSmileTime : 0;
				}
			}
	        serialCom.sendStats(stats);
			lastSerialStatsTime += serialElapsed;
		}
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
                setMode(DETECT);
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
	
	ofLogNotice() << "reset";
    
    // clear session history
    for (int i=0; i<4; i++) {
        talkTime[i] = 0;
        talkRatio[i] = 0;
        talkHistory[i].clear();
   
        interruptions[i][0] = 0;
        interruptions[i][1] = 0;
        totalTalkTime[i] = 0;
        for (int j=0; j<4; j++) {
            personSmileTime[i][j] = 0;
        }
		totalSmileTime[i] = 0;
    }
    
    // clear stats
     stats = vector<vector<int> >(4, vector<int>(3, 0));
    
    lastUpdate = ofGetElapsedTimef();
	lastSerialStatsTime = 0;
}

void Analyzer::setMode(int mode) {
	
    if (mode == PRACTICE) {
        detectParticipants(mode);
    }

    if (curMode == ANALYSIS && mode != ANALYSIS) {
        endAnalysisSession();
    }
	
	if (mode == OFF) {
		for (int i=0; i<4; i++) {
			participantStatus[i] = false;
		}
	}
    
	reset();
	modeStart = ofGetElapsedTimef();
    curMode = mode;
    ofLogNotice() << "mode set to " << curMode;
}

void Analyzer::detectParticipants(int mode) {
	if (mode == PRACTICE) {
		for (int i=0; i<4; i++) {
			participantStatus[i] = true;
		}
		serialCom.sendParticipants(participantStatus);
		ofLog() << "Faces detected " << participantStatus[0] << " " << participantStatus[1] << " " << participantStatus[2] << " " << participantStatus[3];
	}

	else if (mode == DETECT) {
		vector<bool> newStatus = faceInput.detectFaces();
		bool changed = false;
		for (int i=0; i<4; i++) {
			if (!participantStatus[i] && newStatus[i]) {
				participantStatus[i] = true;
				changed = true;
			}
		}
		// ofLogNotice() << "detected " << participantStatus[0] << " " << newStatus[0] << " " <<  faceInput.detectFaces()[0];
		if (changed) {
			serialCom.sendParticipants(participantStatus);
			ofLog() << "Faces detected " << participantStatus[0] << " " << participantStatus[1] << " " << participantStatus[2] << " " << participantStatus[3];
		}
	}
}


void Analyzer::endAnalysisSession() {
    
    ofLogNotice() << "Processing end of conversation";

	float sessionTime = ofGetElapsedTimef() - modeStart;
    
    // figure out most and least
    int mostInterrupting = 0;
    int mostInterruptingVal = 0;
    int leastInterrupting = -1;
    int leastInterruptingVal = 0;
    
    int mostInterrupted = 0;
    int mostInterruptedVal = 0;
    int leastInterrupted = -1;
    int leastInterruptedVal = 0;
    
    int mostSpeaking = 0;
    float mostSpeakingVal = 0;
    int leastSpeaking = -1;
    float leastSpeakingVal = 0;
    
    int mostSmiling = 0;
    float mostSmilingVal = 0;
    int leastSmiling = -1;
    float leastSmilingVal = 0;
    
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
            if (totalSmileTime[i] > mostSmilingVal) {
                mostSmiling = i;
                mostSmilingVal = totalSmileTime[i];
            }
            else if (totalSmileTime[i] < leastSmilingVal || leastSmiling == -1) {
                leastSmiling = i;
                leastSmilingVal = totalSmileTime[i];
            }
            
        }
    }
    
    vector<string> labels = vector<string>(4, "");
    
    for (int i=0; i<4; i++) {
		if (true) {
        //if (participantStatus[i]) {
        
            std::stringstream ss;
            ss << "results" << i << ".txt";
            ofFile file(ss.str(), ofFile::WriteOnly);

			
            std::stringstream results;

            labels[0] = "Person A";
            labels[1] = "Person B";
            labels[2] = "Person C";
            labels[3] = "Person D";
            labels[i] = "You";
            
            // INTERRUPTED
            results << "You interrupted people " << interruptions[i][0] << getTimes(interruptions[i][0]);
            if (i == mostInterrupting) results << ",\nmore than anyone.\n";
            else if (i == leastInterrupting) results << ",\nless than anyone.\n";
            else results << ".\n";
            
            if (mostInterrupting != -1 && mostInterrupting != i)
                results << labels[mostInterrupting] << " interrupted the most (" << mostInterruptingVal << getTimes(mostInterruptingVal) << ").\n";
            if (leastInterrupting != -1 && leastInterrupting != i)
                results << labels[leastInterrupting] << " interrupted the least (" << leastInterruptingVal << getTimes(leastInterruptingVal) << ").\n";
            
            // WAS INTERRUPTED
            results << "\nYou were interrupted " << interruptions[i][1] << getTimes(interruptions[i][1]);
            if (i == mostInterrupted) results << ",\nmore than anyone.\n";
            else if (i == leastInterrupted) results << ",\nless than anyone.\n";
            else results << ".\n";
            
            if (mostInterrupted != -1 && mostInterrupted != i) {
                results << labels[mostInterrupted] << " was interrupted the most (" << mostInterruptedVal << getTimes(mostInterruptedVal) << ").\n";
            }
            if (leastInterrupted != -1 && leastInterrupted != i) {
                results << labels[leastInterrupted] << " was interrupted the least (" << leastInterruptedVal << getTimes(leastInterruptingVal) << ").\n";
            }
            
            // TALKED
            if (groupTotalTalk > 0) {
                results << "\nYou were talking " << int(100*totalTalkTime[i]/groupTotalTalk) << "% of the time";
                if (i == mostSpeaking) results << ",\nmore than anyone.\n";
                else if (i == leastSpeaking) results << ",\nless than anyone.\n";
                else results << ".\n";
                
                if (mostSpeaking != -1 && mostSpeaking != i)
                    results << labels[mostSpeaking] << " talked the most (" << int(100*mostSpeakingVal/groupTotalTalk) << "% of the time).\n";
                if (leastSpeaking != -1 && leastSpeaking != i)
                    results << labels[leastSpeaking] << " talked the least (" << int(100*leastSpeakingVal/groupTotalTalk) << "% of the time).\n";
            }
            
            // SMILED
            results << "\nYou were smiling " << int(100*totalSmileTime[i]/sessionTime) << "% of the time";
            if (i == mostSmiling) results << ",\nmore than anyone.\n";
            else if (i == leastSmiling) results << ",\nless than anyone.\n";
            else results << ".\n";
                
            if (mostSmiling != -1 && mostSmiling != i)
                results << labels[mostSmiling] << " smiled the most (" << int(100*mostSmilingVal/sessionTime) << "% of everyone).\n";
            if (leastSmiling != -1 && leastSmiling != i)
                results << labels[leastSmiling] << " smiled the least (" << int(100*leastSmilingVal/sessionTime) << "% of everyone).\n";
            //results << "\n"; // tear off clearance

			// write to file
			file << results.str();
            file.close();
			
			// print
			ofLogNotice() << "PRINTING";
			printer.println(results.str());
			printer.cut();
        }
    }
}

string Analyzer::getTimes(int val) {
	if (val == 1) {
		return " time";
	} else {
		return " times";
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


