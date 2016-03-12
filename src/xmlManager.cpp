//
//  xmlManager.cpp
//  shapeDetector
//
//  Created by Baptiste Bohelay on 28/02/2016.
//
//

#include "xmlManager.hpp"
#include "Settings.hpp"

#define XML_OBJECTS_SETTINGS_PATH "ObjectsSettings.xml"
#define XML_GENERAL_SETTINGS_PATH "ObjectsSettings.xml"

xmlManager::xmlManager()
{
    if( XMLObjects.loadFile(XML_OBJECTS_SETTINGS_PATH) ){
        std::cout << "mySettings.xml loaded!" << std::endl;
    }else{
        std::cout << "unable to load mySettings.xml check data/ folder" << std::endl;
    }
    
    if( XMLGeneral.loadFile(XML_GENERAL_SETTINGS_PATH) ){
        std::cout << "mySettings.xml loaded!" << std::endl;
    }else{
        std::cout << "unable to load mySettings.xml check data/ folder" << std::endl;
    }
}

int xmlManager::getObjectCount()
{
    return XMLObjects.getNumTags("OBJECT");
    
}

void xmlManager::saveObjects(std::vector<Object *> aObjects)
{
    XMLObjects.clear();
    for(int o = 0; o < aObjects.size(); o++)
    {
        int objIndex = XMLObjects.addTag("OBJECT");
        XMLObjects.pushTag("OBJECT", objIndex);
        XMLObjects.addTag("SOUND");
        XMLObjects.setValue("SOUND", aObjects[o]->getSoundIndex());
        std::vector<ofVec2f> lPts = aObjects[o]->getPts();
        for(int i = 0; i < lPts.size(); i++)
        {
            int tagNum = XMLObjects.addTag("PT");
            XMLObjects.setValue("PT:X", lPts[i].x, tagNum);
            XMLObjects.setValue("PT:Y", lPts[i].y, tagNum);
        }
        
        XMLObjects.popTag();
    }
    
    XMLObjects.saveFile(XML_OBJECTS_SETTINGS_PATH);
}

void xmlManager::associateObjectAndSoundIndex(int aObjectIndex, int aSoundIndex)
{
    if(XMLObjects.pushTag("OBJECT", aObjectIndex))
    {
        XMLObjects.popTag();
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
        XMLObjects.pushTag("OBJECT", objectId);
        Object * lObject = new Object();
        
        //we see how many points we have stored in <PT> tags
        int numPtTags = XMLObjects.getNumTags("PT");
        
        if(numPtTags > 0){

            //we have only allocated a certan amount of space for our array
            //so we don't want to read more than that amount of points
            int totalToRead = numPtTags;
            
            for(int i = 0; i < totalToRead; i++){
                //the last argument of getValue can be used to specify
                //which tag out of multiple tags you are refering to.
                int x = XMLObjects.getValue("PT:X", 0, i);
                int y = XMLObjects.getValue("PT:Y", 0, i);
                lObject->addPt(x, y);
            }
        }
        
        int lSoundIndex = XMLObjects.getValue("SOUND", 0);
        lObject->setSoundIndex(lSoundIndex);
        
        lContainer.push_back(lObject);
        
        //this pops us out of the STROKE tag
        //sets the root back to the xml document
        XMLObjects.popTag();
    }
    
    return lContainer;
}

void xmlManager::SaveSettings()
{
    XMLGeneral.clear();
    XMLGeneral.addValue("whiteThreshold", Settings::sWhiteThreshold);
    XMLGeneral.addValue("cameraIndex", Settings::sCameraIndex);
    XMLGeneral.saveFile(XML_GENERAL_SETTINGS_PATH);
}