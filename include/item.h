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

enum class ItemPlace {
    GROUND,
    HOTBAR,
    INVENTORY,
    EQUIPED,
    MOUSE,
};

struct ItemEffect {
    std::string effectName;
    int effectValue;
};

class Item
{
public:

    std::vector<ItemEffect> effects;
    ItemType type;
    ItemRarity rarity;
    ItemPlace place;
    std::string name;
    std::string description;
    int id; 
    int stackSize;
    int maxStackSize = 25; //to be changed to 255
    int price;
    Vector2 position;
    Rectangle hitbox;

    Item(int id = -1, Vector2 position = {0, 0}, int stackSize = 0, Rectangle hitbox = {0, 0, 12.0f, 12.0f}, ItemPlace place = ItemPlace::GROUND) : id(id), position(position),stackSize(stackSize), hitbox({position.x,position.y,12.0f,12.0f}), place(place) {}

    void useItem() {

    };

    void placeItem() {

    };

    void displayItemDetails() {

    };
    
    void drawItemPickUp() {

    };
};


#endif
