//
//  SaveGameHeader.h
//  triptych
//
//  Created by Nick Enchev on 2014-12-05.
//
//

#ifndef __triptych__SaveGameHeader__
#define __triptych__SaveGameHeader__

#include "BoardCell.h"
#include "Board.h"
#include "Player.h"
#include "GameType.h"

class SaveGameHeader
{
public:
    SaveGameHeader();
    
    int rows, columns;
    GameAttribute level;
    GameAttribute disks;
    int numPowerUps;
    int bankCapacity;
    int bankSlotsUsed;
    GameType gameType;
    BoardCell prevNumber, currentNumber, nextNumber, highestNumber;
};

#endif /* defined(__triptych__SaveGameHeader__) */
