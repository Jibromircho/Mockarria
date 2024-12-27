#ifndef ITEM_H
#define ITEM_H

#include <config.h>

enum class ItemType {
    WEAPON,
    CONSUMABLE,
    TOOL,
    MATERIAL,
    ARMOUR,
    STRUCTURE,

};

enum class ItemRarity {
    COMMON,
    UNCOMMON,
    RARE,
    EPIC,
    LEGENDARY
};

struct ItemEffect {
    std::string effectName;
    int effectValue;
};

class Item
{
public:

    std::vector<ItemEffect> effects;
    Texture2D texture = LoadTexture("../img/items/weapon_1.png");
    ItemType type;
    ItemRarity rarity;
    std::string name;
    std::string description;
    int id = -1;  
    int stackSize = 0;
    int maxStackSize = 255; 
    int price;
    
    void useItem() {

    };

    void placeItem() {

    };

    void dropItem() {

    };

    void displayItemDetails() {

    };
};


#endif
