#include "ofApp.h"
#include "Settings.hpp"

//--------------------------------------------------------------
void ofApp::setup(){
    
#ifdef _USE_LIVE_VIDEO
    vidGrabber.setDeviceID(1);
    vector<ofVideoDevice> lVec = vidGrabber.listDevices();
    vidGrabber.setVerbose(true);
    vidGrabber.setup(320,240);
#else
    vidPlayer.load("testCamWithRotation.3gp");
    vidPlayer.play();
    vidPlayer.setVolume(0.f);
    vidPlayer.setLoopState(OF_LOOP_NORMAL);
#endif
    
    rectImg.load("images/rect.png");
    
    colorImg.allocate(320,240);
    grayImage.allocate(320,240);
    grayBg.allocate(320,240);
    grayDiff.allocate(320,240);
    
    bLearnBakground = true;
    Settings::sWhiteThreshold = 80;
    
    int numDragTags = mXmlManager.getObjectCount();
    
    for(int objectId = 0; objectId < numDragTags; objectId++)
    {
        mRecordObjects = mXmlManager.getObjects();
    }
    
    mIndexShapeSelected = 0;
    mDraw = true;
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
    vidPlayer.update();
    bNewFrame = vidPlayer.isFrameNew();
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
        
        std::vector<BlobSmoother::Blobito> * lBlobSorter[mRecordObjects.size()];
        
    }
}

float ofApp::getClosest(int blobId, int & aClosestId)
{
    ofxCvBlob lBlob = contourFinder.blobs[blobId];
    int lMinIndex = -1;
    float lDistanceMin = 100;
    float lAngleMin = 0.f;
    int lCountUnderThreshold = 0;
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
        grayImage.draw(360,20);
        grayBg.draw(20,280);
        grayDiff.draw(360,280);
        
        // then draw the contours:
        
        ofFill();
        ofSetHexColor(0x333333);
        ofDrawRectangle(360,540,320,240);
        ofSetHexColor(0xffffff);
        
        // we could draw the whole contour finder
        //contourFinder.draw(360,540);
        
        // or, instead we can draw each blob individually from the blobs vector,
        // this is how to get access to them:
        for (int i = 0; i < contourFinder.nBlobs; i++){
            contourFinder.blobs[i].draw(360,540);

            int lIndexClosest;
            getClosest(i, lIndexClosest);
            if(lIndexClosest >= 0)
            {
                std::stringstream lStr;
                lStr << lIndexClosest;
                ofDrawBitmapString(lStr.str(), contourFinder.blobs[i].centroid.x + 20, contourFinder.blobs[i].centroid.y + 20);
            }
//            // draw over the centroid if the blob is a hole
//            ofSetColor(255);
//            if(contourFinder.blobs[i].hole){
//                ofDrawBitmapString("hole",
//                                   contourFinder.blobs[i].boundingRect.getCenter().x + 360,
//                                   contourFinder.blobs[i].boundingRect.getCenter().y + 540);
//            }
        }
        
        for(int i = 0; i < mRecordObjects.size(); i++)
        {
            mRecordObjects[i]->drawShape(700, 60 + 60 * i, i == mIndexShapeSelected ? 0xFFFFFF : 0x000000);
//            mRecordObjects[i]->drawBlob(20, 20, i);
            std::stringstream lStr;
            lStr << i;
            ofSetHexColor(0xFFFFFF);
            ofDrawBitmapString(lStr.str(), 700, 60 + 60 * i);
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

void ofApp::simplifyDP_openCV ( const vector<ofPoint>& contourIn, vector<ofPoint>& contourOut, float tolerance )
{
    
    //    result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);
    
    //-- copy points.
    
    int numOfPoints;
    numOfPoints = contourIn.size();
    
    CvPoint* cvpoints;
    cvpoints = new CvPoint[ numOfPoints ];
    
    for( int i=0; i<numOfPoints; i++)
    {
        int j = i % numOfPoints;
        
        cvpoints[ i ].x = contourIn[ j ].x;
        cvpoints[ i ].y = contourIn[ j ].y;
    }
    
    //-- create contour.
    
    CvContour	contour;
    CvSeqBlock	contour_block;
    
    cvMakeSeqHeaderForArray
    (
     CV_SEQ_POLYLINE,
     sizeof(CvContour),
     sizeof(CvPoint),
     cvpoints,
     numOfPoints,
     (CvSeq*)&contour,
     &contour_block
     );
    
    printf( "length = %f \n", cvArcLength( &contour ) );
    
    //-- simplify contour.
    
    CvMemStorage* storage;
    storage = cvCreateMemStorage( 1000 );
    
    CvSeq *result = 0;
    double contourSize = cvContourPerimeter( &contour );
    result = cvApproxPoly
    (
     &contour,
     sizeof( CvContour ),
     storage,
     CV_POLY_APPROX_DP,
     contourSize * tolerance,
     0
     );
    
    //-- contour out points.
    
    contourOut.clear();
    for( int j=0; j<result->total; j++ )
    {
        CvPoint * pt = (CvPoint*)cvGetSeqElem( result, j );
        
        bool lAlreadyExist = false;
        for(int pti = 0; pti < contourOut.size(); pti++)
        {
            float xDis = contourOut[pti].x - (float)pt->x;
            float yDis = contourOut[pti].y - (float)pt->y;
            if((xDis * xDis + yDis * yDis) < (contourSize * contourSize * 0.005))
            {
                lAlreadyExist = true;
            }
        }
        if(!lAlreadyExist)
        {
            contourOut.push_back( ofPoint() );
            contourOut.back().x = (float)pt->x;
            contourOut.back().y = (float)pt->y;
        }
    }
    
    //-- clean up.
    
    if( storage != NULL )
        cvReleaseMemStorage( &storage );
    
    delete[] cvpoints;
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
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
//    saveShape(x, y);
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
