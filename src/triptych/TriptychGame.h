//
//  TriptychGame.h
//  triptych
//
//  Created by Nick Enchev on 2014-10-22.
//
//

#ifndef __triptych__TriptychGame__
#define __triptych__TriptychGame__

#include "Board.h"
#include "Player.h"
#include "SaveGameHeader.h"
#include "LoadGameData.h"

class TriptychGame
{
    GameType gameType;
    Player player;
    Board board;
    bool gameOver, canUndo, tutorial, boughtPowerup;
    
public:
    TriptychGame(GameType gameType, const int rows, const int cols);
    TriptychGame(LoadGameData &loadData);
    
    void operator=(const TriptychGame &rh);
    
    Player &getPlayer() { return player; }
    Board &getBoard() { return board; }
    bool isGameOver() const { return gameOver; }
    bool getCanUndo() const { return canUndo; }
    bool isTutorial() const { return tutorial; }
    void setTutorial(bool tutorial) { this->tutorial = tutorial; }
    bool hasBoughtPowerup() const { return boughtPowerup; }
    
    void turnBegin();
    TurnInfo turnEnd();
    
    void reset();
    void saveGameData();
    static LoadGameData loadSaveGameData();
    
    //powerup management
    bool buyPowerup(shared_ptr<PowerUp> powerUp);
};
#endif /* defined(__triptych__TriptychGame__) */
