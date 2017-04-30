//
//  GridPosition.h
//  DigTale
//
//  Created by Nick Enchev on 2014-04-28.
//
//

#ifndef __DigTale__GridPosition__
#define __DigTale__GridPosition__

class GridPosition
{
public:
    unsigned int row, column;

    GridPosition();
    GridPosition(unsigned int row, unsigned int column);
    
    inline bool operator==(const GridPosition &rh) const { return (this->row == rh.row) && (this->column == rh.column); }
    inline bool operator!=(const GridPosition &rh) const { return !(*this == rh); }
    GridPosition &operator=(const GridPosition &rh);
};

#endif /* defined(__DigTale__GridPosition__) */
