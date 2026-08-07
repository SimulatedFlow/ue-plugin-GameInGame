// Copyright 2026 Silvan Teufel All Rights Reserved.

#include "Games/GameInGameTetris.h"
#include "Slate/SGameInGameCanvas.h"

namespace GameInGameTetris
{
	// Piece types: 0=I 1=O 2=T 3=S 4=Z 5=J 6=L.
	// Each rotation lists four (x,y) cells inside a small bounding box (x right, y down).
	struct FShape { int32 NumRot; int32 Cells[4][4][2]; };

	static const FShape Shapes[7] =
	{
		// I (2 rotations)
		{ 2, { { {0,1},{1,1},{2,1},{3,1} }, { {2,0},{2,1},{2,2},{2,3} }, {}, {} } },
		// O (1 rotation)
		{ 1, { { {1,0},{2,0},{1,1},{2,1} }, {}, {}, {} } },
		// T (4)
		{ 4, { { {1,0},{0,1},{1,1},{2,1} }, { {1,0},{1,1},{2,1},{1,2} }, { {0,1},{1,1},{2,1},{1,2} }, { {1,0},{0,1},{1,1},{1,2} } } },
		// S (2)
		{ 2, { { {1,0},{2,0},{0,1},{1,1} }, { {1,0},{1,1},{2,1},{2,2} }, {}, {} } },
		// Z (2)
		{ 2, { { {0,0},{1,0},{1,1},{2,1} }, { {2,0},{1,1},{2,1},{1,2} }, {}, {} } },
		// J (4)
		{ 4, { { {0,0},{0,1},{1,1},{2,1} }, { {1,0},{2,0},{1,1},{1,2} }, { {0,1},{1,1},{2,1},{2,2} }, { {1,0},{1,1},{0,2},{1,2} } } },
		// L (4)
		{ 4, { { {2,0},{0,1},{1,1},{2,1} }, { {1,0},{1,1},{1,2},{2,2} }, { {0,1},{1,1},{2,1},{0,2} }, { {0,0},{1,0},{1,1},{1,2} } } },
	};
}

int32 UGameInGameTetris::GetRotationCount(int32 Type) const
{
	return GameInGameTetris::Shapes[FMath::Clamp(Type, 0, 6)].NumRot;
}

void UGameInGameTetris::GetPieceCells(int32 Type, int32 Rotation, FIntPoint (&OutCells)[4]) const
{
	Type = FMath::Clamp(Type, 0, 6);
	const GameInGameTetris::FShape& Shape = GameInGameTetris::Shapes[Type];
	Rotation = ((Rotation % Shape.NumRot) + Shape.NumRot) % Shape.NumRot;
	for (int32 i = 0; i < 4; ++i)
	{
		OutCells[i] = FIntPoint(Shape.Cells[Rotation][i][0], Shape.Cells[Rotation][i][1]);
	}
}

FLinearColor UGameInGameTetris::GetPieceColor(int32 Type) const
{
	static const FLinearColor Colors[7] =
	{
		FLinearColor(0.0f, 0.9f, 0.9f),   // I cyan
		FLinearColor(0.9f, 0.9f, 0.0f),   // O yellow
		FLinearColor(0.7f, 0.2f, 0.9f),   // T purple
		FLinearColor(0.1f, 0.9f, 0.2f),   // S green
		FLinearColor(0.9f, 0.1f, 0.1f),   // Z red
		FLinearColor(0.15f, 0.35f, 0.95f),// J blue
		FLinearColor(0.95f, 0.55f, 0.1f), // L orange
	};
	return Colors[FMath::Clamp(Type, 0, 6)];
}

void UGameInGameTetris::Reset()
{
	Super::Reset();
	Grid.Init(GridW, GridH);
	Grid.ClearCells();
	LinesCleared = 0;
	FallTimer = 0.0f;
	FallInterval = 0.6f;
	SpawnPiece();
}

bool UGameInGameTetris::DoesCollide(int32 Type, int32 Rotation, int32 OriginX, int32 OriginY) const
{
	FIntPoint Cells[4];
	GetPieceCells(Type, Rotation, Cells);
	for (const FIntPoint& C : Cells)
	{
		const int32 X = OriginX + C.X;
		const int32 Y = OriginY + C.Y;
		if (X < 0 || X >= GridW || Y >= GridH)
		{
			return true;
		}
		if (Y >= 0 && Grid.At(X, Y).bFilled)
		{
			return true;
		}
	}
	return false;
}

void UGameInGameTetris::SpawnPiece()
{
	CurrentType = FMath::RandRange(0, 6);
	CurrentRot = 0;
	PieceX = GridW / 2 - 2;
	PieceY = 0;

	if (DoesCollide(CurrentType, CurrentRot, PieceX, PieceY))
	{
		bGameOver = true;
	}
}

void UGameInGameTetris::LockPiece()
{
	FIntPoint Cells[4];
	GetPieceCells(CurrentType, CurrentRot, Cells);
	const FLinearColor Color = GetPieceColor(CurrentType);
	for (const FIntPoint& C : Cells)
	{
		const int32 X = PieceX + C.X;
		const int32 Y = PieceY + C.Y;
		if (Grid.IsValidCoord(X, Y))
		{
			FGameInGameCell& Cell = Grid.At(X, Y);
			Cell.bFilled = true;
			Cell.Color = Color;
		}
	}

	const int32 Cleared = ClearLines();
	if (Cleared > 0)
	{
		static const int32 LineScores[5] = { 0, 100, 300, 500, 800 };
		Score += LineScores[FMath::Clamp(Cleared, 0, 4)];
		LinesCleared += Cleared;
		FallInterval = FMath::Max(0.12f, 0.6f - LinesCleared * 0.015f);
	}

	SpawnPiece();
}

int32 UGameInGameTetris::ClearLines()
{
	int32 Cleared = 0;
	for (int32 Y = GridH - 1; Y >= 0; --Y)
	{
		bool bFull = true;
		for (int32 X = 0; X < GridW; ++X)
		{
			if (!Grid.At(X, Y).bFilled)
			{
				bFull = false;
				break;
			}
		}

		if (bFull)
		{
			++Cleared;
			// Shift everything above this row down by one.
			for (int32 YY = Y; YY > 0; --YY)
			{
				for (int32 X = 0; X < GridW; ++X)
				{
					Grid.At(X, YY) = Grid.At(X, YY - 1);
				}
			}
			for (int32 X = 0; X < GridW; ++X)
			{
				Grid.At(X, 0) = FGameInGameCell();
			}
			++Y; // Re-check the same row index after the shift.
		}
	}
	return Cleared;
}

bool UGameInGameTetris::TryRotate(int32 Dir)
{
	const int32 NewRot = CurrentRot + Dir;
	// Basic wall kicks: try in place, then nudge left/right.
	static const int32 Kicks[5] = { 0, -1, 1, -2, 2 };
	for (int32 K : Kicks)
	{
		if (!DoesCollide(CurrentType, NewRot, PieceX + K, PieceY))
		{
			CurrentRot = ((NewRot % GetRotationCount(CurrentType)) + GetRotationCount(CurrentType)) % GetRotationCount(CurrentType);
			PieceX += K;
			return true;
		}
	}
	return false;
}

void UGameInGameTetris::HardDrop()
{
	while (!DoesCollide(CurrentType, CurrentRot, PieceX, PieceY + 1))
	{
		++PieceY;
		Score += 1;
	}
	LockPiece();
}

void UGameInGameTetris::HandleInput(EGameInGameInput InputType)
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
	case EGameInGameInput::Left:
		if (!DoesCollide(CurrentType, CurrentRot, PieceX - 1, PieceY)) { --PieceX; }
		break;
	case EGameInGameInput::Right:
		if (!DoesCollide(CurrentType, CurrentRot, PieceX + 1, PieceY)) { ++PieceX; }
		break;
	case EGameInGameInput::Down:
		if (!DoesCollide(CurrentType, CurrentRot, PieceX, PieceY + 1)) { ++PieceY; FallTimer = 0.0f; }
		break;
	case EGameInGameInput::Up:
		TryRotate(1);
		break;
	case EGameInGameInput::Action:
		HardDrop();
		break;
	default:
		break;
	}
}

void UGameInGameTetris::Tick(float DeltaTime)
{
	if (bGameOver)
	{
		return;
	}

	FallTimer += DeltaTime;
	if (FallTimer >= FallInterval)
	{
		FallTimer = 0.0f;
		if (!DoesCollide(CurrentType, CurrentRot, PieceX, PieceY + 1))
		{
			++PieceY;
		}
		else
		{
			LockPiece();
		}
	}
}

void UGameInGameTetris::Render(SGameInGameCanvas* Canvas)
{
	if (!Canvas)
	{
		return;
	}

	// Playfield background.
	Canvas->FillRectN(0.0f, 0.0f, 1.0f, 1.0f, FLinearColor(0.04f, 0.04f, 0.07f, 1.0f));

	// Locked cells.
	for (int32 Y = 0; Y < GridH; ++Y)
	{
		for (int32 X = 0; X < GridW; ++X)
		{
			const FGameInGameCell& Cell = Grid.At(X, Y);
			if (Cell.bFilled)
			{
				Canvas->DrawGridCell(X, Y, GridW, GridH, Cell.Color, 0.06f);
			}
		}
	}

	// Active piece.
	if (!bGameOver)
	{
		FIntPoint Cells[4];
		GetPieceCells(CurrentType, CurrentRot, Cells);
		const FLinearColor Color = GetPieceColor(CurrentType);
		for (const FIntPoint& C : Cells)
		{
			const int32 X = PieceX + C.X;
			const int32 Y = PieceY + C.Y;
			if (X >= 0 && X < GridW && Y >= 0 && Y < GridH)
			{
				Canvas->DrawGridCell(X, Y, GridW, GridH, Color, 0.06f);
			}
		}
	}

	Canvas->DrawTextN(FString::Printf(TEXT("Score %d"), Score), 0.02f, 0.01f, FLinearColor::White, 11);

	if (bGameOver)
	{
		DrawGameOverBanner(Canvas);
	}
}
