//
//  entity.h
//  NYUCodebase
//
//  Created by Quanhao Li on 2/15/17.
//  Copyright © 2017 Ivan Safrin. All rights reserved.
//

#ifndef entity_h
#define entity_h

#include "ShaderProgram.h"


class Entity {
public:
    
    Entity (float x, float y, float rotation, float width, float height, float speed, float direction_x, float direction_y):init_x(x),init_y(y), x(x), y(y), rotation(rotation), width(width), height(height), speed(speed), direction_x(direction_x), direction_y(direction_y) {};
    
    void Draw(ShaderProgram& , float []);
    
    float init_x;
    float init_y;
    
    float x;
    float y;
    float rotation;
    
    //int textureID;
    float width;
    float height;
    
    float speed;
    float direction_x;
    float direction_y;
    
};

#endif /* entity_h */
