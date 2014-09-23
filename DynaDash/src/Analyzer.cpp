//
//  Analyzer.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "Analyzer.h"


//Analyzer::Analyzer():
//    db(ofToDataPath("dynadash.sqlite", true),
//       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE) {
//        
//}
//
//Analyzer::~Analyzer() {}

void Analyzer::setup() {
    
    curMode = TRAINING;
	talkTime = vector<float>(4, 0);
	talkRatio = vector<float>(4, 0);
	talkHistory = vector<list<float> >(4, list<float>());

	talkHistoryMinutes = 3;
	showDebug = true;

    audioInput.setup();
    expressionInput.setup();
    feedback.setup();
	debugFeedback.setup(215);
    reset();
}

void Analyzer::update() {

    if (curMode != REMOTE_CONTROL) {
        float curUpdate = ofGetElapsedTimef();
        float elapsed = curUpdate - lastUpdate;
        lastUpdate = curUpdate;
        
        // update inputs
        audioInput.update();
        expressionInput.update();
        feedback.update(audioInput.normalizedVolume);
        debugFeedback.update(audioInput.normalizedVolume, expressionInput.status);
        
        // update talk history arrays and current time
        talkHistoryTime.push_back(curUpdate);
        for(int i = 0; i < 4; i++) {
            float currentDuration = audioInput.speaking[i] * elapsed;
            talkHistory[i].push_back(currentDuration);
            talkTime[i] += currentDuration;
        }

        // remove all time that is too old
        float talkHistorySeconds = talkHistoryMinutes * 60;
        while(curUpdate - talkHistoryTime.front() > talkHistorySeconds) {
            talkHistoryTime.pop_front();
            for(int i = 0; i < 4; i++) {
                talkTime[i] -= talkHistory[i].front();
                talkHistory[i].pop_front();
            }
        }

        // track talk time ratios
        float totalTalkTime = 0;
        for (int i=0; i<4; i++) {
            totalTalkTime += talkTime[i];
        }
        for (int i=0; i<talkTime.size(); i++) {
            talkRatio[i] = talkTime[i]/totalTalkTime;
        }
        
    }
        //ofLogNotice() << talkHistory[0].front() <<"  " << talkHistory[0].back() << " " << talkTime[0];
    // log relevant things to db for end analysis
    if (curMode == RECORDING) {
        try {
            //pend // int nb = db.exec("INSERT INTO test VALUES (NULL, "+ofToString(audioInput.status[0])+")");
        
        
            //        // second row
            //        nb = db.exec("INSERT INTO test VALUES (NULL, \"second\")");
            //
            //        ofLogNotice() << "INSERT INTO test VALUES (NULL, \"second\")\", returned " << nb;
            //
            //        // update the second row
            //        nb = db.exec("UPDATE test SET value=\"second-updated\" WHERE id='2'");
            //
            //        ofLogNotice() << "UPDATE test SET value=\"second-updated\" WHERE id='2', returned " << nb;
            //
            
    //        Check the results : expect two row of result
    //        SQLite::Statement query(db, "SELECT * FROM test");
    //        
    //        ofLogNotice() << "SELECT * FROM test :";
    //        
    //        while (query.executeStep())
    //        {
    //            ofLogNotice() << "row (" << query.getColumn(0) << ", \"" << query.getColumn(1) << ")";
    //        }
            
        } catch (std::exception& e) {
            ofLogError() << "SQLite exception: " << e.what();
        }

    }
}

void Analyzer::draw() {
    feedback.draw(expressionInput.status, audioInput.interrupting, talkRatio);
    
    // reset interruptions after drawn
    if (curMode == REMOTE_CONTROL) {
        for (int i=0; i<4; i++) {
            audioInput.interrupting[i] = 0;
        }
    }
    
	if (showDebug) {
		debugFeedback.draw(audioInput.normalizedVolume, audioInput.speaking);
	}
    //expressionInput.draw();
}

void Analyzer::reset() {
    // reset db
    if (curMode == RECORDING) {
        try {
            //db.exec("DROP TABLE IF EXISTS test"); //pend
            //db.exec("CREATE TABLE test (id INTEGER PRIMARY KEY, value FLOAT)");
            ////db.exec("DROP TABLE test");
        }
        catch (std::exception& e) {
            ofLogError() << "SQLite exception: " << e.what();
        }
    }
    
    for (int i=0; i<talkTime.size(); i++) {
        talkTime[i] = 0;
        talkRatio[i] = 0;
    }
    lastUpdate = ofGetElapsedTimef();
}

void Analyzer::setMode(int mode) {
    ofLogNotice() << "mode set to " << curMode;
    curMode = mode;
    reset();
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
    expressionInput.status[0] = expression;
}


