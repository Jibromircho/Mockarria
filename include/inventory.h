#ifndef INVENTORY_H
#define INVENTORY_H

#include <config.h>

struct Slot {
    int itemID = -1;
    bool selected = false;
    int stack = 0;

};

class Inventory 
{

public:
    static constexpr int hotbarSize = 10;
    static constexpr int inventoryRows = 6;

    int hotbarIndex = 0;

    Slot slots[hotbarSize][inventoryRows];
    
    Inventory() {
        for (int i = 0; i < hotbarSize; ++i) {
            for (int j = 0; j < inventoryRows; ++j) {
                slots[i][j].itemID = -1; // Initialize as empty
                slots[i][j].selected = false;
                slots[i][j].stack = 0;
            }
        }
    }
};

#endif
