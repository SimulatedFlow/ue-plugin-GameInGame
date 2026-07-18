// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameInGameTypes.h"
#include "GameInGameMinigameInterface.generated.h"

class SGameInGameCanvas;

/** Reflected interface marker so minigames can be referenced via TScriptInterface. */
UINTERFACE(BlueprintType, Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEINGAME_API UGameInGameMinigameInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Contract every playable minigame implements. The overlay drives the game purely through
 * these calls, keeping the game logic isolated from the host world and its pause state.
 */
class GAMEINGAME_API IGameInGameMinigameInterface
{
	GENERATED_BODY()

public:
	/** Allocate / reset all state so the game is ready to be played from the start. */
	virtual void Init() = 0;

	/** Advance simulation. DeltaTime is real (Slate) time and ignores world pause / slow-mo. */
	virtual void Tick(float DeltaTime) = 0;

	/** React to a single logical input event. */
	virtual void HandleInput(EGameInGameInput InputType) = 0;

	/** Draw the current frame using the lightweight canvas draw helpers. */
	virtual void Render(class SGameInGameCanvas* Canvas) = 0;

	/** Restart from a clean state. */
	virtual void Reset() = 0;

	/** Current score. */
	virtual int32 GetScore() const = 0;

	/** Human-readable name shown in the selection menu and used as the high-score key. */
	virtual FString GetGameName() const = 0;

	/** Optional: true once the game has ended (win or loss). Default: never ends. */
	virtual bool IsGameOver() const { return false; }

	/** Optional: secondary contextual action (e.g. Minesweeper flag toggle). */
	virtual void HandleSecondaryAction() {}
};
