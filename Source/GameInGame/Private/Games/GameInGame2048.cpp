// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#include "Games/GameInGame2048.h"
#include "Slate/SGameInGameCanvas.h"

namespace
{
	/** Compact non-zero values toward index 0 and merge equal neighbours. Returns score gained. */
	int32 SlideLine(int32 (&Line)[4])
	{
		int32 Packed[4] = { 0, 0, 0, 0 };
		int32 Count = 0;
		for (int32 i = 0; i < 4; ++i)
		{
			if (Line[i] != 0)
			{
				Packed[Count++] = Line[i];
			}
		}

		int32 Result[4] = { 0, 0, 0, 0 };
		int32 Out = 0;
		int32 Gained = 0;
		for (int32 i = 0; i < Count; ++i)
		{
			if (i + 1 < Count && Packed[i] == Packed[i + 1])
			{
				const int32 Merged = Packed[i] * 2;
				Result[Out++] = Merged;
				Gained += Merged;
				++i; // Consume the merged partner.
			}
			else
			{
				Result[Out++] = Packed[i];
			}
		}

		for (int32 i = 0; i < 4; ++i)
		{
			Line[i] = Result[i];
		}
		return Gained;
	}
}

void UGameInGame2048::Reset()
{
	Super::Reset();
	for (int32 i = 0; i < Size * Size; ++i)
	{
		Tiles[i] = 0;
	}
	SpawnTile();
	SpawnTile();
}

void UGameInGame2048::SpawnTile()
{
	TArray<int32, TInlineAllocator<16>> Empty;
	for (int32 i = 0; i < Size * Size; ++i)
	{
		if (Tiles[i] == 0)
		{
			Empty.Add(i);
		}
	}
	if (Empty.Num() == 0)
	{
		return;
	}
	const int32 Cell = Empty[FMath::RandRange(0, Empty.Num() - 1)];
	Tiles[Cell] = (FMath::FRand() < 0.9f) ? 2 : 4;
}

bool UGameInGame2048::Move(EGameInGameInput Dir)
{
	bool bChanged = false;

	for (int32 Lane = 0; Lane < Size; ++Lane)
	{
		// Gather the lane into forward order (toward the merge target first).
		int32 Line[4] = { 0, 0, 0, 0 };
		int32 Coords[4] = { 0, 0, 0, 0 };

		for (int32 i = 0; i < Size; ++i)
		{
			int32 X = 0, Y = 0;
			switch (Dir)
			{
			case EGameInGameInput::Left:  X = i;             Y = Lane;          break;
			case EGameInGameInput::Right: X = Size - 1 - i;  Y = Lane;          break;
			case EGameInGameInput::Up:    X = Lane;          Y = i;             break;
			case EGameInGameInput::Down:  X = Lane;          Y = Size - 1 - i;  break;
			default: break;
			}
			Coords[i] = Idx(X, Y);
			Line[i] = Tiles[Coords[i]];
		}

		int32 Original[4];
		FMemory::Memcpy(Original, Line, sizeof(Line));

		Score += SlideLine(Line);

		for (int32 i = 0; i < Size; ++i)
		{
			if (Line[i] != Original[i])
			{
				bChanged = true;
			}
			Tiles[Coords[i]] = Line[i];
		}
	}

	return bChanged;
}

bool UGameInGame2048::HasMovesLeft() const
{
	for (int32 i = 0; i < Size * Size; ++i)
	{
		if (Tiles[i] == 0)
		{
			return true;
		}
	}
	for (int32 Y = 0; Y < Size; ++Y)
	{
		for (int32 X = 0; X < Size; ++X)
		{
			const int32 V = Tiles[Idx(X, Y)];
			if (X + 1 < Size && Tiles[Idx(X + 1, Y)] == V) { return true; }
			if (Y + 1 < Size && Tiles[Idx(X, Y + 1)] == V) { return true; }
		}
	}
	return false;
}

void UGameInGame2048::HandleInput(EGameInGameInput InputType)
{
	if (bGameOver)
	{
		if (InputType == EGameInGameInput::Action)
		{
			Reset();
		}
		return;
	}

	if (InputType == EGameInGameInput::Up || InputType == EGameInGameInput::Down ||
		InputType == EGameInGameInput::Left || InputType == EGameInGameInput::Right)
	{
		if (Move(InputType))
		{
			SpawnTile();
			if (!HasMovesLeft())
			{
				bGameOver = true;
			}
		}
	}
}

FLinearColor UGameInGame2048::TileColor(int32 Value) const
{
	switch (Value)
	{
	case 2:    return FLinearColor(0.93f, 0.89f, 0.85f);
	case 4:    return FLinearColor(0.93f, 0.88f, 0.78f);
	case 8:    return FLinearColor(0.95f, 0.69f, 0.47f);
	case 16:   return FLinearColor(0.96f, 0.58f, 0.39f);
	case 32:   return FLinearColor(0.96f, 0.49f, 0.37f);
	case 64:   return FLinearColor(0.96f, 0.37f, 0.23f);
	case 128:  return FLinearColor(0.93f, 0.81f, 0.45f);
	case 256:  return FLinearColor(0.93f, 0.80f, 0.38f);
	case 512:  return FLinearColor(0.93f, 0.78f, 0.31f);
	case 1024: return FLinearColor(0.93f, 0.77f, 0.25f);
	case 2048: return FLinearColor(0.93f, 0.76f, 0.18f);
	default:   return FLinearColor(0.4f, 0.68f, 0.42f);
	}
}

void UGameInGame2048::Render(SGameInGameCanvas* Canvas)
{
	if (!Canvas)
	{
		return;
	}

	Canvas->FillRectN(0.0f, 0.0f, 1.0f, 1.0f, FLinearColor(0.18f, 0.17f, 0.15f, 1.0f));

	const float CW = 1.0f / Size;
	const float CH = 1.0f / Size;

	for (int32 Y = 0; Y < Size; ++Y)
	{
		for (int32 X = 0; X < Size; ++X)
		{
			const int32 Value = Tiles[Idx(X, Y)];
			const FLinearColor Color = (Value == 0) ? FLinearColor(0.27f, 0.25f, 0.23f) : TileColor(Value);
			Canvas->DrawGridCell(X, Y, Size, Size, Color, 0.06f);
			if (Value != 0)
			{
				const int32 FontSize = Value >= 1000 ? 14 : (Value >= 100 ? 16 : 18);
				Canvas->DrawTextN(FString::FromInt(Value),
					X * CW + CW * 0.30f, Y * CH + CH * 0.32f,
					FLinearColor(0.15f, 0.13f, 0.11f), FontSize);
			}
		}
	}

	Canvas->DrawTextN(FString::Printf(TEXT("Score %d"), Score), 0.02f, 0.005f, FLinearColor::White, 11);

	if (bGameOver)
	{
		DrawGameOverBanner(Canvas);
	}
}
