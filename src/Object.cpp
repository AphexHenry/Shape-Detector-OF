//
//  Object.cpp
//  shapeDetector
//
//  Created by Baptiste Bohelay on 28/02/2016.
//
//

#include "Object.hpp"

Object::Object(){
    mSoundIndex = 0;
}

void Object::addPt(float aX, float aY)
{
    mShape.push_back(ofPoint(aX, aY));
}

void Object::addPt(ofPoint aPt)
{
    mShape.push_back(aPt);
}

float Object::getDistanceWithAngle(ofxCvBlob * aBlob, float aAngle = 0.f)
{
    float lDistanceTotal = 0.f;
    for(int pti = 0; pti < aBlob->nPts; pti++)
    {
        ofPoint lPointIn = aBlob->pts[pti];
        lPointIn.x -= aBlob->centroid.x;
        lPointIn.y -= aBlob->centroid.y;
        
        float lMinDis = 1000000.f;

        for(int pto = 0; pto < mShape.size(); pto += 2)
        {
            ofVec2f lPointThis = mShape[pto].getRotatedRad(aAngle);
            float lDistanceX = lPointIn.x - lPointThis.x;
            float lDistanceY = lPointIn.y - lPointThis.y;
            float lDistance = lDistanceX * lDistanceX + lDistanceY * lDistanceY;
            if(lMinDis > lDistance)
            {
                lMinDis = lDistance;
            }
        }

        lDistanceTotal += lMinDis;
    }
    
    lDistanceTotal = lDistanceTotal / aBlob->nPts;
    return lDistanceTotal;
}

float Object::getDistance(ofxCvBlob * aBlob, float & aAngleStart)
{
    int aAngleTestCount = 40;
    float lDistanceMin = 100000.f;
    float lAngleMin = 0.f;

    for(int lAngle = 0; lAngle < aAngleTestCount; lAngle++)
    {
        float lAngleRad = (float)lAngle * 2.f * M_PI / (float)aAngleTestCount + aAngleStart;
        float lDistanceWithAngle = getDistanceWithAngle(aBlob, lAngleRad);
        if(lDistanceMin > lDistanceWithAngle)
        {
            lDistanceMin = lDistanceWithAngle;
            lAngleMin = lAngleRad;
        }
    }
    aAngleStart = lAngleMin;
    return lDistanceMin;
}

void Object::drawShape(float x = 0, float y = 0, int aColor = 0x00FFFF)
{
    ofNoFill();
    ofSetHexColor(aColor);
    ofBeginShape();
    for (int i = 0; i < mShape.size(); i++){
        ofVertex(x + mShape[i].x, y + mShape[i].y);
    }
    ofEndShape(true);
    
    std::stringstream lStr;
    lStr << "sound" << mSoundIndex;
    ofSetHexColor(0xFFFFFF);
    ofDrawBitmapString(lStr.str(), x + 40, y);

}

void Object::drawBlob(float x, float y, int index)
{
    mBlobSmoother.draw(x,y, index);
}

bool Object::wantToSendMessage()
{
    return mBlobSmoother.wantToSendMessage();
}

ofxOscMessage Object::getOSCMessage()
{
    std::stringstream lStr;
    lStr << "/" << mSoundIndex;

    ofxOscMessage m;
    m.setAddress(lStr.str());
    m.addFloatArg(mBlobSmoother.getAngle());
    m.addFloatArg(mBlobSmoother.getRadius());
    m.addFloatArg(mBlobSmoother.getIntensity());
    return m;
}