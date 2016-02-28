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
    std::vector<Object *> getObjects();

private:
    ofxXmlSettings          XML;    
};

#endif /* xmlManager_hpp */
