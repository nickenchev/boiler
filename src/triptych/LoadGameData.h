//
//  LoadGameData.h
//  triptych
//
//  Created by Nick Enchev on 2014-12-12.
//
//

#ifndef triptych_LoadGameData_h
#define triptych_LoadGameData_h

#include <vector>
#include "SaveGameHeader.h"

struct LoadGameData
{
    SaveGameHeader saveHeader;
    mt19937 randomEngine;

    shared_ptr<Board> initialBoard;
    std::vector<string> powerUpCodes;
    std::vector<BoardCell> bankCells;
};

#endif
