#include "ofApp.h"
#include "Settings.hpp"
#include "ofxPS3EyeGrabber.h"

#define HOST "localhost"
#define PORT 1314
#define IMG_WIDTH 320
#define IMG_HEIGHT 240

void ofApp::setup(){
    
#ifdef _USE_LIVE_VIDEO
    vector<ofVideoDevice> lVec = vidGrabber.listDevices();
    vidGrabber.setVerbose(true);
    if(Settings::sUseKinect == 1)
    {
        vidGrabber.setGrabber(std::make_shared<ofxPS3EyeGrabber>());
    }
    else {
        vidGrabber.setDeviceID(Settings::sCameraIndex);
    }
    vidGrabber.setup(IMG_WIDTH,IMG_HEIGHT);
#else
    vidPlayer.load("FindingContours.mp4");
    vidPlayer.play();
    vidPlayer.setVolume(0.f);
    vidPlayer.setLoopState(OF_LOOP_NORMAL);
#endif
    
    colorImg.allocate(IMG_WIDTH,IMG_HEIGHT);
    grayImage.allocate(IMG_WIDTH,IMG_HEIGHT);
    grayBg.allocate(IMG_WIDTH,IMG_HEIGHT);
    grayDiff.allocate(IMG_WIDTH,IMG_HEIGHT);
    
    bLearnBakground = true;
    Settings::sWhiteThreshold = 80;
    
    int numDragTags = mXmlManager.getObjectCount();
    
    for(int objectId = 0; objectId < numDragTags; objectId++)
    {
        mRecordObjects = mXmlManager.getObjects();
    }
    
    for(int i = 0; i < mRecordObjects.size(); i++)
    {
        mRecordObjects[i]->setCenter(ofVec2f(IMG_WIDTH * 0.5, IMG_HEIGHT * 0.5));
    }
    
    mIndexShapeSelected = 0;
    mDraw = true;
    mNextFrame = true;
    sender.setup(HOST, PORT);
}

void ofApp::saveShapes()
{
    for(int i = 0; i < mRecordObjects.size(); i++)
    {
        delete mRecordObjects[i];
    }
    mRecordObjects.clear();
    
    for (int i = 0; i < contourFinder.nBlobs; i++){
        Object * lObject = new Object();
        lObject->setSoundIndex(i);
        lObject->setCenter(ofVec2f(IMG_WIDTH * 0.5, IMG_HEIGHT * 0.5));
        ofPoint lCenter = contourFinder.blobs[i].centroid;
        
        for(int pt = 0; pt < contourFinder.blobs[i].nPts; pt++)
        {
            lObject->addPt(contourFinder.blobs[i].pts[pt].x - lCenter.x, contourFinder.blobs[i].pts[pt].y - lCenter.y);
        }
        mRecordObjects.push_back(lObject);
    }
    
    mXmlManager.saveObjects(mRecordObjects);
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(100,100,100);
    
    bool bNewFrame = false;
    
#ifdef _USE_LIVE_VIDEO
    vidGrabber.update();
	   bNewFrame = vidGrabber.isFrameNew();
#else
    if(mNextFrame)
    {
        vidPlayer.update();
        bNewFrame = vidPlayer.isFrameNew();
    }
#endif
    
    if (bNewFrame){
        
#ifdef _USE_LIVE_VIDEO
        colorImg.setFromPixels(vidGrabber.getPixels());
#else
        colorImg.setFromPixels(vidPlayer.getPixels());
#endif
        
        grayImage = colorImg;
        if (bLearnBakground == true){
            grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBakground = false;
        }
        
        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(Settings::sWhiteThreshold);
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, false, false);	// find holes

        for(int i = 0; i < mRecordObjects.size(); i++)
        {
            mRecordObjects[i]->startNewFrame();
        }
                
        mBlobMapper.clear();
        for (int i = 0; i < contourFinder.nBlobs; i++){
            int lIndexClosest;
            getClosest(i, lIndexClosest);
            mBlobMapper.push_back(lIndexClosest);
            if(lIndexClosest >= 0)
            {
                mRecordObjects[lIndexClosest]->addNewMatch(contourFinder.blobs[i]);
            }
        }
        
        for(int i = 0; i < mRecordObjects.size(); i++)
        {
            mRecordObjects[i]->digestFrame();
            if(mRecordObjects[i]->wantToSendMessage())
            {
                ofxOscMessage lMess = mRecordObjects[i]->getOSCMessage();
                sender.sendMessage(lMess, false);
            }
        }
    }
}

float ofApp::getClosest(int blobId, int & aClosestId)
{
    ofxCvBlob lBlob = contourFinder.blobs[blobId];
    int lMinIndex = -1;
    float lDistanceMin = 1000;
    float lAngleMin = 0.f;
    for(int i = 0; i < mRecordObjects.size(); i++)
    {
        float lAngleStart = 0.f;
        float lDistance = mRecordObjects[i]->getDistance(&lBlob, lAngleStart);
        
        if(lDistance < lDistanceMin)
        {
            lAngleMin = lAngleStart;
            lDistanceMin = lDistance;
            lMinIndex = i;
        }
    }

    aClosestId = lMinIndex;
    return lDistanceMin;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(mDraw)
    {
        // draw the incoming, the grayscale, the bg and the thresholded difference
        ofSetHexColor(0xffffff);
        colorImg.draw(20,20);
        grayImage.draw(IMG_WIDTH + 40,20);
        grayBg.draw(20,IMG_HEIGHT + 40);
        grayDiff.draw(IMG_WIDTH + 40,IMG_HEIGHT+ 40);
        
        // then draw the contours:
        
        ofFill();
        ofSetHexColor(0x333333);
        ofDrawRectangle(IMG_WIDTH + 40,IMG_HEIGHT * 2 + 60,IMG_WIDTH,IMG_HEIGHT);
        ofSetHexColor(0xffffff);
        
        // we could draw the whole contour finder
        //contourFinder.draw(360,540);
        
        // or, instead we can draw each blob individually from the blobs vector,
        // this is how to get access to them:
        for (int i = 0; i < contourFinder.nBlobs; i++){
            contourFinder.blobs[i].draw(IMG_WIDTH + 40,IMG_HEIGHT * 2 + 60);

            int lIndexClosest = mBlobMapper[i];
            if(lIndexClosest >= 0)
            {
                std::stringstream lStr;
                lStr << lIndexClosest;
                ofDrawBitmapString(lStr.str(), contourFinder.blobs[i].centroid.x + 15, contourFinder.blobs[i].centroid.y + 15);
            }
        }
        
        for(int i = 0; i < mRecordObjects.size(); i++)
        {
            mRecordObjects[i]->drawShape(2 * IMG_WIDTH + 60, 60 + 60 * i, i == mIndexShapeSelected ? 0xFFFFFF : 0x000000);
            mRecordObjects[i]->drawBlob(20, 20, i);
//            std::stringstream lStr;
//            lStr << i;
//            ofSetHexColor(0xFFFFFF);
//            ofDrawBitmapString(lStr.str(), 700, 60 + 60 * i);
        }
    }
    
    // finally, a report:
    ofSetHexColor(0xffffff);
    stringstream reportStr;
    reportStr << "bg subtraction and blob detection" << endl
    << "press ' ' to capture bg" << endl
    << "threshold " << Settings::sWhiteThreshold << " (press: +/-)" << endl
    << "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
    ofDrawBitmapString(reportStr.str(), 20, 600);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key){
        case ' ':
            bLearnBakground = true;
            break;
        case '+':
            Settings::sWhiteThreshold ++;
            if (Settings::sWhiteThreshold > 255) Settings::sWhiteThreshold = 255;
            break;
        case '-':
            Settings::sWhiteThreshold --;
            if (Settings::sWhiteThreshold < 0) Settings::sWhiteThreshold = 0;
            break;
        case 's':
            saveShapes();
            break;
        case 'd':
            mDraw = !mDraw;
            break;
        case 'g':
            mXmlManager.SaveSettings();
            break;
        case OF_KEY_DOWN:
            mIndexShapeSelected++;
            mIndexShapeSelected = mIndexShapeSelected % mRecordObjects.size();
            break;
        case OF_KEY_UP:
            mIndexShapeSelected--;
            if(mIndexShapeSelected < 0)
            {
                mIndexShapeSelected = mRecordObjects.size() - 1;
            }
            break;
        case OF_KEY_RIGHT:
            mRecordObjects[mIndexShapeSelected]->pushSoundIndex(1);
            break;
        case OF_KEY_LEFT:
            mRecordObjects[mIndexShapeSelected]->pushSoundIndex(-1);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    mNextFrame  = false;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
//    saveShape(x, y);
    mNextFrame = true;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
