// Copyright 2026 Silvan Teufel All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/GameInGameMinigameBase.h"
#include "GameInGame2048.generated.h"

/** 2048 — 4x4 sliding tile merge with spawn probabilities and game-over detection. */
UCLASS()
class GAMEINGAME_API UGameInGame2048 : public UGameInGameMinigameBase
{
	GENERATED_BODY()

public:
	virtual void Reset() override;
	virtual void HandleInput(EGameInGameInput InputType) override;
	virtual void Render(class SGameInGameCanvas* Canvas) override;
	virtual FString GetGameName() const override { return TEXT("2048"); }

private:
	static constexpr int32 Size = 4;

	FORCEINLINE int32 Idx(int32 X, int32 Y) const { return Y * Size + X; }

	bool Move(EGameInGameInput Dir);   // Returns true if the board changed.
	void SpawnTile();
	bool HasMovesLeft() const;
	FLinearColor TileColor(int32 Value) const;

	int32 Tiles[Size * Size] = {};
};
