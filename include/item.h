#ifndef TILE_H
#define TILE_H

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
    Rectangle texture;  
    ItemType type;
    ItemRarity rarity;
    std::string name;
    std::string description;
    int id;  
    int stackSize; 
    int price;

    Item(std::string itemName, ItemType itemType, ItemRarity itemRarity, Rectangle itemTexture, int itemId, 
         int itemStackSize, int itemPrice)
        : name(itemName), type(itemType), rarity(itemRarity), texture(itemTexture), id(itemId), 
          stackSize(itemStackSize), price(itemPrice) {}

    
    void useItem();

    void placeItem();

    void dropItem();

    void displayItemDetails();
};


#endif
