// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/GameInGameMinigameBase.h"
#include "GameInGameMinesweeper.generated.h"

/** Minesweeper with first-click safety, flood-fill reveal, flagging and win detection. */
UCLASS()
class GAMEINGAME_API UGameInGameMinesweeper : public UGameInGameMinigameBase
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Reset() override;
	virtual void HandleInput(EGameInGameInput InputType) override;
	virtual void HandleSecondaryAction() override;
	virtual void Render(class SGameInGameCanvas* Canvas) override;
	virtual FString GetGameName() const override { return TEXT("Minesweeper"); }

	/** Configure board size (clamped to 8..12) and mine density. Call before Init/Reset. */
	void Configure(int32 InSize, int32 InMineCount);

private:
	struct FMineCell
	{
		bool bMine = false;
		bool bRevealed = false;
		bool bFlagged = false;
		int32 Adjacent = 0;
	};

	void GenerateField(int32 SafeX, int32 SafeY);
	void Reveal(int32 X, int32 Y);
	void RevealAt(int32 X, int32 Y);
	void ToggleFlag(int32 X, int32 Y);
	bool CheckWin() const;
	FORCEINLINE int32 Idx(int32 X, int32 Y) const { return Y * GridSize + X; }
	FORCEINLINE bool InBounds(int32 X, int32 Y) const { return X >= 0 && X < GridSize && Y >= 0 && Y < GridSize; }

	int32 GridSize = 9;
	int32 MineCount = 10;
	bool bFieldGenerated = false;
	bool bWon = false;
	int32 CursorX = 0;
	int32 CursorY = 0;

	TArray<FMineCell> Cells;
};
