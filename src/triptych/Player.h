//
//  Player.h
//  triptych
//
//  Created by Nick Enchev on 2014-09-27.
//
//

#ifndef __triptych__Player__
#define __triptych__Player__

#include <vector>
#include "PowerUp.h"
#include "GameCore/GameAttribute.h"

#define PLAYER_INVENTORY_SIZE 6
#define PLAYER_BANK_MAX_SIZE 5
#define PLAYER_INITIAL_BANK_SLOTS 1
#define PLAYER_LEVELS_PER_BANK_INCREASE 3
#define PLAYER_BANK_NOT_SELECT_INDEX -1
#define BANK_SLOT_COST 5

class Player
{
    GameAttribute level;
    GameAttribute disks;
    std::vector<std::shared_ptr<PowerUp>> powerUps;
    std::vector<BoardCell> bankCells;
    int numBankSlots;
    int selectedBankCellIndex;
    TurnInfo previousTurnInfo;
    
public:
    Player();
    void operator=(const Player &rh);
    
    GameAttribute &getLevelAttribute() { return level; }
    void setLevelAttribute(const GameAttribute &level) { this->level = level; }
    GameAttribute &getDisksAttribute() { return disks; }
    void setDisksAttribute(const GameAttribute &disks) { this->disks = disks; }
    void setPreviousTurnInfo(TurnInfo &turnInfo) { previousTurnInfo = turnInfo; }
    const TurnInfo &getPreviousTurnInfo() const { return previousTurnInfo; }
    
    void reset();
    int addPoints(unsigned int points);
    float getLevelCompletion() const;
    
    void coinsUpdated(GameAttribute &target);
    
    //powerups
    void addPowerUp(shared_ptr<PowerUp> powerUp);
    void removePowerUp(PowerUp *powerUp);
    PowerUp *getPowerUp(int index) const;
    const std::vector<std::shared_ptr<PowerUp>> &getPowerUps() const { return powerUps; };
    bool hasInventorySpace() const { return (powerUps.size() < PLAYER_INVENTORY_SIZE); }
    bool hasPowerUps() const { return (powerUps.size()); }
    
    //bank cells
    const vector<BoardCell> &getBankCells() const { return bankCells; }
    bool bankSlotHasNumber(const int bankIndex) const { return (bankIndex < bankCells.size()); }
    const BoardCell &getBankCell(const int bankIndex) const { return bankCells[bankIndex]; }
    bool canBank() const { return getBankCellSlotCount() > bankCells.size(); }
    bool bankCell(const BoardCell &cell);
    int getBankSlotCost() const;
    bool increaseBankSlots();
    void setBankSlots(const int slotCount) { numBankSlots = slotCount; }
    int getBankCellSlotCount() const { return numBankSlots; }
    void selectBankCell(const int bankCellIndex) { selectedBankCellIndex = bankCellIndex; }
    int getSelectedBankCellIndex() const { return selectedBankCellIndex; }
    bool bankSlotSelected() const { return (selectedBankCellIndex != PLAYER_BANK_NOT_SELECT_INDEX); }
    const BoardCell &getSelectedBankCell() const { return bankCells[selectedBankCellIndex]; }
    void deselectBankCell() { selectedBankCellIndex = PLAYER_BANK_NOT_SELECT_INDEX; }
    void consumeSelectedBankCell();
};

#endif /* defined(__triptych__Player__) */
