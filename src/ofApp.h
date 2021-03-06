#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"

#include "Object.hpp"
#include "xmlManager.hpp"

//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
// otherwise, we'll use a movie file

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    float getClosest(int blobId, int & aClosestId);
    
#ifdef _USE_LIVE_VIDEO
		  ofVideoGrabber 		vidGrabber;
#else
		  ofVideoPlayer 		vidPlayer;
#endif
private:
    
    void saveShapes();
    
    std::vector<int> mBlobMapper;
    
    ofxCvColorImage			colorImg;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    
    ofxCvContourFinder 	contourFinder;
    
    bool				bLearnBakground;
    
    xmlManager      mXmlManager;
    
    bool mDraw;
    bool mNextFrame;
    
    int mIndexShapeSelected;
    
    ofxOscSender sender;
    
    /*
     *  list of user recorded objects.
     */
    std::vector<Object *> mRecordObjects;
};

