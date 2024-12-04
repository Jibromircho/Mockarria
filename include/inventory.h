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
                slots[i][j].itemID = -1;
                slots[i][j].stack = 0;

                if (j == 0 && i == hotbarIndex) {
                    slots[i][j].selected = true;
                } else slots[i][j].selected = false;
            }
        }
    }

    void selectHotbarSlot() {

        for (int i = 0; i < hotbarSize; ++i) {
            slots[i][0].selected = false;
        }

        for (int i = 0; i < hotbarSize; ++i) {
            if (IsKeyPressed('0' + ((i + 1) % 10))) {
                hotbarIndex = i;
                slots[i][0].selected = true;
                break;
            }
        }
    }

    void addItem(int id, int quantity) {
        for (int i = 0; i < hotbarSize; ++i) {
            for (int j = 0; j < inventoryRows; ++j) {
                if (slots[i][j].itemID == -1) slots[i][j].itemID = id;
                else if (slots[i][j].itemID == id) slots[i][j].stack += quantity;    
            }
        }
    }

    void drawHotbarItems(Tile tile,Vector2 position) {
        for (int i = 0; i < hotbarSize; i++) {
            if (slots[i][0].itemID >= 0) DrawTextureRec(tile.tileSet, tile.dirtCenter,position,WHITE);
        }
    }

};

#endif
