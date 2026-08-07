// Copyright 2026 Simulated Flow All Rights Reserved.

#include "GameInGameGrid.h"

void FGameInGameGrid::Init(int32 InWidth, int32 InHeight)
{
	Width = FMath::Max(0, InWidth);
	Height = FMath::Max(0, InHeight);
	Cells.Reset();
	Cells.SetNum(Width * Height);
}

void FGameInGameGrid::ClearCells()
{
	for (FGameInGameCell& Cell : Cells)
	{
		Cell = FGameInGameCell();
	}
}

FGameInGameCell& FGameInGameGrid::At(int32 X, int32 Y)
{
	check(IsValidCoord(X, Y));
	return Cells[Index(X, Y)];
}

const FGameInGameCell& FGameInGameGrid::At(int32 X, int32 Y) const
{
	check(IsValidCoord(X, Y));
	return Cells[Index(X, Y)];
}
