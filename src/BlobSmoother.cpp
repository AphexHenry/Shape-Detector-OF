//
//  BlobSmoother.cpp
//  shapeDetector
//
//  Created by Baptiste Bohelay on 29/02/2016.
//
//

#include "BlobSmoother.hpp"

BlobSmoother::BlobSmoother()
{
    mIntensity = 0.f;
}

void BlobSmoother::newFrameWithBlobs(std::vector<Blobito> aBlobArray)
{
    float lDelta = 0.06f;
    
    if(aBlobArray.size())
    {
        mIntensity += (1.f - mIntensity) * lDelta;
    }
    else
    {
        mIntensity += -mIntensity * lDelta;
        return;
    }

    ofVec2f lPosNow(0.f, 0.f);
    float lCoeff = 0.f;
    for(int i = 0; i < aBlobArray.size(); i++)
    {
        float lCoeffThis = 1.f / fmax(aBlobArray[i].distance * aBlobArray[i].distance, 0.0001f);
        lPosNow.x += aBlobArray[i].pos.x;
        lPosNow.y += aBlobArray[i].pos.y;
        lPosNow *= lCoeffThis;
        lCoeff += lCoeffThis;
    }
    
    lPosNow.x = lPosNow.x / lCoeff;
    lPosNow.y = lPosNow.y / lCoeff;
    float lStr = 10.f;
    mSpeed.x += lStr * (lPosNow.x - mPos.x) * lDelta;
    mSpeed.y += lStr * (lPosNow.y - mPos.y) * lDelta;
    mSpeed *= 0.7;
    
    mPos.x += mSpeed.x * lDelta;
    mPos.y += mSpeed.y * lDelta;
}

void BlobSmoother::draw(float x = 0, float y = 0, int index = 0)
{
    ofNoFill();
    ofSetHexColor(0x0000FF);
    ofCircle(x + mPos.x, y + mPos.y, 10.f * mIntensity);
    std::stringstream lStr;
    lStr << index;
    ofDrawBitmapString(lStr.str(), x + mPos.x,  y + mPos.y);
}