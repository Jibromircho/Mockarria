#ifndef INVENTORY_H
#define INVENTORY_H

#include <config.h>

struct Slot {
    int itemID = -1;
    bool selected = false;
    int stack = 0;
    bool placeable = true;

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
        for (int j = 0; j < inventoryRows; ++j) {
            for (int i = 0; i < hotbarSize; ++i) {
                if (slots[i][j].itemID == -1) {
                    slots[i][j].itemID = id;
                    slots[i][j].stack = quantity;
                    return;
                }
                else if (slots[i][j].itemID == id) {

                    slots[i][j].stack += quantity;
                    return;
                }
            }
        }
    }
    
    int useItem(int id) {
        if (slots[hotbarIndex][0].stack > 0) {
            slots[hotbarIndex][0].stack -= 1;
            return slots[hotbarIndex][0].itemID;
        } else return -1;
    }

    void drawHotbarItems(Tile tile,Vector2 position) {
        for (int i = 0; i < hotbarSize; i++) {
            if (slots[i][0].itemID >= 0) {
                int xOffset = 20;
                int yOffset = 20;
                DrawTextureRec(tile.tileSet, tile.getIconRecSource(slots[i][0].itemID),{position.x + (i * 48.0f) + (xOffset/2), position.y + (yOffset/2)},WHITE);

                Font font = GetFontDefault();
                DrawTextEx( font, (std::to_string(slots[i][0].stack).c_str()), {position.x + (i * 48.0f) + xOffset, position.y + yOffset}, 20, 2,BLACK);
                DrawTextEx( font, (std::to_string(slots[i][0].stack).c_str()), {position.x + (i * 48.0f) + xOffset + 0.3f, position.y + yOffset + 0.3f}, 20, 2,BLACK);
                DrawTextEx( font, (std::to_string(slots[i][0].stack).c_str()), {position.x + (i * 48.0f) + xOffset + 0.6f, position.y + yOffset + 0.6f}, 20, 2,BLACK);
                DrawTextEx( font, (std::to_string(slots[i][0].stack).c_str()), {position.x + (i * 48.0f) + xOffset + 0.9f, position.y + yOffset + 0.9f}, 20, 2,BLACK);
            }
        }
    }

};

#endif
