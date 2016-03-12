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
#include "BlobSmoother.hpp"

class Object {
public:
    Object();
    void addPt(float aX, float aY);
    void addPt(ofPoint aPt);
    float getDistance(ofxCvBlob * aBlob, float & aAngleStart);
    void drawShape(float x, float y, int aColor);
    void drawBlob(float x, float y, int index);
    std::vector<ofVec2f> getPts() {return mShape;};
    
    void newFrameWithBlobs(std::vector<BlobSmoother::Blobito> aBlobArray) {mBlobSmoother.newFrameWithBlobs(aBlobArray);};
    
    int getSoundIndex() {return mSoundIndex;};
    void setSoundIndex(int aSoundIndex) {mSoundIndex = aSoundIndex;};
    void pushSoundIndex(int aValue) {mSoundIndex += aValue; mSoundIndex = mSoundIndex < 0 ? 0 : mSoundIndex;};

private:
    
    BlobSmoother mBlobSmoother;
    
    int mSoundIndex;
    float getDistanceWithAngle(ofxCvBlob * aBlob, float aAngle);
    std::vector<ofVec2f> mShape;
};

#endif /* Object_hpp */
