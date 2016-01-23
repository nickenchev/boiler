//
//  TriptychGame.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-10-22.
//
//

#include <fstream>
#include "TriptychGame.h"
#include "GameDirector.h"

TriptychGame::TriptychGame(GameType gameType, const int rows, const int cols) : board(rows, cols)
{
    this->gameType = gameType;
    gameOver = false;
    canUndo = false;
    tutorial = false;
    boughtPowerup = false;
}

TriptychGame::TriptychGame(LoadGameData &loadData) : TriptychGame(loadData.saveHeader.gameType, loadData.saveHeader.rows, loadData.saveHeader.columns)
{
    //initialize the game cells with the save data
    Board &initialBoard = *loadData.initialBoard;
    for (int r = 1; r <= loadData.saveHeader.rows; ++r)
    {
        for (int c = 1; c <= loadData.saveHeader.columns; ++c)
        {
            board.getCell(r, c) = initialBoard(r, c);
        }
    }
    
    //add the available numbers
    BoardCell number = board.getHighestNumber();
    while (number <= loadData.saveHeader.highestNumber)
    {
        board.addAvailableNumber(number);
        number.increaseValue();
    }
    
    //random engines
    board.setRandomEngine(loadData.randomEngine);
    
    board.setPrevNumber(loadData.saveHeader.prevNumber);
    board.setCurrentNumber(loadData.saveHeader.currentNumber);
    board.setNextNumber(loadData.saveHeader.nextNumber);
    
    //setup the player state
    player.setLevelAttribute(loadData.saveHeader.level);
    player.setDisksAttribute(loadData.saveHeader.disks);
    
    //setup number bank
    player.setBankSlots(loadData.saveHeader.bankCapacity);
    for (BoardCell &bankCell : loadData.bankCells)
    {
        player.bankCell(bankCell);
    }
    
    //powerups
    for (int i = 0; i < loadData.powerUpCodes.size(); ++i)
    {
        shared_ptr<PowerUp> powerUp = PowerUp::createPowerUp(loadData.powerUpCodes[i]);
        if (powerUp)
        {
            player.addPowerUp(powerUp);
        }
    }
}

void TriptychGame::operator=(const TriptychGame &rh)
{
    gameType = rh.gameType;
    player = rh.player;
    board = rh.board;
    gameOver = rh.gameOver;
    canUndo = false;
    tutorial = rh.tutorial;
}

void TriptychGame::reset()
{
    gameOver = false;
    canUndo = false;

    player.reset();
    board.restart();
}

void TriptychGame::turnBegin()
{
    board.turnBegin();
}

TurnInfo TriptychGame::turnEnd()
{
    TurnInfo turnInfo = board.turnEnd(getPlayer());
    gameOver = turnInfo.gameOver;

    return turnInfo;
}

void TriptychGame::saveGameData()
{
    canUndo = true;
    Player &player = getPlayer();
    Board &board = getBoard();

    SaveGameHeader saveHeader;
    saveHeader.rows = board.getRows();
    saveHeader.columns = board.getColumns();
    saveHeader.numPowerUps = static_cast<int>(player.getPowerUps().size());
    saveHeader.bankCapacity = player.getBankCellSlotCount();
    saveHeader.bankSlotsUsed = static_cast<int>(player.getBankCells().size());
    saveHeader.prevNumber = board.getPrevNumber();
    saveHeader.currentNumber = board.getCurrentNumber();
    saveHeader.nextNumber = board.getNextNumber();
    saveHeader.highestNumber = board.getHighestNumber();
    saveHeader.level = player.getLevelAttribute();
    saveHeader.disks = player.getDisksAttribute();
    
    try
    {
        ofstream saveFile(GameDirector::getInstance().getSaveFilePath(), ios::binary);
        saveFile.write((char *)&saveHeader, sizeof(saveHeader));
        
        //store the board cells
        for (int r = 1; r <= board.getRows(); ++r)
        {
            for (int c = 1; c <= board.getColumns(); ++c)
            {
                const BoardCell &cell = board(r, c);
                saveFile.write((char *)&cell, sizeof(cell));
            }
        }
        
        //store the bank cells
        for (const BoardCell &cell : player.getBankCells())
        {
            saveFile.write((char *)&cell, sizeof(cell));
        }
        
        //persist the powerups by powerup-code
        for (int i = 0; i < saveHeader.numPowerUps; ++i)
        {
            PowerUp *powerUp = player.getPowerUps()[i].get();
            saveFile.write(powerUp->getCode().c_str(), powerUp->getCode().size());
        }
        
        //persist the random engine
        auto numberEng = board.getRandomEngine();
        saveFile.write((char *)&numberEng, sizeof(numberEng));
        
        saveFile.flush();
        saveFile.close();
    }
    catch (exception ex)
    {
        cout << ex.what() << endl;
    }
}

LoadGameData TriptychGame::loadSaveGameData()
{
    LoadGameData loadData;
    ifstream saveFile(GameDirector::getInstance().getSaveFilePath(), ios::binary);
    if (saveFile.is_open())
    {
        //load the save data header
        saveFile.read((char *)&loadData.saveHeader, sizeof(loadData.saveHeader));
        
        //load the board cells
        Board *board = new Board(loadData.saveHeader.rows, loadData.saveHeader.columns);
        loadData.initialBoard = shared_ptr<Board>(board);
        for (int r = 1; r <= loadData.saveHeader.rows; ++r)
        {
            for (int c = 1; c <= loadData.saveHeader.columns; ++c)
            {
                const BoardCell &cell = (*board)(r, c);
                saveFile.read((char *)&cell, sizeof(cell));
            }
        }
        
        //load the bank
        for (int i = 0; i < loadData.saveHeader.bankSlotsUsed; ++i)
        {
            BoardCell cell;
            saveFile.read((char *)&cell, sizeof(cell));
            loadData.bankCells.push_back(cell);
        }
        
        //load the powerup codes
        for (int i = 0; i < loadData.saveHeader.numPowerUps; ++i)
        {
            char powerUpCode[4];
            saveFile.read(powerUpCode, 3);
            powerUpCode[3] = '\0';
            loadData.powerUpCodes.push_back(string(powerUpCode));
        }
        //load the random state
        saveFile.read((char *)&loadData.randomEngine, sizeof(loadData.randomEngine));
        
        saveFile.close();
    }
    
    return loadData;
}

bool TriptychGame::buyPowerup(shared_ptr<PowerUp> powerUp)
{
    bool success = false;
    if (player.getDisksAttribute().subtract(powerUp->getCost()))
    {
        player.addPowerUp(powerUp);
        success = true;
        boughtPowerup = true;
    }
    
    return success;
}