//
//  PowerUp.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-10-02.
//
//

#include "Player.h"
#include "PowerUp.h"
#include "DeleteNumberPowerUp.h"
#include "PromoteNumberPowerUp.h"
#include "SwapNumbersPowerUp.h"
#include "MoveNumberPowerUp.h"
#include "BombPowerUp.h"
#include "CopyNumberPowerUp.h"
#include "BankFromBoardPowerUp.h"

PowerUp::PowerUp()
{
    tier = 1;
    cost = 0;
    newlyAdded = true;
    name = "ERROR";
    code = "ERR";
    help = "NO HELP DEFINED";
}

vector<string> PowerUp::validateRequirements(const Board &board, const Player &player)
{
    vector<string> errors;
    for (PowerUpRequirement req : requirements)
    {
        if (req == PowerUpRequirement::BANK_SLOT)
        {
            if (!player.canBank())
            {
                errors.push_back("One free bank slot\nis required.");
            }
        }
    }
    
    return errors;
}

bool PowerUp::isValidSelection(int selectionNumber, const BoardCell &cell) const
{
    bool isValid = false;
    if (selectionNumber < selectionRules.size())
    {
        SelectionRule rule = selectionRules[selectionNumber];
        if (rule.valueValidation == ValueValidation::REQUIRE_EMPTY && cell.isEmpty())
        {
            isValid = true;
        }
        else if (rule.valueValidation == ValueValidation::REQUIRE_NON_EMPTY && !cell.isEmpty())
        {
            isValid = true;
        }
        else if (rule.valueValidation == ValueValidation::NO_REQUIREMENT)
        {
            isValid = true;
        }
    }
    
    return isValid;
}

unique_ptr<PowerUp> PowerUp::createPowerUp(string powerUpCode)
{
    unique_ptr<PowerUp> powerUp;
    if (powerUpCode == "DEL")
    {
        powerUp = unique_ptr<DeleteNumberPowerUp>(new DeleteNumberPowerUp);
    }
    else if (powerUpCode == "PMT")
    {
        powerUp = unique_ptr<PromoteNumberPowerUp>(new PromoteNumberPowerUp);
    }
    else if (powerUpCode == "CPY")
    {
        powerUp = unique_ptr<CopyNumberPowerUp>(new CopyNumberPowerUp);
    }
    else if (powerUpCode == "MOV")
    {
        powerUp = unique_ptr<MoveNumberPowerUp>(new MoveNumberPowerUp);
    }
    else if (powerUpCode == "SWP")
    {
        powerUp = unique_ptr<SwapNumbersPowerUp>(new SwapNumbersPowerUp);
    }
    else if (powerUpCode == "BMB")
    {
        powerUp = unique_ptr<BombPowerUp>(new BombPowerUp);
    }
    else if (powerUpCode == "BNK")
    {
        powerUp = unique_ptr<BankFromBoardPowerUp>(new BankFromBoardPowerUp);
    }
    
    return powerUp;
}
