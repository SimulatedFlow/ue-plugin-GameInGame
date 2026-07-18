// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/GameInGameMinigameBase.h"
#include "GameInGameBreakout.generated.h"

/** Breakout — float-physics paddle & ball with a 3-tier brick wall, 3 lives and speed escalation. */
UCLASS()
class GAMEINGAME_API UGameInGameBreakout : public UGameInGameMinigameBase
{
	GENERATED_BODY()

public:
	virtual void Reset() override;
	virtual void Tick(float DeltaTime) override;
	virtual void HandleInput(EGameInGameInput InputType) override;
	virtual void Render(class SGameInGameCanvas* Canvas) override;
	virtual FString GetGameName() const override { return TEXT("Breakout"); }

private:
	static constexpr int32 BrickCols = 8;
	static constexpr int32 BrickRows = 3;

	void ResetBall();
	void LaunchBall();
	int32 CountBricks() const;

	// All positions/sizes are normalized [0..1] within the canvas.
	float PaddleX = 0.5f;       // Paddle centre.
	float PaddleW = 0.20f;
	float PaddleY = 0.94f;
	float PaddleH = 0.025f;

	FVector2f Ball = FVector2f(0.5f, 0.9f);
	FVector2f BallVel = FVector2f(0.0f, 0.0f);
	float BallRadius = 0.015f;
	float BallSpeed = 0.55f;

	bool bBricks[BrickCols * BrickRows] = {};
	int32 Lives = 3;
	bool bBallLaunched = false;
	bool bWon = false;
};
