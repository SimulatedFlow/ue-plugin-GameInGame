# Game In Game — Overlay Framework · Fab Store Listing

## Headline

**Game In Game — 5 Playable Arcade Minigames as a Pause-Proof, Material-Styled Overlay + an Extensible C++ Framework**

## Pitch (1 paragraph)

Give your players something to *do* — during loading screens, in-world arcade cabinets, downtime
menus, or just as an Easter egg. **Game In Game** drops a polished, material-styled overlay into
any Unreal Engine 5.8 project containing five fully-playable classic arcade games — **Tetris,
Snake, Minesweeper, 2048 and Breakout** — that keep running even while your main game is paused or
slowed down. It's not a Blueprint hack that stutters the game thread: it's a lightweight,
independently-ticking Slate overlay with a clean C++ interface, drag-and-drop UMG integration,
persistent high scores, and an open framework so you can add your own minigames in just a few
lines of code. Style it with your own material in seconds, ship it on desktop, mobile or console.

## Feature Bullets

- 🎮 **5 complete games out of the box** — Tetris (rotation, wall-kicks, hard drop, line clears),
  Snake (input-queue, safe food spawning), Minesweeper (8×8–12×12, first-click safety, flood-fill),
  2048 (merge logic, game-over detection) and Breakout (float physics, 3-tier bricks, 3 lives).
- ⏸️ **Pause-proof by design** — games tick on real Slate time, so they run smoothly under game
  pause, slow-mo, and any time-dilation.
- 🧩 **Drag-and-drop for designers** — a `GameInGameWidget` you drop into any UMG UserWidget, plus
  a `GameInGameDemoActor` for a zero-widget, straight-to-viewport setup.
- 🎨 **Material-driven styling** — border material, tint, padding and corner rounding, all editable
  in the palette; matches any UI style instantly.
- 🕹️ **Isolated input** — keyboard, gamepad and optional on-screen touch buttons are captured by
  the panel, so game input never leaks to the host player character.
- 🏆 **Persistent high scores** — per-game best scores saved automatically via a `USaveGame`.
- 🛠️ **Extensible C++ framework** — implement one interface (or subclass a provided base) and
  register your game with a single call.
- 🔵 **Full Blueprint exposure** — subsystem calls, completion/close events, and all styling
  properties are Blueprint-accessible.
- 📦 **Clean, dependency-free code** — no third-party libraries, no hard-coded paths, Epic coding
  standard, source included.

## Technical Details

| | |
| --- | --- |
| **Engine version** | Unreal Engine 5.8.0 |
| **Type** | Code Plugin (C++, source included) |
| **Module** | `GameInGame` — Runtime, LoadingPhase Default |
| **Dependencies** | Core, CoreUObject, Engine, Slate, SlateCore, UMG, InputCore, Projects (engine-only; no third-party) |
| **Network replicated** | No (client-side overlay) |
| **Blueprint support** | Yes (subsystem, widget, events, styling) |
| **Supported platforms** | Windows, macOS, Linux, iOS, Android, consoles (cross-platform modules only) |
| **Number of C++ classes** | Subsystem, SaveGame, UMG widget, demo actor, 2 Slate widgets, minigame interface + base, grid helper, 5 games |
| **Content** | Can contain content (demo actor / example widget) |

## Target Audience

- **Game developers** who want ready-made minigames for loading screens, arcade cabinets, in-game
  consoles, mobile downtime, or hidden Easter eggs — without building each game from scratch.
- **UMG/UI designers** who want a drag-and-drop, fully-stylable widget with no C++ required.
- **C++ programmers** who want a clean, extensible framework to author and register their own
  minigames on top of a tested Slate/UMG rendering and input layer.
- Suited to indie and studio teams across desktop, mobile and console projects.

## Price Idea

- **Suggested price: €49** (self-serve, well under the Fab $1,500 cap).
- Positioned as a mid-tier framework: more than a single-asset widget, less than a full game
  template. Optional intro/launch discount (e.g. €39) to build early reviews.

---

*Author: Silvan Teufel — Teufel-Engineering.com · Support: info@teufel-engineering.com*
