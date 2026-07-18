// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameInGameMinigameInterface.h"
#include "GameInGameSubsystem.generated.h"

class UGameInGameSaveGame;

/**
 * Owns the registry of playable minigames and the high-score store. Created once per
 * GameInstance; auto-registers the 5 built-in games and (de)serializes high scores.
 */
UCLASS()
class GAMEINGAME_API UGameInGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Add a user-provided minigame to the registry. */
	UFUNCTION(BlueprintCallable, Category = "GameInGame")
	void RegisterCustomMinigame(TScriptInterface<IGameInGameMinigameInterface> NewGame);

	/** All currently registered minigames (built-in + custom). */
	UFUNCTION(BlueprintCallable, Category = "GameInGame")
	TArray<TScriptInterface<IGameInGameMinigameInterface>> GetRegisteredMinigames() const;

	/** Highest recorded score for a game, or 0 if none. */
	UFUNCTION(BlueprintCallable, Category = "GameInGame")
	int32 GetHighScore(const FString& GameName) const;

	/** Records a score; returns true and persists if it beats the previous best. */
	UFUNCTION(BlueprintCallable, Category = "GameInGame")
	bool SubmitScore(const FString& GameName, int32 Score);

	/** Force a flush of the high-score save game to disk. */
	UFUNCTION(BlueprintCallable, Category = "GameInGame")
	void SaveHighScores();

private:
	void LoadHighScores();

	UPROPERTY(Transient)
	TArray<TScriptInterface<IGameInGameMinigameInterface>> RegisteredGames;

	UPROPERTY(Transient)
	TObjectPtr<UGameInGameSaveGame> SaveData;

	static const FString SaveSlotName;
	static const int32 SaveUserIndex;
};
