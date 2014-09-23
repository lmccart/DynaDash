#pragma once

class LineGraph {
protected:
    ofMesh mesh;
    ofVec2f min, max;
    int n;
    
public:
    LineGraph()
    :n(0) {
    }
    void reset() {
        n = 0;
        min = ofVec2f();
        max = ofVec2f();
    }
    void add(float y) {
        ofVec2f cur(n, y);
        mesh.addVertex(cur);
        if(n == 0) {
            min = cur;
            max = cur;
        } else {
            min.set(MIN(min.x, cur.x), MIN(min.y, cur.y));
            max.set(MAX(max.x, cur.x), MAX(max.y, cur.y));
        }
        n++;
    }
    float getNormalized(float y) {
        return ofNormalize(y, min.y, max.y);
    }
    void draw(float w, float h) {
        ofSetColor(100);
        ofRect(0, 0, w, h);
        ofSetColor(255);
        if(n > 2) {
            ofPushMatrix();
            ofTranslate(0, h);
            ofScale(w / (max.x - min.x), -h / (max.y - min.y));
            ofTranslate(-min);
            mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
            mesh.draw();
            ofPopMatrix();
        }
    }
};