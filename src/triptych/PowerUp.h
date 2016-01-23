//
//  PowerUp.h
//  triptych
//
//  Created by Nick Enchev on 2014-10-02.
//
//

#ifndef __triptych__PowerUp__
#define __triptych__PowerUp__

#include <vector>
#include <string>
#include "Board.h"
#include "GridPosition.h"

using namespace std;

class Player;

enum class ValueValidation
{
    NO_REQUIREMENT,
    REQUIRE_NON_EMPTY,
    REQUIRE_EMPTY
};

enum class PowerUpRequirement
{
    BANK_SLOT
};

struct SelectionRule
{
    static SelectionRule create(ValueValidation validation)
    {
        return SelectionRule { validation };
    }
    
    ValueValidation valueValidation;
};

class Board;

class PowerUp
{
protected:
    int tier;
    bool newlyAdded;
    string name;
    int cost;
    string code;
    string help;
    vector<PowerUpRequirement> requirements;
    vector<SelectionRule> selectionRules;

public:
    PowerUp();
    
    static unique_ptr<PowerUp> createPowerUp(string powerUpCode);

    vector<string> validateRequirements(const Board &board, const Player &player);
    bool isValidSelection(int selectionNumber, const BoardCell &cell) const;
    virtual vector<GridPosition> run(Board &board, Player &player) = 0;
    
    int getNumSelections() const { return static_cast<int>(selectionRules.size()); }
    int getTier() const { return tier; }
    bool isNewlyAdded() const { return newlyAdded; }
    void setAcknowledged() { newlyAdded = false; }
    const string &getName() const { return name; }
    int getCost() const { return cost; }
    const string &getCode() const { return code; }
    const string &getHelp() const { return help; }

    virtual bool canRecover(const Player &player) const = 0;
};

#endif /* defined(__triptych__PowerUp__) */
