//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Quanhao Li on 4/2/17.
//  Copyright © 2017 Ivan Safrin. All rights reserved.
//

#include <stdio.h>
#include "Entity.h"

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

void Entity::Draw(ShaderProgram& p, float vertex[]) {
    glVertexAttribPointer(p.positionAttribute, 2, GL_FLOAT, false, 0, vertex);
    glEnableVertexAttribArray(p.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(p.positionAttribute);
}

void Entity::Render(ShaderProgram& p, float vertex[]) {
    matrix.Translate(position.x-prev_pos.x, position.y-prev_pos.y, 0);
    p.setModelMatrix(matrix);
    Draw(p, vertex);
}

void Entity::Update(float elapsed) {
    
    this->velocity_x += this->acceleration_x * elapsed;
    this->velocity_y += this->acceleration_y * elapsed;

    this->prev_pos = position;
    
    this->position.x += this->velocity_x * elapsed;
    this->position.y += this->velocity_y * elapsed;
}

