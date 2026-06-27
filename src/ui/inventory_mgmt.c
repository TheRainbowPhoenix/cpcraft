/* src/ui/inventory_mgmt.c - Add/remove items from hotbar and inventory */
#include "engine.h"
#include "config.h"


void addItemToHotbar(int itemIndex, int amount)
{
    for (int i = 0; i < 9; i++)
    {
        if((hotbarBlockTypes[i] == itemIndex || hotbarBlockTypes[i] == -1) && hotbarBlockAmount[i] + amount <= 64)
        {
            hotbarBlockTypes[i] = itemIndex;
            hotbarBlockAmount[i] += amount;

            if(hotbarBlockTypes[currentSlot] != -1)
            blockSelected = hotbarBlockTypes[currentSlot];
            else
            blockSelected = -1;

            break;
        }
    }
    
}
void addItemToInventory(int itemIndex, int amount)
{
    for (int i = 0; i < 27; i++)
    {
        if((inventoryBlockTypes[i] == itemIndex || inventoryBlockTypes[i] == -1) && inventoryBlockAmount[i] + amount <= 64)
        {
            inventoryBlockTypes[i] = itemIndex;
            inventoryBlockAmount[i] += amount;
			if(allItem[itemIndex].breaksIn != 0)
			inventoryBlockHp[i] = allItem[itemIndex].breaksIn;

            if(inventoryBlockTypes[currentSlot] != -1)
            blockSelected = inventoryBlockTypes[currentSlot];
            else
            blockSelected = -1;

            break;
        }
    }
    
}
void addItemToHotbarInventory(int itemIndex, int amount)
{
    char setItem = 0;
    for (int i = 0; i < 9; i++)
    {
        if(hotbarBlockTypes[i] == itemIndex && hotbarBlockAmount[i] + amount <= 64 && amount > 0)
        {
            setItem = 1;
            hotbarBlockTypes[i] = itemIndex;
			if(hotbarBlockAmount[i] == 0)
			hotbarBlockHp[i] = allItem[itemIndex].breaksIn;
            hotbarBlockAmount[i] += amount;

            if(hotbarBlockTypes[currentSlot] != -1)
            blockSelected = hotbarBlockTypes[currentSlot];
            else
            blockSelected = -1;

            amount -= 64;

            break;
        }
    }
    for (int i = 0; i < 9; i++)
    {
        if(hotbarBlockTypes[i] == -1 && hotbarBlockAmount[i] + amount <= 64 && amount > 0)
        {
            setItem = 1;
            hotbarBlockTypes[i] = itemIndex;
			if(hotbarBlockAmount[i] == 0)
			hotbarBlockHp[i] = allItem[itemIndex].breaksIn;
            hotbarBlockAmount[i] += amount;

            if(hotbarBlockTypes[currentSlot] != -1)
            blockSelected = hotbarBlockTypes[currentSlot];
            else
            blockSelected = -1;

            amount -= 64;

            break;
        }
    }
    if(setItem == 0)
    {
        for (int i = 0; i < 27; i++)
        {
            if(inventoryBlockTypes[i] == itemIndex && inventoryBlockAmount[i] + amount <= 64 && amount > 0)
            {
                inventoryBlockTypes[i] = itemIndex;
				if(inventoryBlockAmount[i] == 0)
				inventoryBlockHp[i] = allItem[itemIndex].breaksIn;
                inventoryBlockAmount[i] += amount;
                amount -= 64;

                break;
            }
        }
        for (int i = 0; i < 27; i++)
        {
            if(inventoryBlockTypes[i] == -1 && inventoryBlockAmount[i] + amount <= 64 && amount > 0)
            {
                inventoryBlockTypes[i] = itemIndex;
				if(inventoryBlockAmount[i] == 0)
				inventoryBlockHp[i] = allItem[itemIndex].breaksIn;
                inventoryBlockAmount[i] += amount;

                break;
            }
        }
    }
}
void removeItemFromHotbar(int itemIndex, int amount)
{
    for (int i = 0; i < 9; i++)
    {
        if(hotbarBlockTypes[i] == itemIndex && amount != 0)
        {
            if(hotbarBlockAmount[i] > amount)
            {
                hotbarBlockAmount[i] -= amount;
                amount = 0;
            }
            if(hotbarBlockAmount[i] == amount)
            {
                hotbarBlockAmount[i] = 0;
                hotbarBlockTypes[i] = -1;
				hotbarBlockHp[i] = 0;
                amount = 0;
            }
            if(hotbarBlockAmount[i] < amount)
            {
                amount -= hotbarBlockAmount[i];
                hotbarBlockAmount[i] = 0;
                hotbarBlockTypes[i] = -1;
				hotbarBlockHp[i] = 0;
            }
        }
    }
}
void removeItemFromInventory(int itemIndex, int amount)
{
    for (int i = 0; i < 27; i++)
    {
        if(inventoryBlockTypes[i] == itemIndex && amount != 0)
        {
            if(inventoryBlockAmount[i] > amount)
            {
                inventoryBlockAmount[i] -= amount;
                amount = 0;
            }
            if(inventoryBlockAmount[i] == amount)
            {
                inventoryBlockAmount[i] = 0;
                inventoryBlockTypes[i] = -1;
				inventoryBlockHp[i] = 0;
                amount = 0;
            }
            if(inventoryBlockAmount[i] < amount)
            {
                amount -= inventoryBlockAmount[i];
                inventoryBlockAmount[i] = 0;
                inventoryBlockTypes[i] = -1;
				inventoryBlockHp[i] = 0;
            }
        }
    }
}
void removeItemFromInventoryHotbar(int itemIndex, int amount)
{
    for (int i = 0; i < 9; i++)
    {
        if(hotbarBlockTypes[i] == itemIndex && amount != 0)
        {
            if(hotbarBlockAmount[i] > amount)
            {
                hotbarBlockAmount[i] -= amount;
                amount = 0;
            }
            if(hotbarBlockAmount[i] == amount)
            {
                hotbarBlockAmount[i] = 0;
                hotbarBlockTypes[i] = -1;
				hotbarBlockHp[i] = 0;
                amount = 0;
            }
            if(hotbarBlockAmount[i] < amount)
            {
                amount -= hotbarBlockAmount[i];
                hotbarBlockAmount[i] = 0;
                hotbarBlockTypes[i] = -1;
				hotbarBlockHp[i] = 0;
            }
        }
    }

    if(amount != 0)
    {
        for (int i = 0; i < 27; i++)
        {
            if(inventoryBlockTypes[i] == itemIndex && amount != 0)
            {
                if(inventoryBlockAmount[i] > amount)
                {
                    inventoryBlockAmount[i] -= amount;
                    amount = 0;
                }
                if(inventoryBlockAmount[i] == amount)
                {
                    inventoryBlockAmount[i] = 0;
                    inventoryBlockTypes[i] = -1;
					inventoryBlockHp[i] = 0;
                    amount = 0;
                }
                if(inventoryBlockAmount[i] < amount)
                {
                    amount -= inventoryBlockAmount[i];
                    inventoryBlockAmount[i] = 0;
                    inventoryBlockTypes[i] = -1;
					inventoryBlockHp[i] = 0;
                }
            }
        }
    }
}
void updateItemSlected()
{
    if(hotbarBlockTypes[currentSlot] != -1)
    blockSelected = hotbarBlockTypes[currentSlot];
    else
    blockSelected = -1;
}
