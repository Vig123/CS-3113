//
//  entity.h
//  NYUCodebase
//
//  Created by Quanhao Li on 2/15/17.
//  Copyright © 2017 Ivan Safrin. All rights reserved.
//

#ifndef entity_h
#define entity_h

#include "Vector3.h"
#include "SpriteSheet.h"
#include "Matrix.h"

class Entity {
public:
    
    Entity () {};
    
    Entity (SpriteSheet* spr, Vector3* vel = new Vector3(0,0,0), Vector3* pos = new Vector3(0,0,0), float rot = 0.0f) : Pos(*pos), init_Pos(*pos), velocity (*vel), rotation (rot), sprite (*spr)
    {
        size = sprite.size;
    };
    
    Vector3 init_Pos;
    Vector3 Pos;
    Vector3 velocity;
    float size;
    
    SpriteSheet sprite;
    float rotation;
    
};

#endif /* entity_h */
