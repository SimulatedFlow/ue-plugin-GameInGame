// Copyright 2026 Silvan Teufel All Rights Reserved.

#include "Games/GameInGameSnake.h"
#include "Slate/SGameInGameCanvas.h"

void UGameInGameSnake::Reset()
{
	Super::Reset();

	Body.Reset();
	const int32 CX = GridW / 2;
	const int32 CY = GridH / 2;
	Body.Add(FIntPoint(CX, CY));
	Body.Add(FIntPoint(CX - 1, CY));
	Body.Add(FIntPoint(CX - 2, CY));

	CurrentDir = FIntPoint(1, 0);
	DirQueue.Reset();
	MoveTimer = 0.0f;
	MoveInterval = 0.14f;
	SpawnFood();
}

void UGameInGameSnake::SpawnFood()
{
	// Collect free cells and pick one at random (safe even when the board is nearly full).
	TArray<FIntPoint> Free;
	Free.Reserve(GridW * GridH);
	for (int32 Y = 0; Y < GridH; ++Y)
	{
		for (int32 X = 0; X < GridW; ++X)
		{
			const FIntPoint P(X, Y);
			if (!Body.Contains(P))
			{
				Free.Add(P);
			}
		}
	}

	if (Free.Num() > 0)
	{
		Food = Free[FMath::RandRange(0, Free.Num() - 1)];
	}
	else
	{
		bGameOver = true; // Board full — perfect game.
	}
}

void UGameInGameSnake::EnqueueDir(const FIntPoint& Dir)
{
	// The direction we compare against is the last queued one (or the current heading).
	const FIntPoint Reference = DirQueue.Num() > 0 ? DirQueue.Last() : CurrentDir;

	// Reject reversals and no-ops; cap the queue so buffered turns stay responsive.
	if ((Dir.X == -Reference.X && Dir.Y == -Reference.Y) || (Dir == Reference))
	{
		return;
	}
	if (DirQueue.Num() < 2)
	{
		DirQueue.Add(Dir);
	}
}

void UGameInGameSnake::HandleInput(EGameInGameInput InputType)
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
	case EGameInGameInput::Up:    EnqueueDir(FIntPoint(0, -1)); break;
	case EGameInGameInput::Down:  EnqueueDir(FIntPoint(0, 1));  break;
	case EGameInGameInput::Left:  EnqueueDir(FIntPoint(-1, 0)); break;
	case EGameInGameInput::Right: EnqueueDir(FIntPoint(1, 0));  break;
	default: break;
	}
}

void UGameInGameSnake::Step()
{
	if (DirQueue.Num() > 0)
	{
		CurrentDir = DirQueue[0];
		DirQueue.RemoveAt(0);
	}

	const FIntPoint NewHead = Body[0] + CurrentDir;

	// Wall collision.
	if (NewHead.X < 0 || NewHead.X >= GridW || NewHead.Y < 0 || NewHead.Y >= GridH)
	{
		bGameOver = true;
		return;
	}

	const bool bEats = (NewHead == Food);

	// Self collision — the tail cell is freed this step unless we are growing.
	for (int32 i = 0; i < Body.Num(); ++i)
	{
		if (Body[i] == NewHead)
		{
			const bool bIsTail = (i == Body.Num() - 1);
			if (!(bIsTail && !bEats))
			{
				bGameOver = true;
				return;
			}
		}
	}

	Body.Insert(NewHead, 0);
	if (bEats)
	{
		Score += 10;
		MoveInterval = FMath::Max(0.06f, MoveInterval - 0.003f);
		SpawnFood();
	}
	else
	{
		Body.Pop();
	}
}

void UGameInGameSnake::Tick(float DeltaTime)
{
	if (bGameOver)
	{
		return;
	}

	MoveTimer += DeltaTime;
	if (MoveTimer >= MoveInterval)
	{
		MoveTimer = 0.0f;
		Step();
	}
}

void UGameInGameSnake::Render(SGameInGameCanvas* Canvas)
{
	if (!Canvas)
	{
		return;
	}

	Canvas->FillRectN(0.0f, 0.0f, 1.0f, 1.0f, FLinearColor(0.03f, 0.06f, 0.03f, 1.0f));

	Canvas->DrawGridCell(Food.X, Food.Y, GridW, GridH, FLinearColor(0.95f, 0.2f, 0.2f), 0.12f);

	for (int32 i = 0; i < Body.Num(); ++i)
	{
		const FLinearColor Color = (i == 0) ? FLinearColor(0.6f, 1.0f, 0.6f) : FLinearColor(0.25f, 0.8f, 0.35f);
		Canvas->DrawGridCell(Body[i].X, Body[i].Y, GridW, GridH, Color, 0.08f);
	}

	Canvas->DrawTextN(FString::Printf(TEXT("Score %d"), Score), 0.02f, 0.01f, FLinearColor::White, 11);

	if (bGameOver)
	{
		DrawGameOverBanner(Canvas);
	}
}
