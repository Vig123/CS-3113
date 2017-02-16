//
//  entity.cpp
//  NYUCodebase
//
//  Created by Quanhao Li on 2/15/17.
//  Copyright © 2017 Ivan Safrin. All rights reserved.
//

#include <stdio.h>
#include "entity.h"

#include "ShaderProgram.h"

void Entity::Draw(ShaderProgram& program, float vertex[]) {
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertex);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
}
