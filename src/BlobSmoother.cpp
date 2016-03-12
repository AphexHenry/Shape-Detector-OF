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
    mShouldSendUpdate = true;
}

//void BlobSmoother::newFrameWithBlobs(std::vector<Blobito> aBlobArray)
//{
//    float lDelta = 0.06f;
//    
//    if(aBlobArray.size())
//    {
//        mIntensity += (1.f - mIntensity) * lDelta;
//    }
//    else
//    {
//        mIntensity += -mIntensity * lDelta;
//        return;
//    }
//
//    ofVec2f lPosNow(0.f, 0.f);
//    float lCoeff = 0.f;
//    for(int i = 0; i < aBlobArray.size(); i++)
//    {
//        float lCoeffThis = 1.f / fmax(aBlobArray[i].distance * aBlobArray[i].distance, 0.0001f);
//        lPosNow.x += aBlobArray[i].pos.x;
//        lPosNow.y += aBlobArray[i].pos.y;
//        lPosNow *= lCoeffThis;
//        lCoeff += lCoeffThis;
//    }
//    
//    lPosNow.x = lPosNow.x / lCoeff;
//    lPosNow.y = lPosNow.y / lCoeff;
//    float lStr = 10.f;
//    mSpeed.x += lStr * (lPosNow.x - mPos.x) * lDelta;
//    mSpeed.y += lStr * (lPosNow.y - mPos.y) * lDelta;
//    mSpeed *= 0.7;
//    
//    mPos.x += mSpeed.x * lDelta;
//    mPos.y += mSpeed.y * lDelta;
//}
//

void BlobSmoother::digestFrame()
{
    int lIndexMin = -1;
    float lDistanceMax = 1000000000.f;
    for(int i = 0; i < mMatchingBlobs.size(); i++)
    {
        float lDistance = mMatchingBlobs[i].centroid.squareDistance(mPos);
        if(lDistance < lDistanceMax)
        {
            lIndexMin = i;
            lDistanceMax = lDistance;
        }
    }
    
    mShouldSendUpdate = true;
    if(lIndexMin >= 0)
    {
        mPos = mMatchingBlobs[lIndexMin].centroid;
        mIntensity += 0.02;
    }
    else // no match
    {
        if(mIntensity < 0.01f)
        {
            mShouldSendUpdate = false;
        }
        mIntensity -= 0.1;
    }
    
    mIntensity = ofClamp(mIntensity, 0.f, 1.f);
    polar(mPos.x - mCenter.x, mPos.y - mCenter.y, mRadius, mAngle);
}

void BlobSmoother::draw(float x = 0, float y = 0, int index = 0)
{
    ofNoFill();
    if(mIntensity > 0.5)
    {
        ofSetHexColor(0xFFFFFF);
    }
    else {
        ofSetHexColor(0x0000FF);
    }
    ofCircle(x + mPos.x, y + mPos.y, 10.f * mIntensity);
    std::stringstream lStr;
    lStr << index;
    ofDrawBitmapString(lStr.str(), x + mPos.x,  y + mPos.y);
}

void BlobSmoother::polar(float x, float y, float& r, float& theta)
{
    r = sqrt(x*x + y*y) / mCenter.x;
    
    if(fabs(x) < 0.0000f)
    {
        if(y > 0.f)
        {
            theta = M_PI_2;
        }
        else
        {
            theta = -M_PI_2;
        }
    }
    else {
        theta = atan2(y,x);
    }
}