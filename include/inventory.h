#ifndef INVENTORY_H
#define INVENTORY_H

#include <config.h>
#include <item.h>


struct Slot {
    int itemID = -1;
    bool empty = false;
    Item item;

};


class Inventory 
{

public:

    static constexpr int hotbarSize = 10;
    static constexpr int inventoryRows = 6;
    int hotbarIndex = 0;
    bool inventoryOpened = false;

    Slot slots[hotbarSize][inventoryRows];

    
    Inventory() {
        for (int i = 0; i < hotbarSize; ++i) {
            for (int j = 0; j < inventoryRows; ++j) {
                slots[i][j].itemID = -1;
                slots[i][j].empty = true;
            }
        }
    }


    void addItem(int id, int quantity) {
        for (int j = 0; j < inventoryRows; ++j) {
            for (int i = 0; i < hotbarSize; ++i) {
                if (slots[i][j].itemID == -1) {
                    slots[i][j].itemID = id;
                    slots[i][j].empty = false;
                    return;
                }
                else if (slots[i][j].itemID == id) {
                    slots[i][j].item.stackSize += quantity;
                    return;
                }
            }
        }
    }

    void resetInventory() {
        for (int i = 0; i < hotbarSize; ++i) {
            for (int j = 0; j < inventoryRows; ++j) {
                slots[i][j].itemID = -1;
                slots[i][j].empty = true;
                hotbarIndex = 0;
            }
        }
    }

    void drawHotbarItems(Tile* tile,Vector2 position) {
        for (int i = 0; i < hotbarSize; i++) {
            float scaleFactor = 1.0f;
            if (!slots[i][0].empty) {
                int xOffset = 23;
                int yOffset = 23;
                Rectangle sourceRec = tile->getIconRecSource(slots[i][0].itemID);
                Vector2 origin = { sourceRec.width / 2, sourceRec.height / 2 };
                if (i == hotbarIndex) {
                    scaleFactor = 2.25f;
                } else scaleFactor = 2.0f;

                Rectangle destRec = { 
                position.x + (i * 48.0f) + (xOffset / 1.75f), 
                position.y + (yOffset / 1.75f),       
                sourceRec.width * scaleFactor,            
                sourceRec.height * scaleFactor         
                };

                DrawTexturePro(tile->tileSet, sourceRec, destRec, origin, 0.0f, WHITE);

                Font font = GetFontDefault();
                DrawTextEx( font, (std::to_string(slots[i][0].item.stackSize).c_str()), {position.x + (i * 48.0f) + (xOffset ), position.y + (yOffset )}, 8 * scaleFactor, 2,WHITE);
                DrawTextEx( font, (std::to_string(slots[i][0].item.stackSize).c_str()), {position.x + (i * 48.0f) + (xOffset ) + 0.3f, position.y + (yOffset ) + 0.3f}, 8 * scaleFactor, 2,WHITE);
                DrawTextEx( font, (std::to_string(slots[i][0].item.stackSize).c_str()), {position.x + (i * 48.0f) + (xOffset ) + 0.6f, position.y + (yOffset ) + 0.6f}, 8 * scaleFactor, 2,WHITE);

            }
        }
    }
    void drawInventoryItems(Tile* tile, Vector2 position) {
        
    }

};

#endif
