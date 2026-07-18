// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#include "Games/GameInGameBreakout.h"
#include "Slate/SGameInGameCanvas.h"

void UGameInGameBreakout::Reset()
{
	Super::Reset();

	for (int32 i = 0; i < BrickCols * BrickRows; ++i)
	{
		bBricks[i] = true;
	}
	Lives = 3;
	PaddleX = 0.5f;
	BallSpeed = 0.55f;
	bWon = false;
	ResetBall();
}

void UGameInGameBreakout::ResetBall()
{
	bBallLaunched = false;
	Ball = FVector2f(PaddleX, PaddleY - BallRadius - 0.01f);
	BallVel = FVector2f(0.0f, 0.0f);
}

void UGameInGameBreakout::LaunchBall()
{
	if (!bBallLaunched)
	{
		bBallLaunched = true;
		const float Angle = FMath::DegreesToRadians(FMath::RandRange(-35.0f, 35.0f));
		BallVel = FVector2f(FMath::Sin(Angle), -FMath::Cos(Angle)) * BallSpeed;
	}
}

int32 UGameInGameBreakout::CountBricks() const
{
	int32 Count = 0;
	for (int32 i = 0; i < BrickCols * BrickRows; ++i)
	{
		if (bBricks[i])
		{
			++Count;
		}
	}
	return Count;
}

void UGameInGameBreakout::HandleInput(EGameInGameInput InputType)
{
	if (bGameOver)
	{
		if (InputType == EGameInGameInput::Action)
		{
			Reset();
		}
		return;
	}

	const float Step = 0.06f;
	switch (InputType)
	{
	case EGameInGameInput::Left:
		PaddleX = FMath::Clamp(PaddleX - Step, PaddleW * 0.5f, 1.0f - PaddleW * 0.5f);
		break;
	case EGameInGameInput::Right:
		PaddleX = FMath::Clamp(PaddleX + Step, PaddleW * 0.5f, 1.0f - PaddleW * 0.5f);
		break;
	case EGameInGameInput::Action:
		LaunchBall();
		break;
	default:
		break;
	}

	if (!bBallLaunched)
	{
		Ball.X = PaddleX;
	}
}

void UGameInGameBreakout::Tick(float DeltaTime)
{
	if (bGameOver || !bBallLaunched)
	{
		return;
	}

	// Clamp delta so a hitch cannot tunnel the ball through geometry.
	DeltaTime = FMath::Min(DeltaTime, 0.033f);

	Ball += BallVel * DeltaTime;

	// Wall bounces.
	if (Ball.X - BallRadius <= 0.0f)
	{
		Ball.X = BallRadius;
		BallVel.X = FMath::Abs(BallVel.X);
	}
	else if (Ball.X + BallRadius >= 1.0f)
	{
		Ball.X = 1.0f - BallRadius;
		BallVel.X = -FMath::Abs(BallVel.X);
	}
	if (Ball.Y - BallRadius <= 0.0f)
	{
		Ball.Y = BallRadius;
		BallVel.Y = FMath::Abs(BallVel.Y);
	}

	// Paddle collision.
	const float PaddleTop = PaddleY - PaddleH * 0.5f;
	if (BallVel.Y > 0.0f &&
		Ball.Y + BallRadius >= PaddleTop &&
		Ball.Y - BallRadius <= PaddleY + PaddleH * 0.5f &&
		Ball.X >= PaddleX - PaddleW * 0.5f &&
		Ball.X <= PaddleX + PaddleW * 0.5f)
	{
		Ball.Y = PaddleTop - BallRadius;
		const float Offset = (Ball.X - PaddleX) / (PaddleW * 0.5f); // -1..1
		const float Angle = FMath::DegreesToRadians(FMath::Clamp(Offset, -1.0f, 1.0f) * 60.0f);
		BallSpeed = FMath::Min(1.2f, BallSpeed + 0.01f);
		BallVel = FVector2f(FMath::Sin(Angle), -FMath::Cos(Angle)) * BallSpeed;
	}

	// Brick collisions.
	const float BrickTop = 0.08f;
	const float BrickAreaH = 0.30f;
	const float BW = 1.0f / BrickCols;
	const float BH = BrickAreaH / BrickRows;
	for (int32 Row = 0; Row < BrickRows; ++Row)
	{
		for (int32 Col = 0; Col < BrickCols; ++Col)
		{
			const int32 Index = Row * BrickCols + Col;
			if (!bBricks[Index])
			{
				continue;
			}
			const float BX = Col * BW;
			const float BY = BrickTop + Row * BH;
			if (Ball.X + BallRadius >= BX && Ball.X - BallRadius <= BX + BW &&
				Ball.Y + BallRadius >= BY && Ball.Y - BallRadius <= BY + BH)
			{
				bBricks[Index] = false;
				Score += (BrickRows - Row) * 10;
				// Reflect vertically (dominant case for a wall of bricks).
				BallVel.Y = -BallVel.Y;
				BallSpeed = FMath::Min(1.2f, BallSpeed + 0.005f);

				if (CountBricks() == 0)
				{
					bWon = true;
					bGameOver = true;
				}
				Row = BrickRows; // break out of both loops
				break;
			}
		}
	}

	// Ball lost.
	if (Ball.Y - BallRadius > 1.0f)
	{
		--Lives;
		if (Lives <= 0)
		{
			bGameOver = true;
		}
		else
		{
			BallSpeed = 0.55f;
			ResetBall();
		}
	}
}

void UGameInGameBreakout::Render(SGameInGameCanvas* Canvas)
{
	if (!Canvas)
	{
		return;
	}

	Canvas->FillRectN(0.0f, 0.0f, 1.0f, 1.0f, FLinearColor(0.02f, 0.02f, 0.05f, 1.0f));

	// Bricks.
	static const FLinearColor RowColors[3] =
	{
		FLinearColor(0.9f, 0.3f, 0.3f),
		FLinearColor(0.9f, 0.7f, 0.2f),
		FLinearColor(0.3f, 0.7f, 0.9f),
	};
	const float BrickTop = 0.08f;
	const float BrickAreaH = 0.30f;
	const float BW = 1.0f / BrickCols;
	const float BH = BrickAreaH / BrickRows;
	for (int32 Row = 0; Row < BrickRows; ++Row)
	{
		for (int32 Col = 0; Col < BrickCols; ++Col)
		{
			if (bBricks[Row * BrickCols + Col])
			{
				Canvas->FillRectN(Col * BW + 0.004f, BrickTop + Row * BH + 0.004f,
					BW - 0.008f, BH - 0.008f, RowColors[Row]);
			}
		}
	}

	// Paddle.
	Canvas->FillRectN(PaddleX - PaddleW * 0.5f, PaddleY - PaddleH * 0.5f, PaddleW, PaddleH,
		FLinearColor(0.85f, 0.85f, 0.95f));

	// Ball.
	Canvas->FillRectN(Ball.X - BallRadius, Ball.Y - BallRadius, BallRadius * 2.0f, BallRadius * 2.0f,
		FLinearColor::White);

	Canvas->DrawTextN(FString::Printf(TEXT("Score %d   Lives %d"), Score, FMath::Max(0, Lives)),
		0.02f, 0.005f, FLinearColor::White, 11);

	if (!bBallLaunched && !bGameOver)
	{
		Canvas->DrawTextN(TEXT("Press Action to launch"), 0.26f, 0.60f, FLinearColor(0.8f, 0.8f, 0.8f), 11);
	}

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
