// Copyright 2026 Simulated Flow All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameInGameMinigameInterface.h"
#include "GameInGameMinigameBase.generated.h"

/** Shared UObject base providing default interface plumbing and common state for the built-in games. */
UCLASS(Abstract)
class GAMEINGAME_API UGameInGameMinigameBase : public UObject, public IGameInGameMinigameInterface
{
	GENERATED_BODY()

public:
	// IGameInGameMinigameInterface — sensible defaults; concrete games override as needed.
	virtual void Init() override { Reset(); }
	virtual void Tick(float DeltaTime) override {}
	virtual void HandleInput(EGameInGameInput InputType) override {}
	virtual void Render(class SGameInGameCanvas* Canvas) override {}
	virtual void Reset() override { Score = 0; bGameOver = false; }
	virtual int32 GetScore() const override { return Score; }
	virtual FString GetGameName() const override { return TEXT("Minigame"); }
	virtual bool IsGameOver() const override { return bGameOver; }

protected:
	/** Draw a translucent full-canvas "GAME OVER" banner. Shared helper for concrete games. */
	void DrawGameOverBanner(class SGameInGameCanvas* Canvas) const;

	int32 Score = 0;
	bool bGameOver = false;
};
