// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/GameInGameMinigameBase.h"
#include "GameInGameGrid.h"
#include "GameInGameTetris.generated.h"

/** Classic Tetris on a 10x20 well with 7 tetrominoes, rotation + wall kicks, hard drop and line clears. */
UCLASS()
class GAMEINGAME_API UGameInGameTetris : public UGameInGameMinigameBase
{
	GENERATED_BODY()

public:
	virtual void Reset() override;
	virtual void Tick(float DeltaTime) override;
	virtual void HandleInput(EGameInGameInput InputType) override;
	virtual void Render(class SGameInGameCanvas* Canvas) override;
	virtual FString GetGameName() const override { return TEXT("Tetris"); }

private:
	static constexpr int32 GridW = 10;
	static constexpr int32 GridH = 20;

	/** Returns the four occupied cells of a piece type at a rotation, in bounding-box space. */
	void GetPieceCells(int32 Type, int32 Rotation, FIntPoint (&OutCells)[4]) const;
	int32 GetRotationCount(int32 Type) const;
	FLinearColor GetPieceColor(int32 Type) const;

	bool DoesCollide(int32 Type, int32 Rotation, int32 OriginX, int32 OriginY) const;
	void SpawnPiece();
	void LockPiece();
	int32 ClearLines();
	void HardDrop();
	bool TryRotate(int32 Dir);

	FGameInGameGrid Grid;
	int32 CurrentType = 0;
	int32 CurrentRot = 0;
	int32 PieceX = 0;
	int32 PieceY = 0;
	int32 LinesCleared = 0;

	float FallTimer = 0.0f;
	float FallInterval = 0.6f;
};
