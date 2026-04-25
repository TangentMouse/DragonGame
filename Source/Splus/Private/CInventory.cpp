// Fill out your copyright notice in the Description page of Project Settings.


#include "CInventory.h"

// Sets default values for this component's properties
UCInventory::UCInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


//物品系统;不要碰 这个东西是初始化背包
void UCInventory::ReSetArray()
{
    ItemData.Empty();
    for (int32 i = 0; i < CSlot; i++)
    {
        FBData NewElement;
        NewElement.Num = 0;
        ItemData.Add(NewElement);
    }
}
//这是添加的逻辑
FAddreturn UCInventory::AddStructToArray(FBData Playerbackpack)
{
    if (ItemData.IsEmpty())
    {
        FAddreturn Result;
        Result.isfull = false;
        Result.LessFigure = 0;
        return Result;
    }
    if (Playerbackpack.Canduidie == true)
    {
        for (int32 i = 0; i < CSlot; i++)
        {
            if (ItemData[i].Num == 0)
            {
                if (Playerbackpack.DuidieNum > Maxduidie)
                {
                    FBData ForthisSlot;
                    ForthisSlot = Playerbackpack;
                    ForthisSlot.DuidieNum = Maxduidie;
                    ItemData[i] = ForthisSlot;
                    Playerbackpack.DuidieNum = Playerbackpack.DuidieNum - Maxduidie;
                }
                else
                {
                    ItemData[i] = Playerbackpack;
                    FAddreturn Result;
                    Result.isfull = false;
                    Result.LessFigure = 0;
                    return Result;
                }
            }
            else
            {
                if (Playerbackpack.Num == ItemData[i].Num)
                {
                    int32 DataNum = Maxduidie - ItemData[i].DuidieNum;
                    if (Playerbackpack.DuidieNum > DataNum)
                    {
                        Playerbackpack.DuidieNum = Playerbackpack.DuidieNum - DataNum;
                        FBData NewItemData;
                        NewItemData = ItemData[i];
                        NewItemData.DuidieNum = Maxduidie;
                        ItemData[i] = NewItemData;
                    }
                    else
                    {
                        ItemData[i].DuidieNum = ItemData[i].DuidieNum + Playerbackpack.DuidieNum;
                        FAddreturn Result;
                        Result.isfull = false;
                        Result.LessFigure = 0;
                        return Result;
                    }
                }
            }
            if (i == CSlot)
            {
                if (Playerbackpack.DuidieNum == 0)
                {
                    FAddreturn Result;
                    Result.isfull = false;
                    Result.LessFigure = 0;
                    return Result;
                }
                else
                {
                    FAddreturn Result;
                    Result.isfull = true;
                    Result.LessFigure = Playerbackpack.DuidieNum;
                    return Result;
                }

            }
        }
    }
    else
    {
        for (int32 i = 0; i < CSlot; i++)
        {
            if (ItemData[i].Num == 0)
            {
                ItemData[i] = Playerbackpack;
                FAddreturn Result;
                Result.isfull = false;
                Result.LessFigure = 0;
                return Result;
            }
        }

    }
    FAddreturn Result;
    Result.isfull = true;
    Result.LessFigure = Playerbackpack.DuidieNum;
    return Result;
}
//这是消耗的逻辑
//1.数字索引移除
FBData UCInventory::RemoveSingleSlot(int32 SlotNum)
{
    FBData EmptySLot;
    FBData ReturnData = ItemData[SlotNum];
    EmptySLot.Num = 0;
    ItemData[SlotNum] = EmptySLot;
    return ReturnData;
}
//2.检索是否有（先行判断，移除之前一定要做这个，否则移除那一步有bug）
bool UCInventory::isAllEnough(FDeleteItemArray NeedItem)
{
    // 检查需求列表是否为空
    if (NeedItem.DeleteItemData.Num() == 0)
    {
        return true;
    }

    // 遍历每个需要检查的物品
    for (int32 i = 0; i < NeedItem.DeleteItemData.Num(); i++)
    {
        FDeleteItem neededItem = NeedItem.DeleteItemData[i];
        int32 totalAvailable = 0;

        // 累加所有相同 Num 的格子中的物品数量
        for (int32 n = 0; n < CSlot; n++)
        {
            if (ItemData[n].Num == neededItem.NeedITEM.Num)
            {
                totalAvailable = totalAvailable + ItemData[n].DuidieNum;
            }
        }

        // 检查总数量是否足够
        if (totalAvailable < neededItem.ItemAmount)
        {
            return false; // 只要有一个物品不足，立即返回 false
        }
    }

    // 所有物品都满足需求
    return true;
}
//3.用索引去removeITem
void UCInventory::deleteItem(FDeleteItemArray DeleteItem)
{
    if (DeleteItem.DeleteItemData.Num() == 0)
    {
        return;
    }

    // 遍历每个需要删除的物品
    for (int32 i = 0; i < DeleteItem.DeleteItemData.Num(); i++)
    {
        FDeleteItem deleteItem = DeleteItem.DeleteItemData[i];
        int32 remainingAmount = deleteItem.ItemAmount;

        // 遍历所有槽位
        int32 currentSlot = 0;
        while (currentSlot < ItemData.Num() && remainingAmount > 0)
        {
            FBData slot = ItemData[currentSlot];

            // 找到匹配的物品
            if (slot.Num == deleteItem.NeedITEM.Num)
            {
                if (slot.DuidieNum >= remainingAmount)
                {
                    // 槽位中的数量足够，直接扣除
                    slot.DuidieNum = slot.DuidieNum - remainingAmount;
                    ItemData[currentSlot].DuidieNum = slot.DuidieNum;
                    remainingAmount = 0;

                    // 如果槽位为空，移除该槽位
                    if (slot.DuidieNum == 0)
                    {
                        RemoveSingleSlot(currentSlot);
                        // 注意：删除后 currentSlot 不需要增加，因为后续元素会前移
                        continue;
                    }
                }
                else
                {
                    //槽位中的数量不足，清空该槽位
                    remainingAmount = remainingAmount - slot.DuidieNum;
                    RemoveSingleSlot(currentSlot);
                    //删除后 currentSlot 不需要增加
                    continue;
                }
            }
// 只有在没有删除元素的情况下才增加 currentSlot
            currentSlot++;
        }
    }
}
//该函数为物品栏切换的时候和相同信息物品交换的逻辑(对不起一个有FBD类型和Int类型的结构体我不想再定义一个，就凑合着用Amount专用的吧）
FBData UCInventory::CalculateChangeItem(FDeleteItem IntItemDatas)
{
    
    int32 AllAmount;
    if (IntItemDatas.ItemAmount < ItemData.Num())
    {
        AllAmount = IntItemDatas.NeedITEM.DuidieNum + ItemData[IntItemDatas.ItemAmount].DuidieNum;
        if (AllAmount > Maxduidie)
        {
            ItemData[IntItemDatas.ItemAmount].DuidieNum = Maxduidie;
            FBData ReturnItem;
            ReturnItem = IntItemDatas.NeedITEM;
            ReturnItem.DuidieNum = AllAmount - Maxduidie;
            return ReturnItem;
        }
        else
        {
            FBData ReturnItem;
            ItemData[IntItemDatas.ItemAmount].DuidieNum = AllAmount;
            ReturnItem = IntItemDatas.NeedITEM;
            ReturnItem.DuidieNum = AllAmount - Maxduidie;
            return ReturnItem;
        }
    }
    else
    {
        FBData AError;
        return AError;
    }
   
}

