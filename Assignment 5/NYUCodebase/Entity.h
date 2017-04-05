//
//  Entity.h
//  NYUCodebase
//
//  Created by Quanhao Li on 4/2/17.
//  Copyright © 2017 Ivan Safrin. All rights reserved.
//

#ifndef Entity_h
#define Entity_h

#include "Matrix.h"
#include "Vector.h"
#include "ShaderProgram.h"
#include <vector>
#include "Vector.h"

class Entity {
public:
    
    Entity(){};
    Entity(Matrix m, Vector p, Vector s, float r): matrix(m), position(p), scale(s), rotation(r), velocity_x(0), velocity_y(0), acceleration_x(0), acceleration_y(0), friction_x(0), friction_y(0){};

    Matrix matrix;
    
    Vector position;
    Vector prev_pos;
    Vector scale;
    float rotation;
    
    float velocity_x;
    float velocity_y;
    float acceleration_x;
    float acceleration_y;
    float friction_x;
    float friction_y;
    
    std::vector<Vector> vertices;
    
    void Draw(ShaderProgram& p, float vertex[]);
    void Update (float elapsed);
    void Render (ShaderProgram& p, float vertex[]);
    
    
};

#endif /* Entity_h */
