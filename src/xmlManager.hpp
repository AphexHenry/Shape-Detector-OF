//
//  xmlManager.hpp
//  shapeDetector
//
//  Created by Baptiste Bohelay on 28/02/2016.
//
//

#ifndef xmlManager_hpp
#define xmlManager_hpp

#include <stdio.h>
#include "Object.hpp"
#include "ofxXmlSettings.h"

class xmlManager
{
public:
    xmlManager();
    int getObjectCount();
    
    /*
     * Load the saved detected shapes.
     */
    std::vector<Object *> getObjects();
    
    /*
     * Save the detected shapes.
     */
    void saveObjects(std::vector<Object *> aObjects);

    void SaveSettings();
    
    /*
     * associate an object to a sound index in the xml.
     */
    void associateObjectAndSoundIndex(int aObjectIndex, int aSoundIndex);
    
private:
    ofxXmlSettings          XMLObjects;
    ofxXmlSettings          XMLGeneral;
};

#endif /* xmlManager_hpp */
