//
//  GameDirector.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-12-09.
//
//

#include <memory>
#include "GameDirector.h"
#include "LoadGameData.h"

unique_ptr<TriptychGame> GameDirector::startGame(GameOptions options)
{
    return std::unique_ptr<TriptychGame>(new TriptychGame(options.gameType, options.rows, options.columns));
}

unique_ptr<TriptychGame> GameDirector::continueGame(std::string saveFilePath)
{
    LoadGameData loadData = TriptychGame::loadSaveGameData();
    return std::unique_ptr<TriptychGame>(new TriptychGame(loadData));
}