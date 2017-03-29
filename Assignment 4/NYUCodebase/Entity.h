//
//  Entity.h
//  NYUCodebase
//
//  Created by Quanhao Li on 3/25/17.
//  Copyright © 2017 Ivan Safrin. All rights reserved.
//

#ifndef Entity_h
#define Entity_h

#include "SpriteSheet.h"

enum EntityType {ENTITY_PLAYER, ENTITY_ENEMY,
    ENTITY_COIN};

class Entity {
public:
    Entity(){}
    void Update(float elapsed);
    void Render(ShaderProgram *program, Matrix& modelMatrix);
    bool collidesWith(Entity *entity);
    
    void handleCollision_COIN (Entity* coin);
    
    void init(SpriteSheet sprite, float width, float height, float velocity_x, float velocity_y, float acceleration_x, float acceleration_y, float friction_x, float friction_y);
    
    SpriteSheet sprite;
    
    float init_x;
    float init_y;
    
    float x;
    float y;
    float width;
    float height;
    float velocity_x;
    float velocity_y;
    float acceleration_x;
    float acceleration_y;
    float friction_x;
    float friction_y;
    
    float gravity = -2.0f;
    
    bool isStatic;
    
    EntityType entityType;
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
};

#endif /* Entity_h */
