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

class BlobSmoother
{
public:
    struct Blobito {
        ofVec2f pos;
        float distance;
    };
    
    BlobSmoother();
    void newFrameWithBlobs(std::vector<Blobito> aBlobArray);
    void draw(float x, float y, int aIndex);
    
private:
    ofVec2f mPos;
    ofVec2f mSpeed;
    float mIntensity;
};

#endif /* BlobSmoother_hpp */
