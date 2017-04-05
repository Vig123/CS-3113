//
//  sat.h
//  NYUCodebase
//
//  Created by Quanhao Li on 4/2/17.
//  Copyright © 2017 Ivan Safrin. All rights reserved.
//

#ifndef sat_h
#define sat_h

#include "Vector.h"
#include <vector>

class SAT {
public:
    bool testSATSeparationForEdge(float edgeX, float edgeY, const std::vector<Vector> &points1, const std::vector<Vector> &points2, Vector &penetration);

    static bool penetrationSort(Vector &p1, Vector &p2);

    bool checkSATCollision(const std::vector<Vector> &e1Points, const std::vector<Vector> &e2Points, Vector &penetration);
};

#endif /* sat_h */
