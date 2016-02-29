//
//  Object.hpp
//  shapeDetector
//
//  Created by Baptiste Bohelay on 28/02/2016.
//
//

#ifndef Object_hpp
#define Object_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxOpenCv.h"

class Object {
public:
    Object();
    void addPt(float aX, float aY);
    void addPt(ofPoint aPt);
    float getDistance(ofxCvBlob * aBlob, float & aAngleStart);
    void draw(float x, float y, int aColor);
    std::vector<ofVec2f> getPts() {return mShape;};
    
    int getSoundIndex() {return mSoundIndex;};
    void setSoundIndex(int aSoundIndex) {mSoundIndex = aSoundIndex;};

private:
    
    int mSoundIndex;
    float getDistanceWithAngle(ofxCvBlob * aBlob, float aAngle);
    std::vector<ofVec2f> mShape;
};

#endif /* Object_hpp */
