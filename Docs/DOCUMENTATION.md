# Game In Game — Overlay Framework · Documentation

**Version:** 1.0.0 · **Engine:** Unreal Engine 5.8.0 · **Category:** Code Plugins
**Author:** Silvan Teufel — [Silvan Teufel](https://github.com/SimulatedFlow)
**Support:** [teufelsilvan@gmail.com](mailto:teufelsilvan@gmail.com) · **Docs:** https://github.com/SimulatedFlow

An independent Slate & UMG overlay containing **5 fully-playable classic arcade games**
(Block Stacker, Snake, Minesweeper, 2048, Breakout) with custom, material-driven styling and an
extensible C++ framework for adding your own minigames in a few lines of code.

The overlay ticks on real (Slate) time, so the games keep running under game **pause** and
**slow-mo**, and it captures keyboard, gamepad and touch input so gameplay input never
leaks to the host player character in the background.

---

## Table of Contents

1. [Features](#1-features)
2. [Supported Platforms & Engine](#2-supported-platforms--engine)
3. [Installation](#3-installation)
4. [Quick Start](#4-quick-start)
5. [Architecture Overview](#5-architecture-overview)
6. [API / Class Reference](#6-api--class-reference)
7. [Code Examples](#7-code-examples)
8. [The 5 Built-in Minigames](#8-the-5-built-in-minigames)
9. [High Scores & Persistence](#9-high-scores--persistence)
10. [Input Handling](#10-input-handling)
11. [Styling](#11-styling)
12. [Troubleshooting / FAQ](#12-troubleshooting--faq)

---

## 1. Features

- **5 complete arcade games** — Block Stacker, Snake, Minesweeper, 2048 and Breakout, all fully
  playable out of the box.
- **Pause-proof** — games tick on real Slate time, unaffected by `UGameplayStatics::SetGamePaused`
  or global time dilation.
- **Two integration paths** — drag-and-drop `UGameInGameWidget` for designers, or a pure-C++
  `SGameInGamePanel` / `AGameInGameDemoActor` path for programmers.
- **Material-driven styling** — border driven by any `UMaterialInterface`, plus tint, padding
  and corner-rounding controls.
- **Input isolation** — keyboard, gamepad and optional on-screen touch buttons are captured by
  the panel; the host player never receives the game's arrow/stick input.
- **Extensible framework** — implement `IGameInGameMinigameInterface` (or subclass
  `UGameInGameMinigameBase`) and register it with one call.
- **Persistent high scores** — per-game high scores saved via a `USaveGame` subclass.
- **Blueprint-friendly** — subsystem calls, events and widget properties are all exposed to Blueprint.

---

## 2. Supported Platforms & Engine

| Item | Value |
| --- | --- |
| Engine version | **Unreal Engine 5.8.0** |
| Module type | `Runtime` (`LoadingPhase: Default`) |
| Language | C++ (with full Blueprint exposure) |
| Content | `CanContainContent: true` |

**Target platforms:** The `PlatformAllowList` in the `.uplugin` names **`Win64`**, because that
is the platform this release was built and tested on. The plugin itself uses only cross-platform
modules (`Slate`, `SlateCore`, `UMG`, `InputCore`, `Engine`) with no platform-specific or
third-party native code, so adding macOS, Linux, iOS, Android or a console to that list and
rebuilding is expected to work — it has simply not been verified here.
Keyboard and gamepad are supported everywhere; touch buttons (`bShowTouchButtons`) are provided
for touch and mouse-only devices.

> Built and compile-checked against UE 5.8 in the Editor (`UnrealEditor Development`) configuration.

---

## 3. Installation

### Option A — Project plugin (recommended for source builds)

1. Copy the `GameInGame` folder into your project's `Plugins/` directory:
   `<YourProject>/Plugins/GameInGame/`.
2. Right-click your `.uproject` → **Generate Visual Studio project files**.
3. Open the solution and build, or just launch the project — the editor will offer to compile
   the plugin module on start.
4. In the editor: **Edit → Plugins → Code Plugins → Game In Game Overlay Framework** and make sure
   it is enabled. Restart if prompted.

### Option B — Engine plugin / Fab install

1. Install from Fab into the target engine version, or copy `GameInGame` into
   `<Engine>/Engine/Plugins/Marketplace/`.
2. Enable it via **Edit → Plugins** and restart the editor.

The plugin declares its own module dependencies (`Slate`, `SlateCore`, `UMG`, `InputCore`,
`Projects`), so no extra engine plugins need to be enabled manually.

---

## 4. Quick Start

### 4a. The fastest path — the demo actor

1. In the **Place Actors** panel search for **GameInGameDemoActor** and drag it into your level.
2. Leave **Show On Begin Play** ticked.
3. Press **Play** — the overlay appears, focused, with the game-selection menu.
4. Use arrow keys / D-pad to pick a game, **Enter / Face-Button-Bottom** to start, **Esc / B** to go back.

### 4b. The designer path — UMG widget

1. Create (or open) a **Widget Blueprint**.
2. In the palette find **Game In Game** (under the *GameInGame* category) and drag a
   **GameInGameWidget** onto the canvas.
3. Style it in the Details panel (**Border Material**, **Border Tint**, **Border Padding**,
   **Corner Style**, **Show Touch Buttons**).
4. Bind the **On Minigame Completed** and **On Overlay Closed** events as needed.
5. Add the widget to the viewport from your HUD / player controller as usual.

---

## 5. Architecture Overview

```
UGameInGameSubsystem (GameInstanceSubsystem)
   │  owns registry + high-score store
   ├── registers 5 built-in games (UGameInGameMinigameBase subclasses)
   └── RegisterCustomMinigame(...)  ← your games

Presentation
   ├── SGameInGamePanel (SCompoundWidget)   ← menu + viewport + focus/input capture
   │      └── SGameInGameCanvas (SLeafWidget) ← normalized draw surface games render into
   ├── UGameInGameWidget (UWidget)          ← UMG wrapper around SGameInGamePanel
   └── AGameInGameDemoActor (AActor)        ← pure-C++ "add to viewport" example

Game contract
   IGameInGameMinigameInterface  ← Init / Tick / HandleInput / Render / Reset / GetScore / GetGameName
   UGameInGameMinigameBase       ← convenience UObject base with defaults
   FGameInGameGrid               ← shared 2D tile-grid helper (Block Stacker / Snake / Minesweeper / 2048)

Persistence
   UGameInGameSaveGame (USaveGame) ← TMap<FString,int32> HighScores
```

The **panel drives the game purely through the interface** — it never reaches into the host
world — which is what keeps game logic isolated from the world's pause / slow-mo state.

---

## 6. API / Class Reference

### `EGameInGameInput` (UENUM, BlueprintType)
Logical inputs decoupled from physical keys/gamepad/touch:
`Up`, `Down`, `Left`, `Right`, `Action`, `Back`.

### `IGameInGameMinigameInterface`
The contract every minigame implements.

| Member | Description |
| --- | --- |
| `virtual void Init() = 0` | Allocate/reset all state so the game is ready to play. |
| `virtual void Tick(float DeltaTime) = 0` | Advance simulation. `DeltaTime` is real Slate time (ignores world pause/slow-mo). |
| `virtual void HandleInput(EGameInGameInput InputType) = 0` | React to one logical input event. |
| `virtual void Render(SGameInGameCanvas* Canvas) = 0` | Draw the current frame via the canvas helpers. |
| `virtual void Reset() = 0` | Restart from a clean state. |
| `virtual int32 GetScore() const = 0` | Current score. |
| `virtual FString GetGameName() const = 0` | Menu name + high-score key. |
| `virtual bool IsGameOver() const` | *(optional)* True once the game ended. Default: `false`. |
| `virtual void HandleSecondaryAction()` | *(optional)* Secondary action (e.g. Minesweeper flag toggle). |

### `UGameInGameMinigameBase` (UObject, Abstract)
Convenience base implementing the interface with sensible defaults. Provides protected
`int32 Score`, `bool bGameOver`, and `DrawGameOverBanner(SGameInGameCanvas*)`. Subclass it and
override only what you need.

### `UGameInGameSubsystem` (UGameInstanceSubsystem)
Owns the registry and the high-score store; created once per `GameInstance`; auto-registers the
5 built-in games.

| Method | Blueprint | Description |
| --- | --- | --- |
| `void RegisterCustomMinigame(TScriptInterface<IGameInGameMinigameInterface> NewGame)` | ✔ | Add a user game to the registry. |
| `TArray<TScriptInterface<IGameInGameMinigameInterface>> GetRegisteredMinigames() const` | ✔ | All registered games (built-in + custom). |
| `int32 GetHighScore(const FString& GameName) const` | ✔ | Best recorded score, or 0. |
| `bool SubmitScore(const FString& GameName, int32 Score)` | ✔ | Records a score; returns true + persists if it beats the previous best. |
| `void SaveHighScores()` | ✔ | Force a flush of the save game to disk. |

Access it via `UGameInstance::GetSubsystem<UGameInGameSubsystem>()`.

### `SGameInGameCanvas` (SLeafWidget)
Lightweight draw surface. All coordinates are **normalized `[0..1]`** (resolution-independent);
draw helpers are valid only while called from a game's `Render`.

| Helper | Description |
| --- | --- |
| `void FillRectN(float X, float Y, float W, float H, const FLinearColor& Color) const` | Fill an axis-aligned rectangle. |
| `void StrokeRectN(float X, float Y, float W, float H, const FLinearColor& Color, float ThicknessPx = 1.0f) const` | Rectangle outline. |
| `void DrawGridCell(int32 GX, int32 GY, int32 Cols, int32 Rows, const FLinearColor& Color, float InsetFrac = 0.06f) const` | Draw one cell of a `Cols x Rows` grid. |
| `void DrawTextN(const FString& Text, float X, float Y, const FLinearColor& Color, int32 FontSize = 12) const` | Draw text at a normalized top-left anchor. |
| `FVector2D GetCanvasLocalSize() const` | Canvas size in local (pre-DPI) Slate units. |
| `void SetActiveGame(UObject* InGameObject)` | Set the object (implementing the interface) that will be painted. |

### `SGameInGamePanel` (SCompoundWidget)
The overlay: a material-driven bordered frame hosting the game-selection menu and the active-game
viewport. Captures keyboard/gamepad focus (`SupportsKeyboardFocus() == true`) and ticks the active
game on real Slate time.

Construction arguments (`SLATE_ARGUMENT` / `SLATE_EVENT`):
`BorderMaterial`, `BorderPadding` (default `FMargin(12)`), `BorderTint` (default white),
`CornerStyle` (default `0`), `ShowTouchButtons` (default `false`), `Subsystem`
(`TWeakObjectPtr<UGameInGameSubsystem>`), and events `OnMinigameCompleted(GameName, Score, bIsNewHighScore)`
and `OnOverlayClosed()`.
Runtime: `void UpdateStyle(UMaterialInterface*, const FMargin&, const FLinearColor&, float)`.

### `UGameInGameWidget` (UWidget)
Designer-facing UMG wrapper. Drop it into any UserWidget.

| Property (Category) | Type | Notes |
| --- | --- | --- |
| `BorderMaterial` (Styling) | `UMaterialInterface*` | Border background material. |
| `BorderPadding` (Styling) | `FMargin` | Inner padding of the border. |
| `BorderTint` (Styling) | `FLinearColor` | Tint applied to the border. |
| `CornerStyle` (Styling) | `float` | Corner radius in px when no material is set (ClampMin 0). |
| `bShowTouchButtons` (Input) | `bool` | Show on-screen touch buttons. |

Events (`BlueprintAssignable`, Category `GameInGame|Events`):
- `FGameInGameOnMinigameCompleted OnMinigameCompleted(const FString& GameName, int32 Score, bool bIsNewHighScore)`
- `FGameInGameOnOverlayClosed OnOverlayClosed()`

### `AGameInGameDemoActor` (AActor, Blueprintable)
Drop-in example that adds the overlay straight to the viewport — no UserWidget asset needed.
Properties: `bShowOnBeginPlay`, `BorderMaterial`, `BorderTint`, `CornerStyle`, `bShowTouchButtons`.
Methods (BlueprintCallable): `void ShowOverlay()`, `void HideOverlay()`.

### `UGameInGameSaveGame` (USaveGame)
`TMap<FString, int32> HighScores` — per-game high scores keyed by GameName.

### `FGameInGameGrid`
Shared 2D tile-grid helper wrapping cell states, color tints and grid→normalized layout
conversions, used by Block Stacker, Snake, Minesweeper and 2048.

---

## 7. Code Examples

### 7.1 Show the overlay from C++ (viewport)

```cpp
// e.g. in your PlayerController
#include "GameInGameDemoActor.h"

void AMyPlayerController::OpenArcade()
{
    FActorSpawnParameters Params;
    AGameInGameDemoActor* Arcade = GetWorld()->SpawnActor<AGameInGameDemoActor>(Params);
    Arcade->bShowOnBeginPlay = false;
    Arcade->BorderTint = FLinearColor(0.05f, 0.05f, 0.1f, 0.95f);
    Arcade->CornerStyle = 8.0f;
    Arcade->ShowOverlay(); // adds the panel to the viewport and focuses it
}
```

### 7.2 Read and submit high scores via the subsystem

```cpp
#include "GameInGameSubsystem.h"

if (UGameInstance* GI = GetGameInstance())
{
    if (UGameInGameSubsystem* Sub = GI->GetSubsystem<UGameInGameSubsystem>())
    {
        const int32 Best = Sub->GetHighScore(TEXT("Snake"));

        // Later, when a run ends:
        const bool bNewBest = Sub->SubmitScore(TEXT("Snake"), 1200); // auto-persists if it beats Best
    }
}
```

### 7.3 Add a custom minigame

```cpp
#include "Games/GameInGameMinigameBase.h"
#include "Slate/SGameInGameCanvas.h"

UCLASS()
class UMyPong : public UGameInGameMinigameBase
{
    GENERATED_BODY()
public:
    virtual FString GetGameName() const override { return TEXT("Pong"); }

    virtual void Tick(float Dt) override
    {
        BallX += VelX * Dt;
        BallY += VelY * Dt;
        if (BallY < 0.f || BallY > 1.f) VelY = -VelY;
    }

    virtual void HandleInput(EGameInGameInput In) override
    {
        if (In == EGameInGameInput::Up)   PaddleY -= 0.05f;
        if (In == EGameInGameInput::Down) PaddleY += 0.05f;
    }

    virtual void Render(SGameInGameCanvas* C) override
    {
        C->FillRectN(0.f, 0.f, 1.f, 1.f, FLinearColor::Black);
        C->FillRectN(0.02f, PaddleY, 0.02f, 0.2f, FLinearColor::White);
        C->FillRectN(BallX, BallY, 0.02f, 0.02f, FLinearColor::White);
    }

private:
    float BallX = 0.5f, BallY = 0.5f, VelX = 0.4f, VelY = 0.3f, PaddleY = 0.4f;
};

// Register it (e.g. from your GameInstance init):
UGameInGameSubsystem* Sub = GetGameInstance()->GetSubsystem<UGameInGameSubsystem>();
Sub->RegisterCustomMinigame(NewObject<UMyPong>(Sub));
```

### 7.4 Blueprint

- **Register a custom game:** call **Register Custom Minigame** on the subsystem
  (`Get Game Instance → Get Subsystem (GameInGameSubsystem)`).
- **React to results:** bind **On Minigame Completed** (`GameName`, `Score`, `bIsNewHighScore`)
  and **On Overlay Closed** on the `GameInGameWidget`.
- **Query scores:** call **Get High Score** / **Submit Score** on the subsystem.

---

## 8. The 5 Built-in Minigames

| Game | Highlights |
| --- | --- |
| **Block Stacker** | 10×20 grid, 7 tetrominoes (I, J, L, O, S, T, Z), rotation with wall-kicks, hard drop, full line clearing, fall speed scaling with score. |
| **Snake** | Direction queue (prevents self-collision on rapid inputs), safe food spawning, growth, self- and wall-collision fail states. |
| **Minesweeper** | 8×8 … 12×12 grids, first-click safety (field generated on first click so the first cell is never a mine), flag placement, flood-fill reveal, mine detonation. |
| **2048** | 4×4 sliding board, merge-on-equal, score updates, new-tile spawn (90% "2" / 10% "4"), game-over detection when no legal move remains. |
| **Breakout** | Float physics tick, movable paddle, custom reflection vectors, 3-tier destructible bricks, ball-speed escalation, 3 lives. |

---

## 9. High Scores & Persistence

- High scores live in a `UGameInGameSaveGame` (a `USaveGame` subclass) as a
  `TMap<FString, int32>` keyed by GameName.
- `SubmitScore(GameName, Score)` updates and **persists** the record when it beats the previous
  best and returns `true`; otherwise returns `false`.
- `GetHighScore(GameName)` returns the best recorded score, or `0` if none.
- `SaveHighScores()` forces a flush to disk. The subsystem loads existing scores on
  `Initialize`.

---

## 10. Input Handling

- The panel captures **keyboard focus** (`SupportsKeyboardFocus`) and handles both `OnKeyDown`
  and `OnAnalogValueChanged`, mapping physical input to logical `EGameInGameInput` values.
- Analog sticks use edge detection so a held stick produces discrete moves for grid games.
- Because input is captured by the panel, arrow keys / stick movement **do not** reach the host
  player character while the overlay is focused.
- Set `bShowTouchButtons = true` to display on-screen buttons for touch and mouse-only players.
- `HandleSecondaryAction()` powers contextual actions such as toggling a Minesweeper flag.

---

## 11. Styling

All styling is available on `UGameInGameWidget`, `AGameInGameDemoActor` and the `SGameInGamePanel`
construction args:

- **BorderMaterial** — any `UMaterialInterface`; drives the outer border background. When unset,
  a solid tinted brush is used.
- **BorderTint** (`FLinearColor`) — tint applied to the border/brush.
- **BorderPadding** (`FMargin`) — inner padding around the play area (panel default `12`).
- **CornerStyle** (`float`) — corner radius (px) applied when no material is set.
- **ShowTouchButtons** (`bool`) — on-screen touch controls.

Call `SGameInGamePanel::UpdateStyle(...)` to re-apply styling at runtime without a full rebuild;
the UMG widget does this automatically in `SynchronizeProperties`.

---

## 12. Troubleshooting / FAQ

**The overlay doesn't take input.**
Make sure it has focus. The demo actor's `ShowOverlay()` focuses automatically; for a UMG widget,
ensure input mode allows UI (e.g. `SetInputModeGameAndUI` / `UI Only`) and that the widget is focused.

**Games freeze when I pause the game.**
They shouldn't — games tick on real Slate time. If you subclassed a game and drive it from an actor
tick instead of the interface `Tick`, move the logic into `Tick(float DeltaTime)` so it stays
pause-independent.

**My custom game doesn't appear in the menu.**
Register it *after* the subsystem exists (the GameInstance is created), e.g. from
`UGameInstance::Init` or later, via `RegisterCustomMinigame`. Confirm `GetGameName()` returns a
non-empty string.

**High scores aren't saved.**
`SubmitScore` only persists when the score beats the previous best. Call `SaveHighScores()` to force
a flush, and verify the GameInstance (and thus the subsystem) is alive when you submit.

**Which module do I depend on?**
Add `"GameInGame"` to your module's `PublicDependencyModuleNames` (or `PrivateDependencyModuleNames`)
to use the classes from your own C++.

---

*© 2026 Silvan Teufel. All Rights Reserved.*
