//
//  Pos2D.h
//  triptych
//
//  Created by Nick Enchev on 2015-03-06.
//
//

#ifndef triptych_Pos2D_h
#define triptych_Pos2D_h

struct Pos2D
{
    int x, y;
    Pos2D() : Pos2D(0, 0) { }
    Pos2D(int x, int y) : x(x), y(y) { }
};


#endif
