//
//  Player.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-09-27.
//
//

#include "Player.h"

#define LEVEL_INIT 1
#define LEVEL_BASE 500
#define LEVEL_MULT 1.33f
#define DISKS_INIT 0
#define DISKS_BASE 200
#define DISKS_MULT 1.08f

using namespace std;

Player::Player() : level(LEVEL_INIT, LEVEL_BASE, LEVEL_MULT), disks(DISKS_INIT, DISKS_BASE, DISKS_MULT)
{
    reset();
}

void Player::operator=(const Player &rh)
{
    level = rh.level;
    disks = rh.disks;
    powerUps = rh.powerUps;
    bankCells = rh.bankCells;
    numBankSlots = rh.numBankSlots;
    selectedBankCellIndex = PLAYER_BANK_NOT_SELECT_INDEX;
}

void Player::reset()
{
    level = GameAttribute(LEVEL_INIT, LEVEL_BASE, LEVEL_MULT);
    disks = GameAttribute(DISKS_INIT, DISKS_BASE, DISKS_MULT);
    bankCells.clear();
    powerUps.clear();
    numBankSlots = PLAYER_INITIAL_BANK_SLOTS;
    selectedBankCellIndex = PLAYER_BANK_NOT_SELECT_INDEX;
}

int Player::addPoints(unsigned int points)
{
    disks.addPoints(points, &level);
    int levelsGained = level.addPoints(points);

    return levelsGained;
}

float Player::getLevelCompletion() const
{
    return level.getCompletion();
}

void Player::addPowerUp(shared_ptr<PowerUp> powerUp)
{
    //store the powerup in the board
    powerUps.push_back(powerUp);
}

void Player::removePowerUp(PowerUp *powerUp)
{
    bool done = false;
    for (auto itr = powerUps.begin(); itr != powerUps.end() && !done; ++itr)
    {
        if (itr->get() == powerUp)
        {
            done = true;
            powerUps.erase(itr);
        }
    }
}

PowerUp *Player::getPowerUp(int index) const
{
    PowerUp *powerUp = nullptr;
    if (powerUps.size() >= index + 1)
    {
        powerUp = powerUps[index].get();
    }
    
    return powerUp;
}

bool Player::bankCell(const BoardCell &cell)
{
    bool added = false;
    if (bankCells.size() < numBankSlots)
    {
        bankCells.push_back(cell);
        added = true;
    }
    
    return added;
}

void Player::consumeSelectedBankCell()
{
    int index = 0;
    bool done = false;
    for (auto itr = bankCells.begin(); itr != bankCells.end() && !done; ++itr)
    {
        if (index == selectedBankCellIndex)
        {
            bankCells.erase(itr);
            done = true;
        }
        index++;
    }
    deselectBankCell();
}

int Player::getBankSlotCost() const
{
    return BANK_SLOT_COST + (numBankSlots - 1);
}

bool Player::increaseBankSlots()
{
    bool hadSufficient = false;
    
    if (numBankSlots < PLAYER_BANK_MAX_SIZE && disks.subtract(getBankSlotCost()))
    {
        hadSufficient = true;
        numBankSlots++;
    }
    
    return hadSufficient;
}