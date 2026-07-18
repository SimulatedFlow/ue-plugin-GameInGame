// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** A single logical cell shared by the grid-based minigames. */
struct FGameInGameCell
{
	/** Generic payload: piece id, tile value, mine/adjacency count, etc. */
	int32 Value = 0;

	/** Whether the cell is currently occupied / active. */
	bool bFilled = false;

	/** Tint used when the cell is drawn. */
	FLinearColor Color = FLinearColor::Transparent;
};

/**
 * Lightweight 2D cell grid shared by Tetris, Snake, Minesweeper and 2048.
 * Stores cell state and provides grid-space helpers; the canvas maps grid
 * coordinates to localized Slate pixels at draw time.
 */
struct GAMEINGAME_API FGameInGameGrid
{
	int32 Width = 0;
	int32 Height = 0;
	TArray<FGameInGameCell> Cells;

	void Init(int32 InWidth, int32 InHeight);
	void ClearCells();

	FORCEINLINE bool IsValidCoord(int32 X, int32 Y) const
	{
		return X >= 0 && X < Width && Y >= 0 && Y < Height;
	}

	FORCEINLINE int32 Index(int32 X, int32 Y) const { return Y * Width + X; }

	FGameInGameCell& At(int32 X, int32 Y);
	const FGameInGameCell& At(int32 X, int32 Y) const;
};
