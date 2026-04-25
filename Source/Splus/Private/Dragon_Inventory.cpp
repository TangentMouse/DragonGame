// Fill out your copyright notice in the Description page of Project Settings.


#include "Dragon_Inventory.h"

// Sets default values for this component's properties
UDragon_Inventory::UDragon_Inventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
    DReSetArray();
	// ...
}
//初始化背包
void UDragon_Inventory::DReSetArray()
{
    DData.Empty();
    for (int32 i = 0; i < DSlot; i++)
    {
        FDragonData NewElement;
        NewElement.DragonBaseNum = 0;
        NewElement.Level = 0;
        DData.Add(NewElement);
    }
}
bool UDragon_Inventory::AddDragonToArray(FDragonData InputDragon)
{
    
    for (int32 i = 0;i < DSlot;i++)
    {
        if (DData[i].DragonBaseNum == 0)
        {
            InputDragon.NowHP= (InputDragon.HP* 1.6 + InputDragon.HP * 0.4 * InputDragon.HPIndex) * pow(1.03, InputDragon.Level) + InputDragon.HP * InputDragon.Level / 20;
            DData[i] = InputDragon;
            
            
            return true;
        }
    }
    return false;
}
bool UDragon_Inventory::RemoveDragonFromArray(int32 SlotNum)
{
    if (DSlot < SlotNum)
    {
        return false;
    }
    FDragonData tihuan;
    tihuan.DragonBaseNum = 0;
    DData[SlotNum] = tihuan;
    return true;

}
TArray<int32> UDragon_Inventory::GetDragonFromNum(int32 DragonNum)
{
    TArray<int32>AlltrueDragonNum;
    for (int32 i=0;i < DSlot;i++)
    {
        if (DData[i].DragonBaseNum == DragonNum)
        {
            AlltrueDragonNum.Add(i);
        }
    }
    return AlltrueDragonNum;

}
void UDragon_Inventory::EXchangeDragon(int32 num1, int32 num2)
{
    
    FDragonData Exchange = DData[num1];
    DData[num1] = DData[num2];
    DData[num2] = Exchange;

}




//计算数值(真计算器）

TArray<float> UDragon_Inventory::ChushihuaData(FDragonData Inputdata)
{
    int32 level = Inputdata.Level;
    float AT = Inputdata.AT;
    float DF = Inputdata.De;
    float HP = Inputdata.HP;
    float SP = Inputdata.SP;
    float Aindex = Inputdata.AttackIndex;
    float Dindex = Inputdata.DefenseIndex;
    float Hindex = Inputdata.HPIndex;
    float Sindex = Inputdata.SpeedIndex;
    //计算攻击力
    float A = (AT * 0.8 + AT * 0.2 * Aindex) * pow(1.03, level);
    //防御力
    float D = (DF * 0.8 + DF * 0.2 * Dindex) * pow(1.03, level);
    //计算速度
    float S = (SP * 0.8 + SP * 0.2 * Sindex) * pow(1.02, level);
    //血量
    float H = (HP * 1.6 + HP * 0.4 * Hindex) * pow(1.03, level) + HP * level / 20;
    TArray<float>returndata;
    returndata = { H,A,D,S };
    return returndata;

}

//升级
void UDragon_Inventory::CalcuUpEXPandLEVEL(int32 Num, float EXP)
{
    //孩子们我除了100万个崩溃
    if (DData.Num()<Num-1)
    {
        return;
    }
    if (DData[Num].Level < 100)
    {
        float lastEXP= DData[Num].UpExpNeed+EXP;
        float NEEDEXP = (100 + 10*DData[Num].Level) * pow(1.02, DData[Num].Level);
        for (;lastEXP > NEEDEXP;)
        {
            lastEXP = lastEXP - NEEDEXP;
            DData[Num].Level++;
            if (DData[Num].Level == 100)
            {
                DData[Num].NowHP = (DData[Num].HP * 1.6 + DData[Num].HP * 0.4 * DData[Num].HPIndex) * pow(1.03, DData[Num].Level) + DData[Num].HP * DData[Num].Level / 20;;
                return;
            }
            DData[Num].NowHP = (DData[Num].HP * 1.6 + DData[Num].HP * 0.4 * DData[Num].HPIndex) * pow(1.03, DData[Num].Level) + DData[Num].HP * DData[Num].Level / 20;
            NEEDEXP = (100 + 10 * DData[Num].Level) * pow(1.02, DData[Num].Level);
            
        }
      
        DData[Num].UpExpNeed = lastEXP;
    }
}
//只计算升级需求(给ui用的)

float UDragon_Inventory::CalcuLevelONLY(int32 LEVEL)
{
    return (100 + 10 * LEVEL) * pow(1.02, LEVEL);
}
//计算真伤
void UDragon_Inventory::RealDamage(int32 Damage, int32 INum)
{
    DData[INum].NowHP = DData[INum].NowHP - Damage;
}



//技能替换
void UDragon_Inventory::MoveChange(int32 DNUM, int32 M1, int32 M2)
{
    if (M1 > DData[DNUM].MOVE.Num())
    {
        return;
    }
    if (M2 > DData[DNUM].MOVE.Num())
    {
        return;
    }

    int32 changeMove1 = DData[DNUM].MOVE[M1];
    DData[DNUM].MOVE[M1] = DData[DNUM].MOVE[M2];
    DData[DNUM].MOVE[M2] = changeMove1;


}


/////分割线

//计算元素获取
void UDragon_Inventory::AddElementPotential(int32 Enum, int32 Dnum)
{
    if (Enum == 0)
    {
        return;
    }
    if (DData[Dnum].ElementPotential.Num() > 99)
    {
        return;
    }
    DData[Dnum].ElementPotential.Add(Enum);
}

int32 UDragon_Inventory::ELementUP(int32 Dnum)
{
    // 定义5个分组的计数器和值频率映射
    struct GroupData
    {
        int32 TotalCount = 0;                  // 组内总数量
        TMap<int32, int32> ValueFrequency;     // 组内各值的出现次数
    };

    TArray<GroupData> Groups;
    Groups.SetNum(5);  // 5个分组：1-4,5-8,9-12,13-16,17-20

    // 遍历数组元素进行分组统计
    for (int32 i = 0; i < DData[Dnum].ElementPotential.Num(); i++)
    {
        int32 Value = DData[Dnum].ElementPotential[i];

        // 跳过不在1-20范围内的值
        if (Value < 1 || Value > 20)
            continue;

        // 确定值所属的分组索引（0-4分别对应5个组）
        int32 GroupIndex;
        if (Value <= 4)
            GroupIndex = 0;
        else if (Value <= 8)
            GroupIndex = 1;
        else if (Value <= 12)
            GroupIndex = 2;
        else if (Value <= 16)
            GroupIndex = 3;
        else  // 17-20
            GroupIndex = 4;

        // 更新分组统计数据
        GroupData& CurrentGroup = Groups[GroupIndex];
        CurrentGroup.TotalCount++;
        CurrentGroup.ValueFrequency.FindOrAdd(Value)++;  // 累加值的出现次数
    }

    // 找到总数量最多的分组（如果有多个组数量相同，取第一个）
    int32 MaxTotal = -1;
    int32 MaxGroupIndex = -1;
    for (int32 i = 0; i < Groups.Num(); i++)
    {
        if (Groups[i].TotalCount > MaxTotal)
        {
            MaxTotal = Groups[i].TotalCount;
            MaxGroupIndex = i;
        }
    }

    // 如果所有组都没有数据（全部值超出范围），返回0或根据需求调整
    if (MaxGroupIndex == -1)
        return 0;

    // 在最多数量的分组中，找到出现次数最多的具体值
    GroupData& TargetGroup = Groups[MaxGroupIndex];
    int32 MostFrequentValue = 0;
    int32 MaxFrequency = -1;
    for (const auto& Kvp : TargetGroup.ValueFrequency)
    {
        if (Kvp.Value > MaxFrequency)
        {
            MaxFrequency = Kvp.Value;
            MostFrequentValue = Kvp.Key;
        }
    }

    return MostFrequentValue;
}