// Copyright 2026 Simulated Flow All Rights Reserved.

#include "Games/GameInGameMinigameBase.h"
#include "Slate/SGameInGameCanvas.h"

void UGameInGameMinigameBase::DrawGameOverBanner(SGameInGameCanvas* Canvas) const
{
	if (!Canvas)
	{
		return;
	}

	Canvas->FillRectN(0.0f, 0.40f, 1.0f, 0.20f, FLinearColor(0.0f, 0.0f, 0.0f, 0.65f));
	Canvas->DrawTextN(TEXT("GAME OVER"), 0.34f, 0.44f, FLinearColor::White, 20);
	Canvas->DrawTextN(TEXT("Press Action to restart"), 0.30f, 0.52f, FLinearColor(0.8f, 0.8f, 0.8f, 1.0f), 11);
}
