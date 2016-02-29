//
//  xmlManager.cpp
//  shapeDetector
//
//  Created by Baptiste Bohelay on 28/02/2016.
//
//

#include "xmlManager.hpp"

#define XML_PATH "mySettings.xml"

xmlManager::xmlManager()
{
    if( XML.loadFile(XML_PATH) ){
        std::cout << "mySettings.xml loaded!" << std::endl;
    }else{
        std::cout << "unable to load mySettings.xml check data/ folder" << std::endl;
    }
}

int xmlManager::getObjectCount()
{
    return XML.getNumTags("OBJECT");
    
}

void xmlManager::saveObjects(std::vector<Object *> aObjects)
{
    XML.clear();
    for(int o = 0; o < aObjects.size(); o++)
    {
        int objIndex = XML.addTag("OBJECT");
        XML.pushTag("OBJECT", objIndex);
        XML.addTag("SOUND");
        XML.setValue("SOUND", aObjects[o]->getSoundIndex());
        std::vector<ofVec2f> lPts = aObjects[o]->getPts();
        for(int i = 0; i < lPts.size(); i++)
        {
            int tagNum = XML.addTag("PT");
            XML.setValue("PT:X", lPts[i].x, tagNum);
            XML.setValue("PT:Y", lPts[i].y, tagNum);
        }
        
        XML.popTag();
    }
    
    XML.saveFile(XML_PATH);
}

void xmlManager::associateObjectAndSoundIndex(int aObjectIndex, int aSoundIndex)
{
    if(XML.pushTag("OBJECT", aObjectIndex))
    {
        XML.popTag();
    }
    
}

std::vector<Object *> xmlManager::getObjects()
{
    int objectCount = getObjectCount();
    std::vector<Object *> lContainer;
    
    for(int objectId = 0; objectId < objectCount; objectId++)
    {
        //we push into the last STROKE tag
        //this temporarirly treats the tag as
        //the document root.
        XML.pushTag("OBJECT", objectId);
        Object * lObject = new Object();
        
        //we see how many points we have stored in <PT> tags
        int numPtTags = XML.getNumTags("PT");
        
        if(numPtTags > 0){
            
            //We then read those x y values into our
            //array - so that we can then draw the points as
            //a line on the screen
            
            //we have only allocated a certan amount of space for our array
            //so we don't want to read more than that amount of points
            int totalToRead = numPtTags;
            
            for(int i = 0; i < totalToRead; i++){
                //the last argument of getValue can be used to specify
                //which tag out of multiple tags you are refering to.
                int x = XML.getValue("PT:X", 0, i);
                int y = XML.getValue("PT:Y", 0, i);
                //                dragPts[i].set(x, y);
                lObject->addPt(x, y);
                //                pointCount++;
            }
        }
        
        lContainer.push_back(lObject);
        
        //this pops us out of the STROKE tag
        //sets the root back to the xml document
        XML.popTag();
    }
    
    return lContainer;
}