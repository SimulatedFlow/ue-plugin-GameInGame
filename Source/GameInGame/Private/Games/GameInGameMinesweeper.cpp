// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#include "Games/GameInGameMinesweeper.h"
#include "Slate/SGameInGameCanvas.h"

void UGameInGameMinesweeper::Configure(int32 InSize, int32 InMineCount)
{
	GridSize = FMath::Clamp(InSize, 8, 12);
	MineCount = FMath::Clamp(InMineCount, 1, GridSize * GridSize - 9);
}

void UGameInGameMinesweeper::Init()
{
	Reset();
}

void UGameInGameMinesweeper::Reset()
{
	Super::Reset();

	GridSize = FMath::Clamp(GridSize, 8, 12);
	MineCount = FMath::Clamp(MineCount, 1, GridSize * GridSize - 9);

	Cells.Reset();
	Cells.SetNum(GridSize * GridSize);
	bFieldGenerated = false;
	bWon = false;
	CursorX = 0;
	CursorY = 0;
}

void UGameInGameMinesweeper::GenerateField(int32 SafeX, int32 SafeY)
{
	// Build a pool of candidate cells excluding the first click and its neighbours.
	TArray<int32> Pool;
	Pool.Reserve(GridSize * GridSize);
	for (int32 Y = 0; Y < GridSize; ++Y)
	{
		for (int32 X = 0; X < GridSize; ++X)
		{
			if (FMath::Abs(X - SafeX) <= 1 && FMath::Abs(Y - SafeY) <= 1)
			{
				continue;
			}
			Pool.Add(Idx(X, Y));
		}
	}

	const int32 Mines = FMath::Min(MineCount, Pool.Num());
	for (int32 i = 0; i < Mines; ++i)
	{
		const int32 Pick = FMath::RandRange(0, Pool.Num() - 1);
		Cells[Pool[Pick]].bMine = true;
		Pool.RemoveAtSwap(Pick);
	}

	// Precompute adjacency counts.
	for (int32 Y = 0; Y < GridSize; ++Y)
	{
		for (int32 X = 0; X < GridSize; ++X)
		{
			if (Cells[Idx(X, Y)].bMine)
			{
				continue;
			}
			int32 Count = 0;
			for (int32 DY = -1; DY <= 1; ++DY)
			{
				for (int32 DX = -1; DX <= 1; ++DX)
				{
					if (DX == 0 && DY == 0)
					{
						continue;
					}
					const int32 NX = X + DX;
					const int32 NY = Y + DY;
					if (InBounds(NX, NY) && Cells[Idx(NX, NY)].bMine)
					{
						++Count;
					}
				}
			}
			Cells[Idx(X, Y)].Adjacent = Count;
		}
	}

	bFieldGenerated = true;
}

void UGameInGameMinesweeper::Reveal(int32 X, int32 Y)
{
	if (!InBounds(X, Y))
	{
		return;
	}
	FMineCell& Cell = Cells[Idx(X, Y)];
	if (Cell.bRevealed || Cell.bFlagged)
	{
		return;
	}

	Cell.bRevealed = true;

	if (Cell.Adjacent == 0 && !Cell.bMine)
	{
		// Flood fill neighbours.
		for (int32 DY = -1; DY <= 1; ++DY)
		{
			for (int32 DX = -1; DX <= 1; ++DX)
			{
				if (DX != 0 || DY != 0)
				{
					Reveal(X + DX, Y + DY);
				}
			}
		}
	}
}

void UGameInGameMinesweeper::RevealAt(int32 X, int32 Y)
{
	if (bGameOver || !InBounds(X, Y))
	{
		return;
	}

	if (!bFieldGenerated)
	{
		GenerateField(X, Y);
	}

	FMineCell& Cell = Cells[Idx(X, Y)];
	if (Cell.bFlagged || Cell.bRevealed)
	{
		return;
	}

	if (Cell.bMine)
	{
		Cell.bRevealed = true;
		bGameOver = true;
		// Expose all mines.
		for (FMineCell& C : Cells)
		{
			if (C.bMine)
			{
				C.bRevealed = true;
			}
		}
		return;
	}

	Reveal(X, Y);

	// Score: number of safely revealed cells.
	int32 Revealed = 0;
	for (const FMineCell& C : Cells)
	{
		if (C.bRevealed && !C.bMine)
		{
			++Revealed;
		}
	}
	Score = Revealed;

	if (CheckWin())
	{
		bWon = true;
		bGameOver = true;
		Score += 100;
	}
}

void UGameInGameMinesweeper::ToggleFlag(int32 X, int32 Y)
{
	if (bGameOver || !InBounds(X, Y))
	{
		return;
	}
	FMineCell& Cell = Cells[Idx(X, Y)];
	if (!Cell.bRevealed)
	{
		Cell.bFlagged = !Cell.bFlagged;
	}
}

bool UGameInGameMinesweeper::CheckWin() const
{
	for (const FMineCell& C : Cells)
	{
		if (!C.bMine && !C.bRevealed)
		{
			return false;
		}
	}
	return true;
}

void UGameInGameMinesweeper::HandleInput(EGameInGameInput InputType)
{
	if (bGameOver)
	{
		if (InputType == EGameInGameInput::Action)
		{
			Reset();
		}
		return;
	}

	switch (InputType)
	{
	case EGameInGameInput::Up:    CursorY = FMath::Max(0, CursorY - 1); break;
	case EGameInGameInput::Down:  CursorY = FMath::Min(GridSize - 1, CursorY + 1); break;
	case EGameInGameInput::Left:  CursorX = FMath::Max(0, CursorX - 1); break;
	case EGameInGameInput::Right: CursorX = FMath::Min(GridSize - 1, CursorX + 1); break;
	case EGameInGameInput::Action: RevealAt(CursorX, CursorY); break;
	default: break;
	}
}

void UGameInGameMinesweeper::HandleSecondaryAction()
{
	ToggleFlag(CursorX, CursorY);
}

void UGameInGameMinesweeper::Render(SGameInGameCanvas* Canvas)
{
	if (!Canvas)
	{
		return;
	}

	Canvas->FillRectN(0.0f, 0.0f, 1.0f, 1.0f, FLinearColor(0.10f, 0.10f, 0.12f, 1.0f));

	static const FLinearColor NumberColors[9] =
	{
		FLinearColor::White,
		FLinearColor(0.3f, 0.5f, 1.0f),
		FLinearColor(0.2f, 0.8f, 0.2f),
		FLinearColor(0.9f, 0.3f, 0.3f),
		FLinearColor(0.5f, 0.3f, 0.9f),
		FLinearColor(0.9f, 0.6f, 0.2f),
		FLinearColor(0.2f, 0.8f, 0.8f),
		FLinearColor(0.9f, 0.9f, 0.9f),
		FLinearColor(0.7f, 0.7f, 0.7f),
	};

	const float CW = 1.0f / GridSize;
	const float CH = 1.0f / GridSize;

	for (int32 Y = 0; Y < GridSize; ++Y)
	{
		for (int32 X = 0; X < GridSize; ++X)
		{
			const FMineCell& Cell = Cells[Idx(X, Y)];
			if (Cell.bRevealed)
			{
				if (Cell.bMine)
				{
					Canvas->DrawGridCell(X, Y, GridSize, GridSize, FLinearColor(0.8f, 0.1f, 0.1f), 0.05f);
				}
				else
				{
					Canvas->DrawGridCell(X, Y, GridSize, GridSize, FLinearColor(0.22f, 0.22f, 0.26f), 0.05f);
					if (Cell.Adjacent > 0)
					{
						Canvas->DrawTextN(FString::FromInt(Cell.Adjacent),
							X * CW + CW * 0.32f, Y * CH + CH * 0.18f,
							NumberColors[FMath::Clamp(Cell.Adjacent, 0, 8)], 12);
					}
				}
			}
			else
			{
				Canvas->DrawGridCell(X, Y, GridSize, GridSize, FLinearColor(0.45f, 0.47f, 0.52f), 0.05f);
				if (Cell.bFlagged)
				{
					Canvas->DrawGridCell(X, Y, GridSize, GridSize, FLinearColor(0.95f, 0.75f, 0.1f), 0.28f);
				}
			}
		}
	}

	// Cursor highlight.
	Canvas->StrokeRectN(CursorX * CW, CursorY * CH, CW, CH, FLinearColor(1.0f, 1.0f, 1.0f), 2.0f);

	Canvas->DrawTextN(FString::Printf(TEXT("Revealed %d"), Score), 0.02f, 0.005f, FLinearColor::White, 10);

	if (bGameOver)
	{
		if (bWon)
		{
			Canvas->FillRectN(0.0f, 0.40f, 1.0f, 0.20f, FLinearColor(0.0f, 0.0f, 0.0f, 0.65f));
			Canvas->DrawTextN(TEXT("YOU WIN!"), 0.36f, 0.45f, FLinearColor(0.4f, 1.0f, 0.4f), 20);
		}
		else
		{
			DrawGameOverBanner(Canvas);
		}
	}
}
