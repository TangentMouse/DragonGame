// Fill out your copyright notice in the Description page of Project Settings.


#include "d_Element.h"
// Sets default values for this component's properties
Ud_Element::Ud_Element()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	LoadEvolutionDatabase();
	BaseBuff.Init(FVector(0, 0, 0),HowMuchBuff);

	
}




//非蓝图函数
// 
//获取GameSubsystem并且载入(MOVE存储）
void Ud_Element::getSubsystem()
{
	// 1. 获取当前世界（从组件或其所属 Actor 中）
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("CantGETWORLD"));
		return;
	}
	// 2. 通过世界获取 GameInstance
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("CantGEtGameInstance"));
		return;
	}

	// 3. 通过 GameInstance 获取已注册的子系统，并赋值给指针
	GameSubsystem = GameInstance->GetSubsystem<UGameSubsystem>();
}
//加载蓝图数据表
UDataTable* Ud_Element::LoadEvolutionDatabase()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(
		TEXT("/Game/C_BP/BP_DragonDatabase/Database/StudyMOVEANDEnvo_database") 
	);
	if (DataTableFinder.Succeeded())
	{
		
		return DataTableFinder.Object;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to load EvolutionDatabase!"));
	return nullptr;
}






//蓝图函数
//计算录入能力数值（通过个体）
TArray<float> Ud_Element::ChushihuaData(FDragonData Inputdata)
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
	SingleDragonD.Attack = (AT * 0.8 + AT * 0.2 * Aindex) * FMath::Pow(1.03, level);
	//防御力
	SingleDragonD.Defense = (DF * 0.8 + DF * 0.2 * Dindex) * FMath::Pow(1.03, level);
	//计算速度
	SingleDragonD.Speed = (SP * 0.8 + SP * 0.2 * Sindex) * FMath::Pow(1.02, level);
	//血量
	SingleDragonD.MaxHP = (HP * 1.6 + HP * 0.4 *Hindex) * FMath::Pow(1.03, level) + HP * level / 20;
	TArray<float> returnd = TArray<float>{
		// 使用FMath::Pow并将结果转换为float
		(float)((AT * 0.8 + AT * 0.2 * Aindex) * FMath::Pow(1.03, level)),
		(float)((DF * 0.8 + DF * 0.2 * Dindex) * FMath::Pow(1.03, level)),
		(float)((SP * 0.8 + SP * 0.2 * Sindex) * FMath::Pow(1.02, level)),
		(float)((HP * 1.6 + HP * 0.4 * Hindex) * FMath::Pow(1.03, level) + HP * level / 20.0)
	};
	return returnd;
	
}
//技能克制倍率关系
int32 Ud_Element::MoveAppendElement(int32 MoveElement, int32 actorElement)
{
	// 无效元素处理
	if (actorElement == 0)
	{
		return 0;
	}

	// 使用else if避免条件重叠，确保只会进入一个分支
	if (MoveElement <= 4)
	{
		if (actorElement <= 4)      return -1;
		else if (actorElement <= 8) return 1;
		else if (actorElement <= 12) return 0;
		else if (actorElement <= 16) return 1;
		else if (actorElement <= 20) return 0;
	}
	else if (MoveElement <= 8)
	{
		if (actorElement <= 4)      return 0;
		else if (actorElement <= 8) return -1;
		else if (actorElement <= 12) return 1;
		else if (actorElement <= 16) return -1;
		else if (actorElement <= 20) return 1;
	}
	else if (MoveElement <= 12)
	{
		if (actorElement <= 4)      return 0;
		else if (actorElement <= 8) return 0;
		else if (actorElement <= 12) return -1;
		else if (actorElement <= 16) return 1;
		else if (actorElement <= 20) return 0;
	}
	else if (MoveElement <= 16)
	{
		if (actorElement <= 4)      return 1;
		else if (actorElement <= 8) return 1;
		else if (actorElement <= 12) return -1;
		else if (actorElement <= 16) return -1;
		else if (actorElement <= 20) return 0;
	}
	else if (MoveElement <= 20)
	{
		if (actorElement <= 4)      return 1;
		else if (actorElement <= 8) return -1;
		else if (actorElement <= 12) return 0;
		else if (actorElement <= 16) return 0;
		else if (actorElement <= 20) return -1;
	}

	// 超出范围的默认值
	return 0;
}









//技能计算对自己造成伤害(逆天
int32 Ud_Element::toRealDamage(
	int32 userlevel,
	int32 whichMOVE,
	float attack, 
	float defense,
	TArray<int32> SelfElement,
	TArray<int32> jiachengElement,
	TArray<FVector> UserBuff)
{

	getSubsystem();

	if (whichMOVE > GameSubsystem->MTb.Num() - 1)
	{
		return 0;
	}
	//技能克制伤害倍率（不好意思我用拼音）
	float beilv = 1;
	for (int32 i = 0;i < SelfElement.Num() ;)
	{
		beilv=beilv + 0.25 * MoveAppendElement(GameSubsystem->MTb[whichMOVE].MElement, SelfElement[i]);
		i++;
	}
	if (beilv <= 0)
	{
		beilv = 0.1;
	}

	//本系一致加成
	float benxi = 1;
	
	// 使用else if避免条件重叠，确保只会进入一个分支(我去这坨代码好像垃圾)
	if (GameSubsystem->MTb[whichMOVE].MElement <= 4)
	{
		for (int32 n = 0;n < jiachengElement.Num();)
		{
			if (jiachengElement[n] > 0 && jiachengElement[n] <= 4)
			{
				n++;
				benxi++;
			}
			else
			{
				break;
			}
		}


	}
	else if (GameSubsystem->MTb[whichMOVE].MElement <= 8)
	{
		for (int32 n = 0;n < jiachengElement.Num();)
		{
			if (jiachengElement[n] > 4 && jiachengElement[n] <= 8)
			{
				n++;
				benxi++;
			}
			else
			{
				break;
			}
		}


	}
	else if (GameSubsystem->MTb[whichMOVE].MElement <= 12)
	{
		for (int32 n = 0;n < jiachengElement.Num();)
		{
			if (jiachengElement[n] > 8 && jiachengElement[n] <= 12)
			{
				n++;
				benxi++;
			}
			else
			{
				break;
			}
		}



	}
	else if (GameSubsystem->MTb[whichMOVE].MElement <= 16)
	{
		
		for (int32 n = 0;n < jiachengElement.Num();)
		{
			if (jiachengElement[n] > 12 && jiachengElement[n] <= 16)
			{
				n++;
				benxi++;
			}
			else
			{
				break;
			}
		}


	}
	else if (GameSubsystem->MTb[whichMOVE].MElement <= 20)
	{
		for (int32 n = 0;n < jiachengElement.Num();)
		{
			if (jiachengElement[n] > 16 && jiachengElement[n] <= 20)
			{
				n++;
				benxi++;
			}
			else
			{
				break;
			}
		}



	}



	





	//这是临时的计算公式，后期会根据游戏调整
	
	int32 C1 = FMath::CeilToInt((1 + userlevel / 25) * (1 + (attack*(1+ GBF(1, UserBuff))) / (defense * (1 + GBF(2, BaseBuff))) * GameSubsystem->MTb[whichMOVE].MoveAT * beilv * (1 + benxi * 0.25)));
		
	return C1;

	
}



















//只计算升级需求

float Ud_Element::CalLONLY(int32 LEVEL)
{
	return (100 + 10 * LEVEL) * pow(1.02, LEVEL);
}
//载入等级学习技能（怪和主角都可以用）
TArray<int32> Ud_Element::IntStudyMove(int32 Level, int32 DragonFromDatatable)
{
	//加载一下数据库先
	LoadEvolutionDatabase();
	UDataTable* EvolutionDB = LoadEvolutionDatabase();
	// 将int32转换为FName（行名）
	if (!EvolutionDB) {
		UE_LOG(LogTemp, Error, TEXT("Failed to load Evolution Database!"));
		return TArray<int32>();
	}
	FName RowName = FName(*FString::FromInt(DragonFromDatatable));
	// 使用转换后的FName查找行数据
	const FEvolutionANDmove* RowData = EvolutionDB->FindRow<FEvolutionANDmove>(RowName, TEXT("Get SUccess"));
	if (!RowData)
	{
		UE_LOG(LogTemp, Error, TEXT("cant Find：%d"), DragonFromDatatable);
		return { 0 };
	}
	TArray<FStudyMove> SM= RowData->MOVEtable;
	TArray<int32> Studyedmovetable;
	for (int i = 0;i < SM.Num();i++)
	{
		if (Level > SM[i].Level-1)
		{
			Studyedmovetable.Add(SM[i].LearnMove);
		}
	}
	return{ Studyedmovetable };
}
TArray<int32> Ud_Element::ReSetMove(TArray<int32> intMOVE,int32 Level,int32 DragonFromDatatable)
{
	LoadEvolutionDatabase();
	TArray<int32> CanlearnMoves = IntStudyMove(Level, DragonFromDatatable);
	TArray<int32>AddmoveArray;
	for (int32 i = 0;i < CanlearnMoves.Num();i++)
	{
		bool bFound = false;
		for (int32 i1 = 0; i1 < intMOVE.Num(); i1++)
		{
			if (intMOVE[i1] == CanlearnMoves[i])
			{
				bFound = true;
				break; 
			}
		}
		if (!bFound)
		{
			AddmoveArray.Add(CanlearnMoves[i]);
		}
	}
	for (int32 i2 = 0;i2 < AddmoveArray.Num();i2++)
	{
		intMOVE.Add(AddmoveArray[i2]);
	}
	return intMOVE;
}






//喜欢我Buff函数吗
float Ud_Element::GBF(int32 BuffIndex,TArray<FVector> BuffArray)
{
	if (BuffIndex> BuffArray.Num())
	{
		return 0.0f;
	}
	return 0.1*BuffArray[BuffIndex].X;
}

void  Ud_Element::ADDBuff(int32 WhichB,FVector LevelAndDelay)
{
	if (WhichB > BaseBuff.Num())
	{
		return;
	}
	if (BaseBuff[WhichB].X > LevelAndDelay.X)
	{
		BaseBuff[WhichB].Y += LevelAndDelay.Y;
	}
	else
	{
		BaseBuff[WhichB].X = LevelAndDelay.X;
		BaseBuff[WhichB].Y += LevelAndDelay.Y;
	}
}

void  Ud_Element::RemoveBuff()
{
	for (int32 i = 0;i < BaseBuff.Num();i++)
	{
		if (BaseBuff[i].Y ==0)
		{
			BaseBuff[i].X = 0;
			continue;
		}
		BaseBuff[i].Y -= 1;
	}
}
