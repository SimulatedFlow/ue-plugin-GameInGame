// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#include "GameInGameSubsystem.h"
#include "GameInGameSaveGame.h"
#include "GameInGameLog.h"
#include "Kismet/GameplayStatics.h"

#include "Games/GameInGameTetris.h"
#include "Games/GameInGameSnake.h"
#include "Games/GameInGameMinesweeper.h"
#include "Games/GameInGame2048.h"
#include "Games/GameInGameBreakout.h"

const FString UGameInGameSubsystem::SaveSlotName = TEXT("GameInGameHighScores");
const int32 UGameInGameSubsystem::SaveUserIndex = 0;

void UGameInGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadHighScores();

	// Auto-register the five built-in minigames.
	RegisteredGames.Reset();
	RegisteredGames.Add(TScriptInterface<IGameInGameMinigameInterface>(NewObject<UGameInGameTetris>(this)));
	RegisteredGames.Add(TScriptInterface<IGameInGameMinigameInterface>(NewObject<UGameInGameSnake>(this)));
	RegisteredGames.Add(TScriptInterface<IGameInGameMinigameInterface>(NewObject<UGameInGameMinesweeper>(this)));
	RegisteredGames.Add(TScriptInterface<IGameInGameMinigameInterface>(NewObject<UGameInGame2048>(this)));
	RegisteredGames.Add(TScriptInterface<IGameInGameMinigameInterface>(NewObject<UGameInGameBreakout>(this)));

	UE_LOG(LogGameInGame, Log, TEXT("GameInGame subsystem initialized with %d minigames."), RegisteredGames.Num());
}

void UGameInGameSubsystem::Deinitialize()
{
	SaveHighScores();
	RegisteredGames.Reset();
	Super::Deinitialize();
}

void UGameInGameSubsystem::RegisterCustomMinigame(TScriptInterface<IGameInGameMinigameInterface> NewGame)
{
	if (NewGame.GetObject() && NewGame.GetInterface())
	{
		RegisteredGames.AddUnique(NewGame);
	}
}

TArray<TScriptInterface<IGameInGameMinigameInterface>> UGameInGameSubsystem::GetRegisteredMinigames() const
{
	return RegisteredGames;
}

int32 UGameInGameSubsystem::GetHighScore(const FString& GameName) const
{
	if (SaveData)
	{
		if (const int32* Found = SaveData->HighScores.Find(GameName))
		{
			return *Found;
		}
	}
	return 0;
}

bool UGameInGameSubsystem::SubmitScore(const FString& GameName, int32 Score)
{
	if (!SaveData)
	{
		return false;
	}

	const int32* Existing = SaveData->HighScores.Find(GameName);
	if (!Existing || Score > *Existing)
	{
		SaveData->HighScores.Add(GameName, Score);
		SaveHighScores();
		return true;
	}
	return false;
}

void UGameInGameSubsystem::SaveHighScores()
{
	if (SaveData)
	{
		UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, SaveUserIndex);
	}
}

void UGameInGameSubsystem::LoadHighScores()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		SaveData = Cast<UGameInGameSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
	}

	if (!SaveData)
	{
		SaveData = Cast<UGameInGameSaveGame>(UGameplayStatics::CreateSaveGameObject(UGameInGameSaveGame::StaticClass()));
	}
}
