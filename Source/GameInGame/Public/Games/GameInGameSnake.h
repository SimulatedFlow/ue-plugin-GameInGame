// Copyright 2026 Simulated Flow All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/GameInGameMinigameBase.h"
#include "GameInGameSnake.generated.h"

/** Snake on a 20x20 field with an input queue that prevents rapid-input self reversal. */
UCLASS()
class GAMEINGAME_API UGameInGameSnake : public UGameInGameMinigameBase
{
	GENERATED_BODY()

public:
	virtual void Reset() override;
	virtual void Tick(float DeltaTime) override;
	virtual void HandleInput(EGameInGameInput InputType) override;
	virtual void Render(class SGameInGameCanvas* Canvas) override;
	virtual FString GetGameName() const override { return TEXT("Snake"); }

private:
	static constexpr int32 GridW = 20;
	static constexpr int32 GridH = 20;

	void Step();
	void SpawnFood();
	void EnqueueDir(const FIntPoint& Dir);

	TArray<FIntPoint> Body;    // Body[0] is the head.
	FIntPoint CurrentDir = FIntPoint(1, 0);
	TArray<FIntPoint> DirQueue;
	FIntPoint Food = FIntPoint(0, 0);

	float MoveTimer = 0.0f;
	float MoveInterval = 0.14f;
};
