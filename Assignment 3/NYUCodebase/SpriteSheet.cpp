//
//  SpriteSheet.cpp
//  NYUCodebase
//
//  Created by Quanhao Li on 2/24/17.
//  Copyright © 2017 Ivan Safrin. All rights reserved.
//

#include <stdio.h>
#include "SpriteSheet.h"

void SpriteSheet::Draw(ShaderProgram* program){
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat texCoords[] = {
        u, v+height,
        u+width, v,
        u, v,
        u+width, v,
        u, v+height,
        u+width, v+height
    };
    float aspect = width / height;
    float vertices[] = {
        -0.5f * size * aspect, -0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, 0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, -0.5f * size ,
        0.5f * size * aspect, -0.5f * size
    };
    
    // Defines array of Vertex Data
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    // Defines an array of Texture Data
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    // Draws the triangles that will be textured
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Disables program attributes after use
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
