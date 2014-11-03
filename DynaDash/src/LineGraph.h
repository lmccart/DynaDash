#pragma once

class LineGraph {
protected:
    ofMesh mesh;
    ofVec2f min, max;
    int index, maxSize;
    
public:
    LineGraph()
    :index(0), maxSize(512) {
    }
    void setMaxSize(int maxSize) {
        this->maxSize = maxSize;
    }
    void reset() {
        index = 0;
        min = ofVec2f();
        max = ofVec2f();
    }
    int size() const {
        return mesh.getNumVertices();
    }
    void add(float y) {
        ofVec2f cur(index, y);
        if(size() < maxSize) {
            mesh.addVertex(cur);
        } else {
            mesh.setVertex(index, cur);
        }
        if(size() == 0) {
            min = cur;
            max = cur;
        } else {
            min.set(MIN(min.x, cur.x), MIN(min.y, cur.y));
            max.set(MAX(max.x, cur.x), MAX(max.y, cur.y));
        }
        index = (index + 1) % maxSize;
    }
    float getNormalized(float y) {
        return ofNormalize(y, min.y, max.y);
    }
    void draw(float w, float h) {
        ofSetColor(100);
        ofRect(0, 0, w, h);
        ofSetColor(255);
        if(size() > 2) {
            ofPushMatrix();
            ofTranslate(0, h);
            ofScale(w / maxSize, -h / (max.y - min.y));
            ofTranslate(-min);
            mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
            mesh.draw();
            ofPopMatrix();
        }
    }
};