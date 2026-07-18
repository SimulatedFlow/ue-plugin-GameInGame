// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#include "Slate/SGameInGamePanel.h"
#include "Slate/SGameInGameCanvas.h"
#include "GameInGameSubsystem.h"
#include "GameInGameMinigameInterface.h"

#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Materials/MaterialInterface.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/CoreStyle.h"

void SGameInGamePanel::Construct(const FArguments& InArgs)
{
	BorderMaterial = InArgs._BorderMaterial;
	BorderPadding = InArgs._BorderPadding;
	BorderTint = InArgs._BorderTint;
	CornerStyle = InArgs._CornerStyle;
	bShowTouchButtons = InArgs._ShowTouchButtons;
	SubsystemWeak = InArgs._Subsystem;
	OnCompleted = InArgs._OnMinigameCompleted;
	OnClosed = InArgs._OnOverlayClosed;

	ApplyBrush();
	RefreshGames();

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(&BackgroundBrush)
		.BorderBackgroundColor(BorderTint)
		.Padding(BorderPadding)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SBox)
				.Visibility_Lambda([this]()
				{
					return State == EPanelState::Menu ? EVisibility::Visible : EVisibility::Collapsed;
				})
				[
					BuildMenu()
				]
			]
			+ SOverlay::Slot()
			[
				SNew(SBox)
				.Visibility_Lambda([this]()
				{
					return State == EPanelState::Playing ? EVisibility::Visible : EVisibility::Collapsed;
				})
				[
					BuildPlayArea()
				]
			]
		]
	];
}

void SGameInGamePanel::ApplyBrush()
{
	BackgroundBrush = FSlateBrush();
	BackgroundBrush.TintColor = BorderTint;

	if (BorderMaterial)
	{
		BackgroundBrush.SetResourceObject(BorderMaterial);
		BackgroundBrush.DrawAs = ESlateBrushDrawType::Image;
	}
	else if (CornerStyle > 0.0f)
	{
		BackgroundBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
		BackgroundBrush.OutlineSettings.CornerRadii = FVector4(CornerStyle, CornerStyle, CornerStyle, CornerStyle);
		BackgroundBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	}
	else
	{
		BackgroundBrush.DrawAs = ESlateBrushDrawType::Box;
	}
}

void SGameInGamePanel::UpdateStyle(UMaterialInterface* InMaterial, const FMargin& InPadding, const FLinearColor& InTint, float InCornerStyle)
{
	BorderMaterial = InMaterial;
	BorderPadding = InPadding;
	BorderTint = InTint;
	CornerStyle = InCornerStyle;
	ApplyBrush();
}

void SGameInGamePanel::RefreshGames()
{
	Games.Reset();
	GameNames.Reset();

	if (UGameInGameSubsystem* Sub = SubsystemWeak.Get())
	{
		for (const TScriptInterface<IGameInGameMinigameInterface>& Ref : Sub->GetRegisteredMinigames())
		{
			if (UObject* Obj = Ref.GetObject())
			{
				Games.Add(Obj);
				GameNames.Add(Ref.GetInterface() ? Ref.GetInterface()->GetGameName() : Obj->GetName());
			}
		}
	}

	SelectedIndex = Games.Num() > 0 ? FMath::Clamp(SelectedIndex, 0, Games.Num() - 1) : 0;
}

TSharedRef<SWidget> SGameInGamePanel::BuildMenu()
{
	SAssignNew(MenuList, SVerticalBox);

	if (Games.Num() == 0)
	{
		MenuList->AddSlot()
		.AutoHeight()
		.Padding(4.0f)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("GameInGame", "NoGames", "No minigames registered.\n(Run in PIE so the GameInstance subsystem exists.)"))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.9f, 0.9f)))
		];
	}

	for (int32 i = 0; i < GameNames.Num(); ++i)
	{
		const int32 Index = i;
		MenuList->AddSlot()
		.AutoHeight()
		.Padding(2.0f)
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
			.OnClicked_Lambda([this, Index]()
			{
				StartGame(Index);
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Text_Lambda([this, Index]()
				{
					const FString Prefix = (Index == SelectedIndex) ? TEXT("> ") : TEXT("   ");
					const int32 High = SubsystemWeak.IsValid() ? SubsystemWeak->GetHighScore(GameNames[Index]) : 0;
					return FText::FromString(FString::Printf(TEXT("%s%s   (Best %d)"), *Prefix, *GameNames[Index], High));
				})
			]
		];
	}

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(6.0f)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("GameInGame", "Title", "GAME IN GAME"))
			.ColorAndOpacity(FSlateColor(FLinearColor::White))
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			MenuList.ToSharedRef()
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(4.0f)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("GameInGame", "MenuHint", "Arrows/Stick: select   Action: play   Back: close"))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)))
		];
}

TSharedRef<SWidget> SGameInGamePanel::BuildPlayArea()
{
	TSharedRef<SVerticalBox> Root =
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2.0f)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Text_Lambda([this]()
			{
				if (IGameInGameMinigameInterface* Game = GetActiveGameInterface())
				{
					return FText::FromString(FString::Printf(TEXT("%s   —   Score %d"), *Game->GetGameName(), Game->GetScore()));
				}
				return FText::GetEmpty();
			})
			.ColorAndOpacity(FSlateColor(FLinearColor::White))
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(2.0f)
		[
			SAssignNew(Canvas, SGameInGameCanvas)
		];

	if (bShowTouchButtons)
	{
		Root->AddSlot()
		.AutoHeight()
		.Padding(2.0f)
		[
			BuildTouchButtons()
		];
	}

	return Root;
}

TSharedRef<SWidget> SGameInGamePanel::BuildTouchButtons()
{
	auto MakeButton = [this](const FString& Label, TFunction<void()> Action) -> TSharedRef<SWidget>
	{
		return SNew(SButton)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked_Lambda([Action]()
			{
				Action();
				return FReply::Handled();
			})
			[
				SNew(SBox)
				.MinDesiredWidth(46.0f)
				.MinDesiredHeight(42.0f)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Label))
					.Justification(ETextJustify::Center)
					.Font(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), 22))
				]
			];
	};

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)[ MakeButton(FString::Chr(0x2191), [this]() { ForwardInput(EGameInGameInput::Up); }) ]
		]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)[ MakeButton(FString::Chr(0x2190), [this]() { ForwardInput(EGameInGameInput::Left); }) ]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)[ MakeButton(FString::Chr(0x25CF), [this]() { ForwardInput(EGameInGameInput::Action); }) ]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)[ MakeButton(FString::Chr(0x2192), [this]() { ForwardInput(EGameInGameInput::Right); }) ]
		]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)[ MakeButton(FString::Chr(0x2193), [this]() { ForwardInput(EGameInGameInput::Down); }) ]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)[ MakeButton(FString::Chr(0x25C6), [this]() { ForwardFlag(); }) ]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)[ MakeButton(FString::Chr(0x2261), [this]() { ForwardInput(EGameInGameInput::Back); }) ]
		];
}

IGameInGameMinigameInterface* SGameInGamePanel::GetActiveGameInterface() const
{
	if (UObject* Obj = ActiveGame.Get())
	{
		return Cast<IGameInGameMinigameInterface>(Obj);
	}
	return nullptr;
}

void SGameInGamePanel::StartGame(int32 Index)
{
	if (!Games.IsValidIndex(Index))
	{
		return;
	}

	UObject* Obj = Games[Index].Get();
	if (!Obj)
	{
		return;
	}

	SelectedIndex = Index;
	ActiveGame = Obj;
	bReportedGameOver = false;
	State = EPanelState::Playing;

	if (IGameInGameMinigameInterface* Game = Cast<IGameInGameMinigameInterface>(Obj))
	{
		Game->Init();
	}
	if (Canvas.IsValid())
	{
		Canvas->SetActiveGame(Obj);
	}

	FSlateApplication::Get().SetKeyboardFocus(AsShared());
}

void SGameInGamePanel::ReturnToMenu()
{
	State = EPanelState::Menu;
	ActiveGame.Reset();
	if (Canvas.IsValid())
	{
		Canvas->SetActiveGame(nullptr);
	}
	RefreshGames();
}

void SGameInGamePanel::CloseOverlay()
{
	OnClosed.ExecuteIfBound();
}

void SGameInGamePanel::MoveSelection(int32 Delta)
{
	if (Games.Num() == 0)
	{
		return;
	}
	SelectedIndex = (SelectedIndex + Delta + Games.Num()) % Games.Num();
}

void SGameInGamePanel::ForwardInput(EGameInGameInput Input)
{
	if (State == EPanelState::Menu)
	{
		switch (Input)
		{
		case EGameInGameInput::Up:     MoveSelection(-1); break;
		case EGameInGameInput::Down:   MoveSelection(1);  break;
		case EGameInGameInput::Action: StartGame(SelectedIndex); break;
		case EGameInGameInput::Back:   CloseOverlay(); break;
		default: break;
		}
		return;
	}

	// Playing
	if (Input == EGameInGameInput::Back)
	{
		ReturnToMenu();
		return;
	}
	if (IGameInGameMinigameInterface* Game = GetActiveGameInterface())
	{
		Game->HandleInput(Input);
	}
}

void SGameInGamePanel::ForwardFlag()
{
	if (State == EPanelState::Playing)
	{
		if (IGameInGameMinigameInterface* Game = GetActiveGameInterface())
		{
			Game->HandleSecondaryAction();
		}
	}
}

void SGameInGamePanel::ReportCompletion(IGameInGameMinigameInterface* Game)
{
	if (!Game)
	{
		return;
	}
	const FString Name = Game->GetGameName();
	const int32 FinalScore = Game->GetScore();
	bool bNewHigh = false;
	if (UGameInGameSubsystem* Sub = SubsystemWeak.Get())
	{
		bNewHigh = Sub->SubmitScore(Name, FinalScore);
	}
	OnCompleted.ExecuteIfBound(Name, FinalScore, bNewHigh);
}

void SGameInGamePanel::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (State != EPanelState::Playing)
	{
		return;
	}

	if (IGameInGameMinigameInterface* Game = GetActiveGameInterface())
	{
		Game->Tick(InDeltaTime);

		if (!bReportedGameOver && Game->IsGameOver())
		{
			bReportedGameOver = true;
			ReportCompletion(Game);
		}
	}

	if (Canvas.IsValid())
	{
		Canvas->Invalidate(EInvalidateWidgetReason::Paint);
	}
}

bool SGameInGamePanel::MapKey(const FKey& Key, EGameInGameInput& OutInput)
{
	if (Key == EKeys::Up || Key == EKeys::W || Key == EKeys::Gamepad_DPad_Up)          { OutInput = EGameInGameInput::Up;     return true; }
	if (Key == EKeys::Down || Key == EKeys::S || Key == EKeys::Gamepad_DPad_Down)       { OutInput = EGameInGameInput::Down;   return true; }
	if (Key == EKeys::Left || Key == EKeys::A || Key == EKeys::Gamepad_DPad_Left)       { OutInput = EGameInGameInput::Left;   return true; }
	if (Key == EKeys::Right || Key == EKeys::D || Key == EKeys::Gamepad_DPad_Right)     { OutInput = EGameInGameInput::Right;  return true; }
	if (Key == EKeys::SpaceBar || Key == EKeys::Enter || Key == EKeys::Gamepad_FaceButton_Bottom) { OutInput = EGameInGameInput::Action; return true; }
	if (Key == EKeys::Escape || Key == EKeys::BackSpace || Key == EKeys::Gamepad_FaceButton_Right) { OutInput = EGameInGameInput::Back;   return true; }
	return false;
}

FReply SGameInGamePanel::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	// Flag / secondary action.
	if (Key == EKeys::F || Key == EKeys::Gamepad_FaceButton_Left)
	{
		ForwardFlag();
		return FReply::Handled();
	}

	EGameInGameInput MappedInput;
	if (MapKey(Key, MappedInput))
	{
		ForwardInput(MappedInput);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SGameInGamePanel::OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent)
{
	const FKey Key = InAnalogInputEvent.GetKey();
	const float Value = InAnalogInputEvent.GetAnalogValue();
	const float FireThreshold = 0.6f;
	const float ReleaseThreshold = 0.3f;

	if (Key == EKeys::Gamepad_LeftX)
	{
		if (FMath::Abs(Value) < ReleaseThreshold)
		{
			bAnalogXActive = false;
		}
		else if (!bAnalogXActive && FMath::Abs(Value) >= FireThreshold)
		{
			bAnalogXActive = true;
			ForwardInput(Value > 0 ? EGameInGameInput::Right : EGameInGameInput::Left);
			return FReply::Handled();
		}
	}
	else if (Key == EKeys::Gamepad_LeftY)
	{
		if (FMath::Abs(Value) < ReleaseThreshold)
		{
			bAnalogYActive = false;
		}
		else if (!bAnalogYActive && FMath::Abs(Value) >= FireThreshold)
		{
			bAnalogYActive = true;
			// Gamepad Y is positive-up.
			ForwardInput(Value > 0 ? EGameInGameInput::Up : EGameInGameInput::Down);
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}
