// Copyright 2026 Silvan Teufel All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInGameDemoActor.generated.h"

class SWidget;
class SGameInGamePanel;
class UMaterialInterface;

/**
 * Drop-in example actor: place it in a level (or spawn it) and call ShowOverlay to add the
 * minigame panel straight to the viewport — no UserWidget asset required. Demonstrates the
 * pure-C++ Slate usage path alongside the designer-facing UGameInGameWidget.
 */
UCLASS(Blueprintable)
class GAMEINGAME_API AGameInGameDemoActor : public AActor
{
	GENERATED_BODY()

public:
	AGameInGameDemoActor();

	/** Show the overlay automatically when play begins. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInGame")
	bool bShowOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInGame|Styling")
	TObjectPtr<UMaterialInterface> BorderMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInGame|Styling")
	FLinearColor BorderTint = FLinearColor(0.05f, 0.05f, 0.1f, 0.95f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInGame|Styling")
	float CornerStyle = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInGame|Input")
	bool bShowTouchButtons = false;

	/** Add the overlay to the viewport and give it input focus. */
	UFUNCTION(BlueprintCallable, Category = "GameInGame")
	void ShowOverlay();

	/** Remove the overlay from the viewport. */
	UFUNCTION(BlueprintCallable, Category = "GameInGame")
	void HideOverlay();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void HandleOverlayClosed();

	TSharedPtr<SGameInGamePanel> OverlayPanel;
	TSharedPtr<SWidget> ViewportContent;
};
