//
//  GameDirector.h
//  triptych
//
//  Created by Nick Enchev on 2014-12-09.
//
//

#ifndef __triptych__GameDirector__
#define __triptych__GameDirector__

#include <string>
#include "GameOptions.h"
#include "TriptychGame.h"

#define TRIPTYCH_GAME_SAVE_FILE "savedata.tpk"

class GameDirector
{
    std::string writablePath;
    
public:
    static GameDirector &getInstance()
    {
        static GameDirector director;
        return director;
    }
    
    unique_ptr<TriptychGame> startGame(GameOptions options);
    unique_ptr<TriptychGame> continueGame(std::string saveFilePath);

    void setWritablePath(const std::string writablePath) { this->writablePath = writablePath; }
    std::string getSaveFilePath() const { return writablePath + TRIPTYCH_GAME_SAVE_FILE; }
};

#endif /* defined(__triptych__GameDirector__) */
