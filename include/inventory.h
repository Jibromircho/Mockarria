#ifndef INVENTORY_H
#define INVENTORY_H

#include <config.h>

struct Slot {
    int itemID = -1;
    bool selected = false;
    int stack = 0;
    bool placeable = false;

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
                slots[i][j].placeable = false;

                if (j == 0 && i == hotbarIndex) {
                    slots[i][j].selected = true;
                } else slots[i][j].selected = false;
            }
        }
    }

    void selectHotbarSlot(int hotbarSlot) {
        for (int i = 0; i < hotbarSize; ++i) {
            if (hotbarSlot == i) {
                hotbarIndex = i;
                slots[hotbarSlot][0].selected = true;
            } else slots[i][0].selected = false;
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

    bool isPlaceable(int id) {
        if (slots[hotbarIndex][0].placeable) {
            return 1;
        } else return 0;
    }

    int useItem(int id) {
        if (slots[hotbarIndex][0].stack > 0) {
            slots[hotbarIndex][0].stack -= 1;
            return slots[hotbarIndex][0].itemID;
        } else {
            slots[hotbarIndex][0].itemID = -1;
            return slots[hotbarIndex][0].itemID;
        }
    }

    void resetInventory() {
        for (int i = 0; i < hotbarSize; ++i) {
            for (int j = 0; j < inventoryRows; ++j) {
                slots[i][j].itemID = -1;
                slots[i][j].stack = 0;
                slots[i][j].placeable = false;

                if (j == 0 && i == hotbarIndex) {
                    slots[i][j].selected = true;
                } else slots[i][j].selected = false;
            }
        }
    }

    void drawHotbarItems(Tile tile,Vector2 position) {
        for (int i = 0; i < hotbarSize; i++) {
            float scaleFactor = 1.0f;
            if (slots[i][0].itemID >= 0 && slots[i][0].stack > 0) {
                int xOffset = 23;
                int yOffset = 23;
                Rectangle sourceRec = tile.getIconRecSource(slots[i][0].itemID);
                Vector2 origin = { sourceRec.width / 2, sourceRec.height / 2 };
                if (slots[i][0].selected == true) {
                    scaleFactor = 1.9f;
                } else if (slots[i][0].selected == false) {
                    scaleFactor = 1.6f;
                };

                Rectangle destRec = { 
                position.x + (i * 48.0f) + (xOffset / 2), 
                position.y + (yOffset / 2),       
                sourceRec.width * scaleFactor,            
                sourceRec.height * scaleFactor         
                };

                DrawTexturePro(tile.tileSet, sourceRec, destRec, origin, 0.0f, WHITE);

                Font font = GetFontDefault();
                DrawTextEx( font, (std::to_string(slots[i][0].stack).c_str()), {position.x + (i * 48.0f) + (xOffset ), position.y + (yOffset )}, 8 * scaleFactor, 2,WHITE);
                DrawTextEx( font, (std::to_string(slots[i][0].stack).c_str()), {position.x + (i * 48.0f) + (xOffset ) + 0.3f, position.y + (yOffset ) + 0.3f}, 8 * scaleFactor, 2,WHITE);
                DrawTextEx( font, (std::to_string(slots[i][0].stack).c_str()), {position.x + (i * 48.0f) + (xOffset ) + 0.6f, position.y + (yOffset ) + 0.6f}, 8 * scaleFactor, 2,WHITE);

            }
        }
    }
    void drawInventoryItems(Tile tile, Vector2 position) {
        
    }

};

#endif
