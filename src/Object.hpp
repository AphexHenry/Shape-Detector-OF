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
#include "ofxOsc.h"

class Object {
public:
    Object();
    
    void setCenter(ofVec2f aCenter) {mBlobSmoother.setCenter(aCenter);};
    void addPt(float aX, float aY);
    void addPt(ofPoint aPt);
    float getDistance(ofxCvBlob * aBlob, float & aAngleStart);
    void drawShape(float x, float y, int aColor);
    void drawBlob(float x, float y, int index);
    std::vector<ofVec2f> getPts() {return mShape;};
    
//    void newFrameWithBlobs(std::vector<BlobSmoother::Blobito> aBlobArray) {mBlobSmoother.newFrameWithBlobs(aBlobArray);};
    
    int getSoundIndex() {return mSoundIndex;};
    void setSoundIndex(int aSoundIndex) {mSoundIndex = aSoundIndex; updateName();};
    void pushSoundIndex(int aValue) {mSoundIndex += aValue; mSoundIndex = mSoundIndex < 0 ? 0 : mSoundIndex; updateName();};

    ofxOscMessage getOSCMessage();
    
    void startNewFrame(){mBlobSmoother.startNewFrame();};
    void addNewMatch(ofxCvBlob aBlob) {mBlobSmoother.addNewMatch(aBlob);};
    void digestFrame(){mBlobSmoother.digestFrame();};
    
    std::string getName() {return mObjectName;};
    
    bool wantToSendMessage();
    
private:
    
    void updateName();
    
    BlobSmoother mBlobSmoother;
    
    int mSoundIndex;
    std::string mObjectName;
    float getDistanceWithAngle(ofxCvBlob * aBlob, float aAngle);
    std::vector<ofVec2f> mShape;
};

#endif /* Object_hpp */
