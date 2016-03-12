//
//  BlobSmoother.hpp
//  shapeDetector
//
//  Created by Baptiste Bohelay on 29/02/2016.
//
//

#ifndef BlobSmoother_hpp
#define BlobSmoother_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxOpenCv.h"

class BlobSmoother
{
public:
    struct Blobito {
        ofVec2f pos;
        float distance;
    };
    
    BlobSmoother();
//    void newFrameWithBlobs(std::vector<Blobito> aBlobArray);
    void draw(float x, float y, int aIndex);
    
    void startNewFrame(){mMatchingBlobs.clear();};
    void addNewMatch(ofxCvBlob aBlob) {mMatchingBlobs.push_back(aBlob);};
    void digestFrame();
    
    float getAngle(){return mAngle;};
    float getRadius(){return mRadius;};
    float getIntensity(){return mIntensity;};
    
    void polar(float x, float y, float& r, float& theta);
    
    bool wantToSendMessage() {return mShouldSendUpdate;};
    
private:
    
    std::vector<ofxCvBlob> mMatchingBlobs;
    
    float mAngle;
    float mRadius;
    float mIntensity;
    
    bool mShouldSendUpdate;
    
    ofVec2f mPos;
    ofVec2f mSpeed;
};

#endif /* BlobSmoother_hpp */
