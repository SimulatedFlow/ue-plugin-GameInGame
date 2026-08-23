// Copyright 2026 Silvan Teufel All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameInGameSaveGame.generated.h"

/** Persists per-game high scores keyed by GameName. */
UCLASS()
class GAMEINGAME_API UGameInGameSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameInGame")
	TMap<FString, int32> HighScores;
};
