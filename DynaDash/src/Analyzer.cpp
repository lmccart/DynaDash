//
//  Analyzer.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "Analyzer.h"


Analyzer::Analyzer():
    db(ofToDataPath("dynadash.sqlite", true),
       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE) {
        
}

Analyzer::~Analyzer() {}

void Analyzer::setup() {
    
    curMode = TRAINING;
    audioInput.setup();
    videoInput.setup();
    feedback.setup();
    reset();
}

void Analyzer::update() {

    float elapsed = ofGetElapsedTimef() - lastUpdate;
    lastUpdate = ofGetElapsedTimef();
    
    // update inputs
    audioInput.update();
    videoInput.update();
    
    // track talk time ratios
    float totalTalkTime = 0;
    for (int i=0; i<4; i++) {
        talkTime[i] += audioInput.status[i]*elapsed;
        totalTalkTime += talkTime[i];
    }
    for (int i=0; i<4; i++) {
        talkRatio[i] = talkTime[i]/totalTalkTime;
        
    }
    
    // log relevant things to db for end analysis
    if (curMode == RECORDING) {
        try {
            int nb = db.exec("INSERT INTO test VALUES (NULL, "+ofToString(audioInput.status[0])+")");
        
        
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
    feedback.draw(videoInput.status, audioInput.status, talkRatio);
}

void Analyzer::reset() {
    // reset db
    if (curMode == RECORDING) {
        try {
            db.exec("DROP TABLE IF EXISTS test");
            db.exec("CREATE TABLE test (id INTEGER PRIMARY KEY, value FLOAT)");
            //db.exec("DROP TABLE test");
        }
        catch (std::exception& e) {
            ofLogError() << "SQLite exception: " << e.what();
        }
    }
    
    for (int i=0; i<4; i++) {
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
