//
//  Board.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-09-24.
//
//

#include <iostream>
#include <math.h>
#include <memory>
#include <algorithm>

#include "Board.h"
#include "Player.h"
#include "DeleteNumberPowerUp.h"
#include "MoveNumberPowerUp.h"
#include "SwapNumbersPowerUp.h"
#include "CopyNumberPowerUp.h"
#include "BombPowerUp.h"
#include "PromoteNumberPowerUp.h"
#include "BankFromBoardPowerUp.h"
#include "GameSettings.h"

#define MAX_REPETITIONS 1
#define MAX_START_VALUE 3
#define TIER_1_POWERUPS 3
#define TIER_2_POWERUPS 2
#define TIER_3_POWERUPS 2
#define MIN_POWERUP_COST 2
#define POWERUP_DICE_MAX 6
#define WC_DICE_MAX 30
#define MAX_INITIAL_CELL_VAL 5
#define MAX_INITIAL_NUMS 8
#define PROBABILITY_INIT 40;
#define PROBABILITY_GROWTH 5;

using namespace std;

Board::Board(const int rows, const int cols) : cells(rows, cols), numberIndexDist({}), dice1Rand(1, POWERUP_DICE_MAX),
dice2Rand(1, POWERUP_DICE_MAX), tier1PowerUps(1, TIER_1_POWERUPS), tier2PowerUps(1, TIER_2_POWERUPS), tier3PowerUps(1, TIER_3_POWERUPS)
{
    //initialize the board
    restart();
}

void Board::setBoardCells(int numbers[], int stages[])
{
    int arrayIndex = 0;
    for (int r = 1; r <= getRows(); ++r)
    {
        for (int c = 1; c <= getColumns(); ++c)
        {
            cells(r, c) = BoardCell(numbers[arrayIndex], stages[arrayIndex]);
            ++arrayIndex;
        }
    }
}

void Board::operator=(const Board &rh)
{
    availableNumbersMap = rh.availableNumbersMap;
    availableNumbers = rh.availableNumbers;
    cells = rh.cells;
    randEngine = rh.randEngine;
    numberIndexDist = rh.numberIndexDist;
    dice1Rand = rh.dice1Rand;
    dice2Rand = rh.dice2Rand;
    tier1PowerUps = rh.tier1PowerUps;
    tier2PowerUps = rh.tier2PowerUps;
    tier3PowerUps = rh.tier3PowerUps;
    currentNumber = rh.currentNumber;
    prevNumber = rh.prevNumber;
    nextNumber = rh.nextNumber;
    lastRandomNumber = rh.lastRandomNumber;
    triptychCollapse = rh.triptychCollapse;
    currentPowerUp = nullptr;
}
void Board::restart()
{
    //ensure the position coords are setup
    for (int r = 1; r <= cells.getRows(); ++r)
    {
        for (int c = 1; c <= cells.getColumns(); ++c)
        {
            cells(r, c).position.row = r;
            cells(r, c).position.column = c;

            cells(r, c).clear();
        }
    }
    
    //game specific settings
    GameSettings::sharedInstance().shouldRestart = false;
    
    //initialize the available numbers
    availableNumbers.clear();
    availableNumbersMap.clear();
    for (int i = 1; i <= MAX_START_VALUE; ++i)
    {
        BoardCell cell;
        cell.value = i;
        cell.stage = BOARD_CELL_LOWEST_STAGE;
        addAvailableNumber(cell);
    }
    
    //generate the first numbers
    prevNumber = BoardCell();
    currentNumber = getNewNumber();
    nextNumber = getNewNumber();
    lastRandomNumber = currentNumber;
    currentPowerUp = nullptr;
    
    //place some random numbers on the board
    for (int i = 0; i < MAX_INITIAL_NUMS; ++i)
    {
        uniform_int_distribution<int> initNum(1, MAX_INITIAL_CELL_VAL);
        uniform_int_distribution<int> cellRow(1, getRows());
        uniform_int_distribution<int> cellCols(1, getColumns());
        
        int value = initNum(randEngine);
        int row = cellRow(randEngine);
        int col = cellCols(randEngine);
        cells(row, col) = BoardCell(value, 1);
    }
}

const BoardCell Board::takeNumber()
{
    BoardCell number = currentNumber;
    stepNumbers();
    
    return number;
}

void Board::turnBegin()
{
}

shared_ptr<PowerUp> Board::rollPowerup(Player &player)
{
    const int playerLevel = player.getLevelAttribute().getCount();
    int topTier = floor(playerLevel < 3 ? 1 : playerLevel / 3);
    topTier = topTier > 3 ? 3 : topTier;

    shared_ptr<PowerUp> powerUp;
    if (player.hasInventorySpace())
    {
        uniform_int_distribution<int> tierRand(1, topTier);
        int highestTier = tierRand(randEngine);

        if (highestTier == 1)
        {
            switch (tier1PowerUps(randEngine))
            {
                case 1:
                {
                    powerUp = shared_ptr<DeleteNumberPowerUp>(new DeleteNumberPowerUp);
                    break;
                }
                case 2:
                {
                    powerUp = shared_ptr<CopyNumberPowerUp>(new CopyNumberPowerUp);
                    break;
                }
                case 3:
                {
                    powerUp = shared_ptr<PromoteNumberPowerUp>(new PromoteNumberPowerUp);
                    break;
                }
                default:
                    break;
            }
        }
        else if (highestTier == 2)
        {
            switch (tier2PowerUps(randEngine))
            {
                case 1:
                {
                    powerUp = shared_ptr<SwapNumbersPowerUp>(new SwapNumbersPowerUp);
                    break;
                }
                case 2:
                {
                    powerUp = shared_ptr<MoveNumberPowerUp>(new MoveNumberPowerUp);
                    break;
                }
                default:
                    break;
            }
        }
        else if (highestTier == 3)
        {
            switch (tier3PowerUps(randEngine))
            {
                case 1:
                {
                    powerUp = shared_ptr<BombPowerUp>(new BombPowerUp);
                    break;
                }
                case 2:
                {
                    powerUp = shared_ptr<BankFromBoardPowerUp>(new BankFromBoardPowerUp);
                    break;
                }
                default:
                    break;
            }
        }
    }
    
    return powerUp;
}

const TurnInfo Board::turnEnd(Player &player)
{
    //finalize the turn info object for this turn
    turnInfo.points = turnInfo.points * turnInfo.combo;

    if (turnInfo.points)
    {
        turnInfo.levelsGained = player.addPoints(turnInfo.points);

        //give out a powerup for each level gained
        int numPows = turnInfo.levelsGained;
        while (numPows)
        {
            auto pow = rollPowerup(player);
            if (pow)
            {
                player.addPowerUp(pow);
                turnInfo.newPowers.push_back(pow);
            }
            --numPows;
        }
    }
    else
    {
        //check if all the cells are full
        int emptyCells = 0;
        for (int r = 1; r <= cells.getRows(); ++r)
        {
            for (int c = 1; c <= cells.getColumns(); ++c)
            {
                if (cells(r, c).isEmpty())
                {
                    ++emptyCells;
                }
            }
        }
        turnInfo.boardFull = (emptyCells == 0);

        //check for gameover if the board is full
        if (turnInfo.boardFull)
        {
            //check if any of the players powerups can recover the board
            auto &powerUps = player.getPowerUps();
            for (int i = 0; i < powerUps.size(); ++i)
            {
                if (powerUps[i].get()->canRecover(player))
                {
                    turnInfo.canRecover = true;
                }
            }
            //if the player can't recover with power-ups, can he still buy one
            if (!turnInfo.canRecover)
            {
                turnInfo.canRecover = player.getDisksAttribute().getCount() >= MIN_POWERUP_COST;
            }
            
            //if the player doesn't have powerups that can recover, and can't buy any more
            if (!turnInfo.canRecover)
            {
                turnInfo.gameOver = true;
            }
        }
        else if (player.getPreviousTurnInfo().boardFull)
        {
            //the current turn has recovered the board
            turnInfo.recovered = true;
        }
    }
    //track the turn info, and reset for the next turn
    player.setPreviousTurnInfo(turnInfo);
    turnInfo = TurnInfo();
    
    return player.getPreviousTurnInfo();
}

BoardCell Board::getNewNumber()
{
    //figure out which number to give
    long numberIndex = numberIndexDist(randEngine);
    return availableNumbers[numberIndex];
}

void Board::stepNumbers()
{
    prevNumber = currentNumber;
    currentNumber = nextNumber;
    
    nextNumber = getNewNumber();
    
    //try to reduce repitition by trying once more
    if (nextNumber == currentNumber)
    {
        nextNumber = getNewNumber();
    }

    uniform_int_distribution<int> wc1Rand(1, WC_DICE_MAX);
    uniform_int_distribution<int> wc2Rand(1, WC_DICE_MAX);
    int die1 = wc1Rand(randEngine);
    int die2 = wc2Rand(randEngine);

    if (die1 == die2)
    {
        nextNumber.matchAll = true;
        nextNumber.setLowest();
    }
}

void Board::addAvailableNumber(const BoardCell cell)
{
    bool alreadyAdded = availableNumbersMap[cell.getId()];
    if (!alreadyAdded)
    {
        availableNumbers.push_back(cell);
        availableNumbersMap[cell.getId()] = true;
        
        //make sure the distribution is recreated to reflect the newly added available number
        auto numAvailNums = availableNumbers.size();
        vector<int> probabilities;
        int probability = PROBABILITY_INIT;
        for (int i = 0; i < numAvailNums; ++i)
        {
            probabilities.push_back(probability);
            probability += PROBABILITY_GROWTH;
        }
        numberIndexDist = discrete_distribution<>(probabilities.begin(), probabilities.end());

        cout << "Added cell with ID: " << cell.getId() << endl;
    }
}

BoardCell Board::getHighestNumber() const
{
    return availableNumbers[availableNumbers.size() - 1];
}

unsigned long Board::scorePlacement(GridPosition position, Triptych &triptych)
{
    Triptych up(TriptychDirection::UP), down(TriptychDirection::DOWN), left(TriptychDirection::LEFT), right(TriptychDirection::RIGHT), upLeft(TriptychDirection::UP_LEFT), upRight(TriptychDirection::UP_RIGHT), downLeft(TriptychDirection::DOWN_LEFT), downRight(TriptychDirection::DOWN_RIGHT);
    
    vector<const Triptych *> triptychs = { &left, &upLeft, &up, &upRight, &right, &downRight, &down, &downLeft };
    vector<const Triptych *> triptychsAlt = { &right, &downRight, &down, &downLeft, &left, &upLeft, &up, &upRight };
    BoardCell &cell = cells(position);
    
    //kick off the recursive search for triptychs in all directions
    findTriptychs(cell, cell.position, cell.position, TriptychDirection::NONE, up, down, left, right, upLeft, upRight, downLeft, downRight);

    unsigned int score = 0;
    int numTriptychs = 0;
    int triptychSize = 0;
    
    unordered_map<int, bool> distinctNumbers;
    
    //scan every directional triptych
    for (int i = 0; i < triptychs.size(); ++i)
    {
        const Triptych &tt = *triptychs[i];
        const Triptych &ttAlt = *triptychsAlt[i];
        
        //check both directions for a possible triptych
        bool foundMatches = false;
        if (tt.size() >= 2)
        {
            foundMatches = true;
        }
        else if (tt.size() == 1 && ttAlt.size() > 0)
        {
            foundMatches = cells(tt[0]) == cells(ttAlt[0]); //returns whether the opposite cells are equal
        }

        //current cell position is part of a triptych
        if (foundMatches)
        {
            const BoardCell &firstCell = cells(tt[0].row, tt[0].column);
            distinctNumbers[firstCell.getId()] = true;
            addAvailableNumber(firstCell);
            
            triptychSize += tt.size();
            numTriptychs++;
            for (int i = 0; i < tt.size(); ++i)
            {
                BoardCell &currentCell = cells(tt[i].row, tt[i].column);
                triptych.addPosition(currentCell.position);
                if (currentCell > turnInfo.highestCell) { turnInfo.highestCell = currentCell; }
            }
        }
    }
    
    //clear out the triptyched cells
    for (int i = 0; i < triptych.size(); ++i)
    {
        BoardCell &cell = cells(triptych[i]);
        cell.clear();
    }

    //set the current number to the value of the triptych + 1
    if (numTriptychs)
    {
        if (!cell.matchAll)
        {
            ++triptychSize; //add 1 to account for the placement cell
            //ensure the stage is set prior to increasing the cell value (which could increase the stage on its own)
            if (turnInfo.highestCell.stage > cell.stage)
            {
                cell.stage = turnInfo.highestCell.stage;
            }
            turnInfo.combo++;
        }
        else
        {
            triptychSize += numTriptychs; //add based on number of triptych matches
            //turn the wildcard into the highest number for this turn, disable the matchall
            cell = turnInfo.highestCell;
            cell.matchAll = false;
            
            turnInfo.combo = static_cast<int>(distinctNumbers.size());
        }
        score = turnInfo.highestCell.getScoreValue() * triptychSize;
        
        bool stageIncreased = cell.increaseValue();
        if (stageIncreased)
        {
            turnInfo.stageIncreased = true;
            turnInfo.highestCell = cell;
        }

        turnInfo.points += score;
    }
    
    return score;
}

void Board::findTriptychs(const BoardCell &initialCell, GridPosition prevPosition, GridPosition position, TriptychDirection direction,
                          Triptych &up, Triptych &down, Triptych &left, Triptych &right, Triptych &upLeft, Triptych &upRight, Triptych &downLeft, Triptych &downRight)
{
    const BoardCell &cell = cells(position);
    GridPosition adjPos;

    if (cells.getLeft(position, adjPos) && (direction == TriptychDirection::LEFT || direction == TriptychDirection::NONE))
    {
        const BoardCell &adjCell = cells(adjPos);
        if (cell == adjCell && adjPos != prevPosition && adjCell == initialCell)
        {
            left.addPosition(adjPos);
            findTriptychs(initialCell, position, adjPos, TriptychDirection::LEFT, up, down, left, right, upLeft, upRight, downLeft, downRight);
        }
    }
    if (cells.getTopLeft(position, adjPos) && (direction == TriptychDirection::UP_LEFT || direction == TriptychDirection::NONE))
    {
        const BoardCell &adjCell = cells(adjPos);
        if (cell == adjCell && adjPos != prevPosition && adjCell == initialCell)
        {
            upLeft.addPosition(adjPos);
            findTriptychs(initialCell, position, adjPos, TriptychDirection::UP_LEFT, up, down, left, right, upLeft, upRight, downLeft, downRight);
        }
    }
    if (cells.getTop(position, adjPos) && (direction == TriptychDirection::UP || direction == TriptychDirection::NONE))
    {
        const BoardCell &adjCell = cells(adjPos);
        if (cell == adjCell && adjPos != prevPosition && adjCell == initialCell)
        {
            up.addPosition(adjPos);
            findTriptychs(initialCell, position, adjPos, TriptychDirection::UP, up, down, left, right, upLeft, upRight, downLeft, downRight);
        }
    }
    if (cells.getTopRight(position, adjPos) && (direction == TriptychDirection::UP_RIGHT || direction == TriptychDirection::NONE))
    {
        const BoardCell &adjCell = cells(adjPos);
        if (cell == adjCell && adjPos != prevPosition && adjCell == initialCell)
        {
            upRight.addPosition(adjPos);
            findTriptychs(initialCell, position, adjPos, TriptychDirection::UP_RIGHT, up, down, left, right, upLeft, upRight, downLeft, downRight);
        }
    }
    if (cells.getRight(position, adjPos) && (direction == TriptychDirection::RIGHT || direction == TriptychDirection::NONE))
    {
        const BoardCell &adjCell = cells(adjPos);
        if (cell == adjCell && adjPos != prevPosition && adjCell == initialCell)
        {
            right.addPosition(adjPos);
            findTriptychs(initialCell, position, adjPos, TriptychDirection::RIGHT, up, down, left, right, upLeft, upRight, downLeft, downRight);
        }
    }
    if (cells.getBottomRight(position, adjPos) && (direction == TriptychDirection::DOWN_RIGHT || direction == TriptychDirection::NONE))
    {
        const BoardCell &adjCell = cells(adjPos);
        if (cell == adjCell && adjPos != prevPosition && adjCell == initialCell)
        {
            downRight.addPosition(adjPos);
            findTriptychs(initialCell, position, adjPos, TriptychDirection::DOWN_RIGHT, up, down, left, right, upLeft, upRight, downLeft, downRight);
        }
    }
    if (cells.getBottom(position, adjPos) && (direction == TriptychDirection::DOWN || direction == TriptychDirection::NONE))
    {
        const BoardCell &adjCell = cells(adjPos);
        if (cell == adjCell && adjPos != prevPosition && adjCell == initialCell)
        {
            down.addPosition(adjPos);
            findTriptychs(initialCell, position, adjPos, TriptychDirection::DOWN, up, down, left, right, upLeft, upRight, downLeft, downRight);
        }
    }
    if (cells.getBottomLeft(position, adjPos) && (direction == TriptychDirection::DOWN_LEFT || direction == TriptychDirection::NONE))
    {
        const BoardCell &adjCell = cells(adjPos);
        if (cell == adjCell && adjPos != prevPosition && adjCell == initialCell)
        {
            downLeft.addPosition(adjPos);
            findTriptychs(initialCell, position, adjPos, TriptychDirection::DOWN_LEFT, up, down, left, right, upLeft, upRight, downLeft, downRight);
        }
    }
}

void Board::selectPowerUp(PowerUp *powerUp)
{
    selections.clear();
    currentPowerUp = powerUp;
}

bool Board::selectCell(GridPosition position, bool &isDoneSelecting, bool &deselected)
{
    isDoneSelecting = false;
    deselected = false;

    bool selectionSuccess = false;
    BoardCell &cell = cells(position.row, position.column);
    
    //check if this position has already been picked and deselect
    for (auto itr = selections.begin(); itr != selections.end(); ++itr)
    {
        if (*itr == position)
        {
            deselected = true;
        }
    }
    if (deselected)
    {
        //remove the deselected position from the list
        selections.erase(std::remove(selections.begin(), selections.end(), position), selections.end());
    }

    long selectionNumberIndex = selections.size();
    if (!deselected)
    {
        //make sure its not a bad selection
        selectionSuccess = currentPowerUp->isValidSelection((int)selectionNumberIndex, cell);

        if (selectionSuccess)
        {
            selections.push_back(position);
            
            if (selections.size() == currentPowerUp->getNumSelections())
            {
                isDoneSelecting = true;
            }
        }
    }
    
    return selectionSuccess;
}

vector<GridPosition> Board::runPowerUp(Player &player)
{
    auto retval = currentPowerUp->run(*this, player);
    
    //remove the powerup from the player
    player.removePowerUp(currentPowerUp);
    currentPowerUp = nullptr;
    turnInfo.usedPowerup = true;
    
    return retval;
}

void Board::logBoard()
{
    //log the board
    for (int r = 1; r <= getRows(); ++r)
    {
        for (int c = 1; c <= getColumns(); ++c)
        {
            cout << "[" << getCell(r, c).value << "]";
        }
        cout << endl;
    }
    cout << endl;
}
