// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameInGameTypes.generated.h"

/** Logical inputs a minigame understands, decoupled from physical keys / gamepad / touch. */
UENUM(BlueprintType)
enum class EGameInGameInput : uint8
{
	Up		UMETA(DisplayName = "Up"),
	Down	UMETA(DisplayName = "Down"),
	Left	UMETA(DisplayName = "Left"),
	Right	UMETA(DisplayName = "Right"),
	Action	UMETA(DisplayName = "Action"),
	Back	UMETA(DisplayName = "Back")
};
