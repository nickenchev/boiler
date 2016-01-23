//
//  Board.h
//  triptych
//
//  Created by Nick Enchev on 2014-09-24.
//
//

#ifndef __triptych__Board__
#define __triptych__Board__

#include <random>
#include <unordered_map>
#include "Array2D.h"
#include "BoardCell.h"
#include "Triptych.h"

class PowerUp;
class Player;

using namespace std;

struct TurnInfo
{
    vector<shared_ptr<PowerUp>> newPowers;
    int levelsGained;
    BoardCell highestCell;
    unsigned int points;
    int combo;
    bool stageIncreased;
    bool gameOver;
    bool boardFull;
    bool canRecover;
    bool bankSlotIncrease;
    bool recovered;
    bool usedPowerup;
    
    TurnInfo()
    {
        levelsGained = 0;
        points = 0;
        combo = 0;
        stageIncreased = false;
        gameOver = false;
        boardFull = false;
        canRecover = false;
        bankSlotIncrease = false;
        recovered = false;
        usedPowerup = false;
    }
};

class Player;

class Board
{
    unordered_map<int, bool> availableNumbersMap;
    vector<BoardCell> availableNumbers;
    ensoft::Array2D<BoardCell> cells;
    TurnInfo turnInfo;
    mt19937 randEngine{random_device{}()};
    discrete_distribution<> numberIndexDist;
    uniform_int_distribution<int> dice1Rand;
    uniform_int_distribution<int> dice2Rand;
    uniform_int_distribution<int> tier1PowerUps;
    uniform_int_distribution<int> tier2PowerUps;
    uniform_int_distribution<int> tier3PowerUps;
    
    BoardCell currentNumber, prevNumber, nextNumber, lastRandomNumber;
    bool triptychCollapse;
    vector<GridPosition> selections;
    PowerUp *currentPowerUp;
    
    void setBoardCells(int numbers[], int stages[]);

public:
    Board(const int rows, const int cols);
    void operator=(const Board &rh);
    
    void logBoard();
    const mt19937 &getRandomEngine() const { return randEngine; }
    void setRandomEngine(const mt19937 &eng) { randEngine = eng; }

    void restart();
    const BoardCell takeNumber();
    void turnBegin();
    const TurnInfo turnEnd(Player &player);
    const TurnInfo &getTurnInfo() const { return turnInfo; }

    const BoardCell &getPrevNumber() const { return prevNumber; }
    const BoardCell &getCurrentNumber() const { return currentNumber; }
    const BoardCell &getNextNumber() const { return nextNumber; }
    void setPrevNumber(const BoardCell &cell) { prevNumber = cell; };
    void setCurrentNumber(const BoardCell &cell) { currentNumber = cell; }
    void setNextNumber(const BoardCell &cell) { nextNumber = cell; }

    BoardCell &operator()(GridPosition pos) { return cells(pos); }
    BoardCell &operator()(int row, int col) { return cells(row, col); }
    
    ensoft::Array2D<BoardCell> &getCells(){ return cells; }
    BoardCell &getCell(unsigned int r, unsigned int c) { return cells(r, c); }
    BoardCell &getCell(GridPosition position) { return cells(position.row, position.column); }
    unsigned int getRows() const { return cells.getRows(); }
    unsigned int getColumns() const { return cells.getColumns(); }
    tuple<bool, GridPosition> findCellPosition(BoardCell *cell);
    
    shared_ptr<PowerUp> rollPowerup(Player &player);
    BoardCell getNewNumber();
    void stepNumbers();
    unsigned long scorePlacement(GridPosition position, Triptych &triptych);

    void findTriptychs(const BoardCell &initialCell, GridPosition prevPosition, GridPosition position, TriptychDirection direction,
                       Triptych &up, Triptych &down, Triptych &left, Triptych &right, Triptych &upLeft, Triptych &upRight, Triptych &downLeft, Triptych &downRight);
    
    void selectPowerUp(PowerUp *powerUp);
    void deselectPowerUp() { currentPowerUp = nullptr; }
    bool isPowerUpSelected() const { return (currentPowerUp != nullptr); }
    bool selectCell(GridPosition position, bool &isDoneSelecting, bool &deselected);
    vector<GridPosition> runPowerUp(Player &player);
    const PowerUp *getCurrentPowerUp() const { return currentPowerUp; }
    vector<GridPosition> &getSelections() { return selections; }
    void addAvailableNumber(const BoardCell cell);
    BoardCell getHighestNumber() const;
};

#endif /* defined(__triptych__Board__) */
