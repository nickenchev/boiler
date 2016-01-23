//
//  GameOptions.h
//  triptych
//
//  Created by Nick Enchev on 2014-12-10.
//
//

#ifndef triptych_GameOptions_h
#define triptych_GameOptions_h

#include "Board.h"
#include "GameType.h"

struct GameOptions
{
    GameType gameType;
    int numberOfPlayers;
    int rows, columns;
};

#endif
