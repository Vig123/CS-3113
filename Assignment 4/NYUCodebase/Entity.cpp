//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Quanhao Li on 3/25/17.
//  Copyright © 2017 Ivan Safrin. All rights reserved.
//

#include "Entity.h"


float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

void Entity::init(SpriteSheet sprite, float width, float height, float velocity_x, float velocity_y, float acceleration_x, float acceleration_y, float friction_x, float friction_y) {
    this->sprite = sprite;
    this->width = width;
    this->height = height;
    this->velocity_x = velocity_x;
    this->velocity_y = velocity_y;
    this->acceleration_x = acceleration_x;
    this->acceleration_y = acceleration_y;
    this->friction_x = friction_x;
    this->friction_y = friction_y;
    
    this->init_x = this->x;
    this->init_y = this->y;
}

void Entity::Render(ShaderProgram *p, Matrix& m) {
    m.identity();
    m.Translate(this->x, this->y, 0);
    p->setModelMatrix(m);
    this->sprite.Draw(p);
}

void Entity::Update(float elapsed) {
    if (!this->isStatic) {
        
        if (this->acceleration_x > 2.5f) this->acceleration_x = 2.5f;
        else if (this->acceleration_x < -2.5f) this->acceleration_x = -2.5f;
        
        if (this->acceleration_y > 0) this->acceleration_y-= elapsed;
        
        this->velocity_x = lerp(this->velocity_x, 0.0f, elapsed * this->friction_x);
        this->velocity_y = lerp(this->velocity_y, 0.0f, elapsed * this->friction_y);
        
        this->velocity_x += this->acceleration_x * elapsed;
        this->velocity_y += this->acceleration_y * elapsed;
        this->velocity_y += this->gravity * elapsed;
        
        this->x += this->velocity_x * elapsed;
        this->y += this->velocity_y * elapsed;
    }
}

void Entity::handleCollision_COIN(Entity *coin) {
    //delete coin;
    coin->x = 500;
    //coin = nullptr;
}

bool Entity::collidesWith(Entity *entity) {
    if ((y - sprite.size/2) > (entity->y + entity->sprite.size/2)) {
        return false;
    }
    if ((y + sprite.size/2) < (entity->y - entity->sprite.size/2)) {
        return false;
    }
    if ((x - sprite.size/2) > (entity->x + entity->sprite.size/2)) {
        return false;
    }
    if ((x + sprite.size/2) < (entity->x - entity->sprite.size/2)) {
        return false;
    }
    if (entity->entityType == ENTITY_COIN) handleCollision_COIN(entity);
    return true;
}
