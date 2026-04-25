// Fill out your copyright notice in the Description page of Project Settings.

#include "TLoadWorld.h"
#include "TVoxel.h"
#include "Kismet/GameplayStatics.h"  // 可选，用于额外的世界相关函数
int32 TickNum;
ATLoadWorld::ATLoadWorld()
{
    PrimaryActorTick.bCanEverTick = true;
    
    TickNum = 0; // 初始化计数器
}


// Called when the game starts or when spawned
void ATLoadWorld::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ATLoadWorld::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); // 必须调用父类Tick
    tickFUN();
   
}


void ATLoadWorld::tickFUN()
{
    int32 GenerateRadius = 5; // 默认±4，9×9区块

    // 每次Tick生成10个区块，适配可配置范围的区块生成
    for (int32 f = 0; f < 25; f++)
    {
        UWorld* World = GetWorld();
        if (!World) return;

        APlayerController* LocalPC = World->GetFirstPlayerController();
        if (!LocalPC) return;

        AActor* Pawn = LocalPC->GetPawn();
        if (!Pawn) return;

        // 计算玩家所在区块索引（基于800×800区块尺寸）
        FVector PlayerPos = Pawn->GetActorLocation();
        int32 PlayerBlockX = FMath::FloorToInt(PlayerPos.X / BlockSizeX);
        int32 PlayerBlockY = FMath::FloorToInt(PlayerPos.Y / BlockSizeY);

        // 计算总范围（如GenerateRadius=4则为9，即2×4+1）
        const int32 TotalRange = 2 * GenerateRadius + 1;
        // 区块偏移：围绕玩家±GenerateRadius
        int32 OffsetX = (TickNum % TotalRange) - GenerateRadius;
        int32 OffsetY = (TickNum / TotalRange) - GenerateRadius;
        int32 TargetBlockX = PlayerBlockX + OffsetX;
        int32 TargetBlockY = PlayerBlockY + OffsetY;

        // 生成区块唯一键，避免重复创建
        FString BlockKey = FString::Printf(TEXT("%d_%d"), TargetBlockX, TargetBlockY);
        if (!TVtmap.Contains(BlockKey))
        {
            SpawnVoxel(FVector(TargetBlockX, TargetBlockY, 0));
        }

        // 计数器：循环范围为总区块数（TotalRange×TotalRange）
        TickNum = (TickNum + 1) % (TotalRange * TotalRange);
    }

    // 范围外区块剔除逻辑（每帧只处理10个）
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* LocalPC = World->GetFirstPlayerController();
    if (!LocalPC) return;

    AActor* Pawn = LocalPC->GetPawn();
    if (!Pawn) return;

    FVector PlayerPos = Pawn->GetActorLocation();
    int32 PlayerBlockX = FMath::FloorToInt(PlayerPos.X / BlockSizeX);
    int32 PlayerBlockY = FMath::FloorToInt(PlayerPos.Y / BlockSizeY);

    const int32 KeepRange = 50;
    int32 MinBlockX = PlayerBlockX - KeepRange;
    int32 MaxBlockX = PlayerBlockX + KeepRange;
    int32 MinBlockY = PlayerBlockY - KeepRange;
    int32 MaxBlockY = PlayerBlockY + KeepRange;

    // 静态变量：保存待删除的键列表和当前处理索引（跨帧延续状态）
    static TArray<FString> AllKeysToRemove;
    static int32 CurrentCullIndex = 0;

    // 若待删除列表为空，则重新收集所有需要删除的键
    if (AllKeysToRemove.Num() == 0)
    {
        AllKeysToRemove.Empty(); // 清空旧数据
        for (const auto& Kvp : TVtmap)
        {
            const FString& BlockKey = Kvp.Key;
            TArray<FString> Coords;
            if (BlockKey.ParseIntoArray(Coords, TEXT("_"), true) != 2)
            {
                AllKeysToRemove.Add(BlockKey);
                continue;
            }
            int32 BlockX = FCString::Atoi(*Coords[0]);
            int32 BlockY = FCString::Atoi(*Coords[1]);
            if (BlockX < MinBlockX || BlockX > MaxBlockX || BlockY < MinBlockY || BlockY > MaxBlockY)
            {
                AllKeysToRemove.Add(BlockKey);
            }
        }
        CurrentCullIndex = 0; // 重置索引
    }

    // 每帧只处理个区块（或剩余全部）
    const int32 CullPerFrame = 25;
    int32 Processed = 0;
    while (CurrentCullIndex < AllKeysToRemove.Num() && Processed < CullPerFrame)
    {
        const FString& Key = AllKeysToRemove[CurrentCullIndex];
        ATVoxel** VoxelPtr = TVtmap.Find(Key);
        if (VoxelPtr && *VoxelPtr)
        {
            ATVoxel* VoxelActor = *VoxelPtr;
            TVtmap.Remove(Key);
            if (VoxelActor)
            {
                VoxelActor->Destroy();
            }
        }
        CurrentCullIndex++;
        Processed++;
    }

    // 若所有待删除区块处理完毕，清空列表
    if (CurrentCullIndex >= AllKeysToRemove.Num())
    {
        AllKeysToRemove.Empty();
        CurrentCullIndex = 0;
    }
}

void ATLoadWorld::SpawnVoxel(FVector BlockIndex)
{
    UWorld* World = GetWorld();
    if (!World) return;

    UClass* VoxelClass = ATVoxel::StaticClass();
    if (!VoxelClass)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnVoxel: ATVoxel类未找到！"));
        return;
    }

    // 区块生成位置：X/Y×800，Z=0（Z轴起点不变）
    FVector SpawnLoc = FVector(
        BlockIndex.X * BlockSizeX,
        BlockIndex.Y * BlockSizeY,
        0.0f
    );
    FRotator SpawnRot = FRotator::ZeroRotator;
    FTransform SpawnTrans(SpawnRot, SpawnLoc);

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.bNoFail = true;
    ATVoxel* SpawnedVoxel = World->SpawnActor<ATVoxel>(VoxelClass, SpawnTrans, SpawnParams);

    if (SpawnedVoxel)
    {
        SDFSpawn(BlockIndex); // 生成9×9×129维度的SDF数据
        SpawnedVoxel->GenerateTVoxelmin(TV); // 需确保该函数适配129层Z轴
        FString BlockKey = FString::Printf(TEXT("%d_%d"), (int32)BlockIndex.X, (int32)BlockIndex.Y);
        TVtmap.Add(BlockKey, SpawnedVoxel);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnVoxel: 区块生成失败！索引：%d_%d"), (int32)BlockIndex.X, (int32)BlockIndex.Y);
    }
}


void ATLoadWorld::SDFSpawn(FVector BlockIndex)
{
    // 1. 基础参数：9×9×129=10449个顶点（原5265→翻倍）
    const int32 TotalVertex = VertexDimX * VertexDimY * VertexDimZ;
    TV.SetNum(TotalVertex); // 初始化TV数组容量
    const FString BlockKey = FString::Printf(TEXT("%d_%d"), (int32)BlockIndex.X, (int32)BlockIndex.Y);

    // 2. 区块世界坐标：Z轴范围0~12800（原6400→翻倍）
    const float WorldX0 = BlockIndex.X * BlockSizeX;
    const float WorldY0 = BlockIndex.Y * BlockSizeY;
    const float WorldZ0 = 0.0f;                       // Z轴起点：0
    const float WorldZMax = BlockSizeZ;               // Z轴终点：12800

    // 3. 地形高度参数：适配12800高度，确保地形不矮
    const int32 Octaves = 2;
    const float Frequency = 0.0002f;         // 噪声频率不变（地形密度相同）
    const float Amplitude = 1000.0f;         // 振幅增大（地形起伏更大，最高更高）
    const float Persistence = 0.3f;          // 持续性不变
    const float MinTerrainHeight = 3800.0f;  // 最低地形高度：2000（原3200→降低，留更多空间）
    const float BaseHeight = 5000.0f;        // 基准高度：4000（原2500→提升，整体抬高）

    // 4. 分层与矿物参数：适配12800高度
    const int32 OuterLayerVoxels = 2;                  // 表层2体素（200单位，不变）
    const float OuterLayerThick = OuterLayerVoxels * VoxelSize;
    const float OreSpawnProb = 0.25f;                  // 矿物概率不变
    const float MinOreRadius = 2 * VoxelSize;          // 矿物最小半径200（不变）
    const float MaxOreRadius = 5 * VoxelSize;          // 最大半径500（原400→增大，适配高区块）
    TArray<FVector> OreCenters;
    TArray<float> OreRadii;

    // 5. 预生成矿物球：适配12800 Z轴高度
    FRandomStream Rand;
    Rand.Initialize(FMath::FloorToInt(WorldX0) + FMath::FloorToInt(WorldY0) * 10000);
    for (int32 i = 0; i < 3; i++) // 矿物数量从2→3（适配高区块，增加体验）
    {
        if (Rand.FRand() > OreSpawnProb) continue;

        // 矿物采样位置：适配8体素维度（0~7）
        const int32 SampleVoxelX = Rand.RandRange(0, VertexDimX - 2);
        const int32 SampleVoxelY = Rand.RandRange(0, VertexDimY - 2);
        const float SampleWorldX = WorldX0 + (SampleVoxelX + 0.5f) * VoxelSize;
        const float SampleWorldY = WorldY0 + (SampleVoxelY + 0.5f) * VoxelSize;

        // 计算采样点地形高度
        float SampleNoise = 0.0f;
        float CurrFreq = Frequency;
        float CurrAmp = Amplitude;
        for (int32 o = 0; o < Octaves; o++)
        {
            SampleNoise += PerlinNoise2D(SampleWorldX * CurrFreq, SampleWorldY * CurrFreq) * CurrAmp;
            CurrFreq *= 2.0f;
            CurrAmp *= Persistence;
        }
        float SurfaceHeight = BaseHeight + SampleNoise;
        // 限制地形高度：2000~12000（离区块顶部800单位，避免溢出）
        SurfaceHeight = FMath::Clamp(SurfaceHeight, MinTerrainHeight, WorldZMax - 800.0f);

        // 矿物中心Z坐标：表层之下，且在区块内
        const int32 RandomDepthVoxels = Rand.RandRange(1, 3); // 深入1~3体素（100~300单位）
        float OreCenterZ = SurfaceHeight
            - OuterLayerThick
            - RandomDepthVoxels * VoxelSize
            - (MaxOreRadius / 2.0f);
        // 限制矿物Z：1000~11800（避免过深或超出顶部）
        OreCenterZ = FMath::Clamp(OreCenterZ, 1000.0f, WorldZMax - MaxOreRadius - 200.0f);

        // 矿物XY偏移：区块内±15%，避免跨区块
        const float OreOffsetX = Rand.FRandRange(-BlockSizeX * 0.15f, BlockSizeX * 0.15f);
        const float OreOffsetY = Rand.FRandRange(-BlockSizeY * 0.15f, BlockSizeY * 0.15f);
        const FVector OreCenter(
            FMath::Clamp(SampleWorldX + OreOffsetX, WorldX0 + 50.0f, WorldX0 + BlockSizeX - 50.0f),
            FMath::Clamp(SampleWorldY + OreOffsetY, WorldY0 + 50.0f, WorldY0 + BlockSizeY - 50.0f),
            OreCenterZ
        );

        // 调整矿物半径：不超出表层和区块
        float OreRadius = MinOreRadius + Rand.FRand() * (MaxOreRadius - MinOreRadius);
        const float OreTopZ = OreCenter.Z + OreRadius;
        const float SurfaceBottomZ = SurfaceHeight - OuterLayerThick;
        if (OreTopZ > SurfaceBottomZ || OreTopZ > WorldZMax - 200.0f)
        {
            const float MaxValidRadius = FMath::Min(SurfaceBottomZ, WorldZMax - 200.0f) - OreCenter.Z;
            OreRadius = FMath::Max(MaxValidRadius, MinOreRadius);
        }

        OreCenters.Add(OreCenter);
        OreRadii.Add(OreRadius);
    }

    // 6. 三重循环计算SDF：X9→Y9→Z129（Z轴0~128，对应0~12800）
    for (int32 x = 0; x < VertexDimX; x++)
    {
        for (int32 y = 0; y < VertexDimY; y++)
        {
            // 计算当前(x,y)的地形高度
            const float WorldX = WorldX0 + x * VoxelSize;
            const float WorldY = WorldY0 + y * VoxelSize;
            float NoiseTotal = 0.0f;
            float CurrFreq = Frequency;
            float CurrAmp = Amplitude;
            for (int32 o = 0; o < Octaves; o++)
            {
                NoiseTotal += PerlinNoise2D(WorldX * CurrFreq, WorldY * CurrFreq) * CurrAmp;
                CurrFreq *= 2.0f;
                CurrAmp *= Persistence;
            }
            float SurfaceHeight = BaseHeight + NoiseTotal;
            SurfaceHeight = FMath::Clamp(SurfaceHeight, MinTerrainHeight, WorldZMax - 800.0f);

            // Z轴循环：0~128（129个顶点，对应12800高度）
            for (int32 z = 0; z < VertexDimZ; z++)
            {
                // 顶点索引公式：X + Y*9 + Z*9*9（9×9×129专用）
                const int32 VertexIndex = x + y * VertexDimX + z * VertexDimX * VertexDimY;
                // 索引越界校验
                if (VertexIndex < 0 || VertexIndex >= TotalVertex)
                {
                    UE_LOG(LogTemp, Error, TEXT("SDFSpawn: 索引越界！索引=%d，总数=%d，X=%d,Y=%d,Z=%d"),
                        VertexIndex, TotalVertex, x, y, z);
                    continue;
                }

                // 优先读取挖洞数据
                if (ReLoadMap.Contains(BlockKey))
                {
                    const FSSMap* BlockSDFData = ReLoadMap.Find(BlockKey);
                    if (BlockSDFData && BlockSDFData->SingleBlockMap.Contains(VertexIndex))
                    {
                        TV[VertexIndex] = BlockSDFData->SingleBlockMap[VertexIndex];
                        continue;
                    }
                }

                // 计算原始SDF值
                const float WorldZ = WorldZ0 + z * VoxelSize;
                const float Depth = SurfaceHeight - WorldZ;

                if (Depth <= 0.0f || WorldZ > WorldZMax)
                {
                    TV[VertexIndex] = -1.0f; // 空中或超出区块
                }
                else
                {
                    const int32 DepthVoxels = FMath::FloorToInt(Depth / VoxelSize);
                    if (DepthVoxels < OuterLayerVoxels)
                    {
                        TV[VertexIndex] = 1.0f; // 表层
                    }
                    else
                    {
                        TV[VertexIndex] = 2.0f; // 内部

                        // 矿物判断
                        const FVector VertexPos(WorldX, WorldY, WorldZ);
                        for (int32 OreIdx = 0; OreIdx < OreCenters.Num(); OreIdx++)
                        {
                            const FVector& OreCenter = OreCenters[OreIdx];
                            const float OreRadius = OreRadii[OreIdx];
                            // 快速排除：X/Y/Z超出半径
                            if (FMath::Abs(VertexPos.X - OreCenter.X) > OreRadius) continue;
                            if (FMath::Abs(VertexPos.Y - OreCenter.Y) > OreRadius) continue;
                            if (FMath::Abs(VertexPos.Z - OreCenter.Z) > OreRadius) continue;
                            // 距离平方判断
                            if (FVector::DistSquared(VertexPos, OreCenter) <= OreRadius * OreRadius)
                            {
                                TV[VertexIndex] = 3.0f;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}













//建造函数



//破坏的函数

TArray<float> ATLoadWorld::DigHole(FVector Center, float HalfLength)
{
    TArray<float> DugSDFValues;
    if (HalfLength <= 0.0f)
    {
        return DugSDFValues;
    }

    // 1. 基础参数
    const float InvVoxelSize = 1.0f / VoxelSize;
    const float HalfBlockX = BlockSizeX / 2.0f;
    const float HalfBlockY = BlockSizeY / 2.0f;
    const int32 EdgeThresholdX = VertexDimX - 1;  // X最后一列索引（关键边缘）
    const int32 EdgeThresholdY = VertexDimY - 1;  // Y边缘索引
    const int32 EdgeThresholdZ = VertexDimZ - 1;  // Z边缘索引

    // 2. 范围计算（修正变量顺序）
    const int32 VoxelHalfRange = FMath::CeilToInt(HalfLength * InvVoxelSize);
    const int32 MaxVoxelRange = 20;
    const int32 ClampedVoxelHalfRange = FMath::Min(VoxelHalfRange + 1, MaxVoxelRange);
    const int32 VoxelFullRange = VoxelHalfRange * 2 + 1;

    // 3. 预分配容器
    DugSDFValues.Reserve(VoxelFullRange * VoxelFullRange * VoxelFullRange);
    const int32 EstModifiedBlocks = (ClampedVoxelHalfRange * 2 + 1) * (ClampedVoxelHalfRange * 2 + 1) + 8; // 预留更多边缘区块
    TMap<FString, TArray<float>> CachedBlockSDF;
    TSet<FIntVector> ModifiedBlocks;
    ModifiedBlocks.Reserve(EstModifiedBlocks);
    CachedBlockSDF.Reserve(EstModifiedBlocks);

    // 4. 正方体范围
    const FVector CubeMin = Center - FVector(HalfLength, HalfLength, HalfLength);
    const FVector CubeMax = Center + FVector(HalfLength, HalfLength, HalfLength);

    // 5. 三重循环遍历体素
    for (int32 dx = -ClampedVoxelHalfRange; dx <= ClampedVoxelHalfRange; ++dx)
    {
        for (int32 dy = -ClampedVoxelHalfRange; dy <= ClampedVoxelHalfRange; ++dy)
        {
            for (int32 dz = -ClampedVoxelHalfRange; dz <= ClampedVoxelHalfRange; ++dz)
            {
                // 体素世界坐标
                const FVector VoxelWorldPos(
                    Center.X + dx * VoxelSize,
                    Center.Y + dy * VoxelSize,
                    Center.Z + dz * VoxelSize
                );

                // 过滤正方体外体素
                bool bIsInCube = (VoxelWorldPos.X >= CubeMin.X && VoxelWorldPos.X <= CubeMax.X)
                    && (VoxelWorldPos.Y >= CubeMin.Y && VoxelWorldPos.Y <= CubeMax.Y)
                    && (VoxelWorldPos.Z >= CubeMin.Z && VoxelWorldPos.Z <= CubeMax.Z);
                if (!bIsInCube)
                {
                    continue;
                }

                // 计算区块索引
                const int32 Xx = FMath::FloorToInt((VoxelWorldPos.X + HalfBlockX) / BlockSizeX);
                const int32 Yy = FMath::FloorToInt((VoxelWorldPos.Y + HalfBlockY) / BlockSizeY);
                const FIntVector CurrentBlock(Xx, Yy, 0);
                ModifiedBlocks.Add(CurrentBlock);

                // 区块偏移与局部顶点索引（保留边缘判断能力）
                const float BlockOffsetX = Xx * BlockSizeX;
                const float BlockOffsetY = Yy * BlockSizeY;
                const float BlockOffsetZ = 0.0f;

                const int32 X = FMath::RoundToInt((VoxelWorldPos.X - BlockOffsetX + HalfBlockX) * InvVoxelSize);
                const int32 Y = FMath::RoundToInt((VoxelWorldPos.Y - BlockOffsetY + HalfBlockY) * InvVoxelSize);
                const int32 Z = FMath::RoundToInt((VoxelWorldPos.Z - BlockOffsetZ) * InvVoxelSize);

                // 限制索引在有效范围内（避免越界）
                const int32 ClampedX = FMath::Clamp(X, 0, EdgeThresholdX);
                const int32 ClampedY = FMath::Clamp(Y, 0, EdgeThresholdY);
                const int32 ClampedZ = FMath::Clamp(Z, 0, EdgeThresholdZ);
                const int32 VertexIndex = ClampedX + ClampedY * VertexDimX + ClampedZ * VertexDimX * VertexDimY;
                const FString BlockKey = FString::Printf(TEXT("%d_%d"), Xx, Yy);

                // 读取原始SDF值
                FSSMap* BlockData = ReLoadMap.Find(BlockKey);
                if (!BlockData)
                {
                    BlockData = &ReLoadMap.Emplace(BlockKey, FSSMap());
                }

                float OriginalSDF = 0.0f;
                if (BlockData->SingleBlockMap.Contains(VertexIndex))
                {
                    OriginalSDF = BlockData->SingleBlockMap[VertexIndex];
                }
                else
                {
                    TArray<float>* CachedSDF = CachedBlockSDF.Find(BlockKey);
                    if (!CachedSDF)
                    {
                        SDFSpawn(FVector(Xx, Yy, 0));
                        CachedSDF = &CachedBlockSDF.Emplace(BlockKey, TV);
                    }
                    if (VertexIndex >= 0 && VertexIndex < CachedSDF->Num())
                    {
                        OriginalSDF = (*CachedSDF)[VertexIndex];
                    }
                }

                // 记录有效SDF值
                if (OriginalSDF >= 0.1f && OriginalSDF <= 3.0f)
                {
                    DugSDFValues.Add(OriginalSDF);
                }

                // 标记挖洞状态
                if (!BlockData->SingleBlockMap.Contains(VertexIndex))
                {
                    BlockData->SingleBlockMap.Add(VertexIndex, -1.0f);
                }

                // 【核心：完整边缘同步逻辑（包含所有边缘）】
                auto SyncAdjacentBlock = [&](int32 AdjX, int32 AdjY, int32 AdjVertexIdx)
                    {
                        const FString AdjBlockKey = FString::Printf(TEXT("%d_%d"), AdjX, AdjY);
                        FSSMap* AdjBlockData = ReLoadMap.Find(AdjBlockKey);
                        if (!AdjBlockData)
                        {
                            AdjBlockData = &ReLoadMap.Emplace(AdjBlockKey, FSSMap());
                        }

                        if (!AdjBlockData->SingleBlockMap.Contains(AdjVertexIdx))
                        {
                            TArray<float>* AdjCachedSDF = CachedBlockSDF.Find(AdjBlockKey);
                            if (!AdjCachedSDF)
                            {
                                SDFSpawn(FVector(AdjX, AdjY, 0)); // 强制生成相邻区块SDF
                                AdjCachedSDF = &CachedBlockSDF.Emplace(AdjBlockKey, TV);
                            }
                            if (AdjVertexIdx >= 0 && AdjVertexIdx < AdjCachedSDF->Num())
                            {
                                float AdjSDF = (*AdjCachedSDF)[AdjVertexIdx];
                                if (AdjSDF >= 0.1f && AdjSDF <= 3.0f)
                                {
                                    DugSDFValues.Add(AdjSDF);
                                }
                            }
                            AdjBlockData->SingleBlockMap.Add(AdjVertexIdx, -1.0f); // 同步标记挖洞
                        }
                        ModifiedBlocks.Add(FIntVector(AdjX, AdjY, 0)); // 记录需刷新的区块
                    };

                // 1. X轴边缘同步（包含最后一列）
                if (X == EdgeThresholdX) // X最后一列 → 同步右侧区块
                {
                    const int32 AdjVertexIdx = 0 + ClampedY * VertexDimX + ClampedZ * VertexDimX * VertexDimY;
                    SyncAdjacentBlock(Xx + 1, Yy, AdjVertexIdx);
                }
                else if (X == 0) // X第一列 → 同步左侧区块
                {
                    const int32 AdjVertexIdx = EdgeThresholdX + ClampedY * VertexDimX + ClampedZ * VertexDimX * VertexDimY;
                    SyncAdjacentBlock(Xx - 1, Yy, AdjVertexIdx);
                }

                // 2. Y轴边缘同步
                if (Y == EdgeThresholdY) // Y最后一行 → 同步上侧区块
                {
                    const int32 AdjVertexIdx = ClampedX + 0 * VertexDimX + ClampedZ * VertexDimX * VertexDimY;
                    SyncAdjacentBlock(Xx, Yy + 1, AdjVertexIdx);
                }
                else if (Y == 0) // Y第一行 → 同步下侧区块
                {
                    const int32 AdjVertexIdx = ClampedX + EdgeThresholdY * VertexDimX + ClampedZ * VertexDimX * VertexDimY;
                    SyncAdjacentBlock(Xx, Yy - 1, AdjVertexIdx);
                }

                // 3. 边角同步（X边缘 + Y边缘 → 同步对角区块）
                if ((X == 0 || X == EdgeThresholdX) && (Y == 0 || Y == EdgeThresholdY))
                {
                    int32 DiagX = (X == 0) ? Xx - 1 : Xx + 1;
                    int32 DiagY = (Y == 0) ? Yy - 1 : Yy + 1;
                    int32 DiagVertexIdx = (X == 0 ? EdgeThresholdX : 0) + (Y == 0 ? EdgeThresholdY : 0) * VertexDimX + ClampedZ * VertexDimX * VertexDimY;
                    SyncAdjacentBlock(DiagX, DiagY, DiagVertexIdx);
                }
            }
        }
    }

    // 6. 强制刷新区块（确保所有修改的区块都被更新）
    for (const FIntVector& Block : ModifiedBlocks)
    {
        SDFSpawn(FVector(Block.X, Block.Y, 0)); // 重新生成SDF数据
        const FString BlockKey = FString::Printf(TEXT("%d_%d"), Block.X, Block.Y);
        if (ATVoxel** VoxelActorPtr = TVtmap.Find(BlockKey); VoxelActorPtr && *VoxelActorPtr)
        {
            (*VoxelActorPtr)->GenerateTVoxelmin(TV); // 刷新网格显示
        }
    }

    return DugSDFValues;
}















//柏林噪音实现函数
// 柏林噪声置换表（256个元素，确保随机性）
// 置换表（256个元素）
const int32 ATLoadWorld::Permutation[256] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
    18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
    250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
    189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
    172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,
    192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,
    205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

// 扩展置换表（512元素，传统方式初始化，避免Lambda）
const int32 ATLoadWorld::PermutationExtended[512] = {
    // 前256个元素：复制Permutation
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
    18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
    250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
    189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
    172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,
    192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,
    205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,

    // 后256个元素：再次复制Permutation（确保索引不越界）
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
    18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
    250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
    189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
    172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,
    192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,
    205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

// 其余函数实现（Fade/Lerp/Grad/PerlinNoise2D/SDFSpawn/SpawnVoxel）保持不变
// 平滑曲线函数
float ATLoadWorld::Fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10); // 6t⁵ - 15t⁴ + 10t³
}

// 线性插值
float ATLoadWorld::Lerp(float t, float a, float b)
{
    return a + t * (b - a);
}

// 梯度计算
float ATLoadWorld::Grad(int32 hash, float x, float y)
{
    switch (hash & 0x3) // 取哈希值的最后2位
    {
    case 0x0: return x + y;
    case 0x1: return -x + y;
    case 0x2: return x - y;
    case 0x3: return -x - y;
    default: return 0.0f;
    }
}

// 2D柏林噪声（输出范围[-1, 1]）
float ATLoadWorld::PerlinNoise2D(float x, float y)
{
    // 计算网格单元坐标（整数部分）
    int32 X = FMath::FloorToInt(x) & 255; // 确保在0-255范围内
    int32 Y = FMath::FloorToInt(y) & 255;

    // 计算网格内偏移（小数部分）
    x -= FMath::Floor(x);
    y -= FMath::Floor(y);

    // 计算平滑因子
    float u = Fade(x);
    float v = Fade(y);

    // 获取哈希索引（使用扩展表，避免越界）
    int32 A = PermutationExtended[X] + Y;
    int32 B = PermutationExtended[X + 1] + Y;
    int32 AA = PermutationExtended[A];
    int32 AB = PermutationExtended[A + 1];
    int32 BA = PermutationExtended[B];
    int32 BB = PermutationExtended[B + 1];

    // 插值计算最终噪声值
    float q1 = Lerp(u, Grad(AA, x, y), Grad(BA, x - 1, y));
    float q2 = Lerp(u, Grad(AB, x, y - 1), Grad(BB, x - 1, y - 1));
    return Lerp(v, q1, q2);
}
