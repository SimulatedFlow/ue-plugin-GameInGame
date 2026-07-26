# GameInGame — Fab Resubmit Notes

*Prepared 2026-07-26 · GameInGame v1.0.0 · UE 5.8*

## Why this file exists

A previous Fab submission of **GameInGame** was rejected. The fix for resubmission is the
answer to Fab's third-party-software question. This note records the exact portal answer to
use and the evidence behind it, so the resubmit is fast and correct.

## Exact Fab portal answer

> **"Does this product contain third-party software?" → No**

Select **No**. Do **not** list any third-party libraries, SDKs, or middleware — the plugin
ships none.

## Evidence (verified 2026-07-26)

The plugin was scanned end-to-end; it genuinely contains **no third-party software**:

- **No `ThirdParty` / `External` / `Vendor` / `libs` folders** anywhere in the plugin tree.
- **No bundled binaries in `Source/`** — no `.lib`, `.dll`, `.so`, `.a`, or `.dylib` files.
- **`Source/GameInGame/GameInGame.Build.cs`** references only first-party Epic engine modules:
  `Core, CoreUObject, Engine, Slate, SlateCore, UMG, InputCore, Projects`.
  No `PublicAdditionalLibraries`, `PublicDelayLoadDLLs`, `RuntimeDependencies`, or
  `ThirdParty` include paths.
- **`GameInGame.uplugin`** declares **no** `Plugins` dependencies (no other Marketplace/Fab
  plugins required).
- **No dynamic-library loading in code** — no `LoadLibrary`, `dlopen`, `FPlatformProcess::GetDllHandle`,
  or `#pragma comment(lib, ...)` in any `.cpp`/`.h`.
- All five bundled minigames (Snake, Breakout, Tetris, 2048, Minesweeper) are **original C++
  implementations** on Slate/UMG — no imported game engines or licensed code.

This matches the plugin's own compliance sheet
(`_PluginIdeas/2026-07-17-gameingame/FAB_COMPLIANCE.md`, points 5 & 7:
"Nur Engine-Module … keine Fremd-Plugins" / "Kein Drittcode, keine externen Bibliotheken").

## Related human check before you click Submit (NOT a third-party issue)

- **Trademark:** "Tetris" is a registered trademark of The Tetris Company. The generic
  falling-block *mechanic* is free, but the **name "Tetris"** should not appear as a product
  name in the store text or in the in-overlay game-select UI. Consider renaming that entry to a
  generic label (e.g. "Block Stacker") before resubmitting. ("2048", "Snake", "Breakout",
  "Minesweeper" are generic terms and are normally fine.)

## What still needs a human (portal-only, cannot be done locally)

1. In the Fab publisher portal, set **"Does this product contain third-party software?" = No**.
2. (Optional but recommended) rename the "Tetris" entry to a generic name in the store text/UI.
3. Click **Submit** for review.
