# Copyright 2026 Simulated Flow All Rights Reserved.
#
# GameInGame - Demo-Content-Generator (UE 5.8, Editor-Python)
# ------------------------------------------------------------
# Baut eine anschauliche Demo-Szene fuer das Plugin "GameInGame" und legt sie
# im Plugin-Content ab. Kapselt genau die Schritte, die sonst ueber die
# Unreal-MCP-Tools (spawn_actor / create_blueprint / take_screenshot) laufen
# wuerden - so ist die Demo reproduzierbar, auch ohne laufende MCP-Verbindung.
#
# Ausfuehren (eine der beiden Varianten):
#   A) Im laufenden Editor: Output Log -> Cmd auf "Python" stellen ->
#        py "F:/Unreal Projects/ExamplePluginProject/Plugins/GameInGame/Scripts/generate_demo.py"
#   B) Headless (NUR wenn keine andere Editor-Instanz das Projekt offen haelt):
#        "C:/Program Files/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
#          "F:/Unreal Projects/ExamplePluginProject/ExamplePluginProject.uproject" ^
#          -run=pythonscript ^
#          -script="F:/Unreal Projects/ExamplePluginProject/Plugins/GameInGame/Scripts/generate_demo.py" ^
#          -unattended -nop4 -nosplash
#
# Erzeugt:
#   /GameInGame/Demo/L_GameInGameDemo   (Level mit Boden, Licht, PlayerStart, DemoActor)
#   /GameInGame/Demo/BP_GameInGameDemo  (Blueprint-Subclass von AGameInGameDemoActor)
# und legt einen HighResShot unter .../_PluginIdeas/2026-07-17-gameingame/marketing ab.

import os
import unreal

# ---------------------------------------------------------------------------
# Konfiguration
# ---------------------------------------------------------------------------
DEMO_DIR      = "/GameInGame/Demo"
LEVEL_PATH    = DEMO_DIR + "/L_GameInGameDemo"
BP_PATH       = DEMO_DIR + "/BP_GameInGameDemo"
DEMO_ACTOR    = "/Script/GameInGame.GameInGameDemoActor"
MARKETING_DIR = ("F:/Unreal Projects/ExamplePluginProject/_PluginIdeas/"
                 "2026-07-17-gameingame/marketing")

log = unreal.log
warn = unreal.log_warning


def _sub(cls):
    return unreal.get_editor_subsystem(cls)


# ---------------------------------------------------------------------------
# 1) Demo-Level anlegen
# ---------------------------------------------------------------------------
def create_level():
    les = _sub(unreal.LevelEditorSubsystem)
    log("[GameInGame] Erzeuge Demo-Level %s" % LEVEL_PATH)
    les.new_level(LEVEL_PATH)          # leeres Level erzeugen und laden
    return les


# ---------------------------------------------------------------------------
# 2) Szene bestuecken (Boden, Licht, Himmel, PlayerStart, DemoActor)
# ---------------------------------------------------------------------------
def populate_scene():
    eas = _sub(unreal.EditorActorSubsystem)

    def spawn(cls, loc, rot=None, label=None):
        rot = rot or unreal.Rotator(0, 0, 0)
        actor = eas.spawn_actor_from_class(cls, unreal.Vector(*loc), rot)
        if actor and label:
            actor.set_actor_label(label)
        return actor

    # Boden: skalierte Engine-Plane
    floor = spawn(unreal.StaticMeshActor, (0, 0, 0), label="DemoFloor")
    if floor:
        plane = unreal.load_object(None, "/Engine/BasicShapes/Plane.Plane")
        comp = floor.static_mesh_component
        comp.set_static_mesh(plane)
        floor.set_actor_scale3d(unreal.Vector(20.0, 20.0, 1.0))

    # Beleuchtung + Himmel
    dir_light = spawn(unreal.DirectionalLight, (0, 0, 600),
                      unreal.Rotator(-45, 45, 0), label="DemoSun")
    if dir_light:
        dir_light.light_component.set_intensity(6.0)
    spawn(unreal.SkyLight, (0, 0, 400), label="DemoSkyLight")
    spawn(unreal.SkyAtmosphere, (0, 0, 0), label="DemoSkyAtmosphere")
    spawn(unreal.ExponentialHeightFog, (0, 0, 0), label="DemoFog")

    # Spielerstart, damit PIE sofort einen Pawn hat
    spawn(unreal.PlayerStart, (0, -300, 120),
          unreal.Rotator(0, 90, 0), label="DemoPlayerStart")

    # Der eigentliche Plugin-DemoActor: blendet beim Play das Minigame-Overlay ein
    demo_cls = unreal.load_class(None, DEMO_ACTOR)
    if demo_cls:
        spawn(demo_cls, (0, 0, 90), label="GameInGameDemoActor")
    else:
        warn("[GameInGame] Klasse %s nicht ladbar - Modul geladen?" % DEMO_ACTOR)


# ---------------------------------------------------------------------------
# 3) Blueprint-Subclass des DemoActors (designer-freundliche Variante)
# ---------------------------------------------------------------------------
def create_blueprint():
    if unreal.EditorAssetLibrary.does_asset_exist(BP_PATH):
        log("[GameInGame] Blueprint existiert bereits: %s" % BP_PATH)
        return
    demo_cls = unreal.load_class(None, DEMO_ACTOR)
    if not demo_cls:
        warn("[GameInGame] Kann BP nicht erstellen - Basisklasse fehlt.")
        return
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", demo_cls)
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    bp = tools.create_asset("BP_GameInGameDemo", DEMO_DIR,
                            unreal.Blueprint, factory)
    if bp:
        unreal.EditorAssetLibrary.save_loaded_asset(bp)
        log("[GameInGame] Blueprint erstellt: %s" % BP_PATH)


# ---------------------------------------------------------------------------
# 4) Alles speichern
# ---------------------------------------------------------------------------
def save_all(les):
    les.save_current_level()
    unreal.EditorAssetLibrary.save_directory(DEMO_DIR, only_if_is_dirty=False,
                                             recursive=True)
    log("[GameInGame] Demo-Content gespeichert unter %s" % DEMO_DIR)


# ---------------------------------------------------------------------------
# 5) Screenshot (HighResShot). Schreibt nach Saved/Screenshots und kopiert
#    anschliessend die neueste PNG in den marketing-Ordner.
# ---------------------------------------------------------------------------
def take_screenshot():
    world = unreal.EditorLevelLibrary.get_editor_world() \
        if hasattr(unreal, "EditorLevelLibrary") else None
    if world is None:
        les = _sub(unreal.UnrealEditorSubsystem)
        world = les.get_editor_world()
    # Editor-Viewport in HD abziehen (2x Supersampling)
    unreal.SystemLibrary.execute_console_command(world, "HighResShot 1920x1080")
    log("[GameInGame] HighResShot ausgeloest. Ziel-Standardordner: "
        "<Project>/Saved/Screenshots/WindowsEditor/")
    log("[GameInGame] Marketing-Zielordner: %s" % MARKETING_DIR)
    # Hinweis: HighResShot schreibt asynchron im naechsten Frame; das Kopieren
    # in den marketing-Ordner erfolgt durch den Pipeline-Sammelschritt bzw.
    # manuell aus Saved/Screenshots/WindowsEditor/.


# ---------------------------------------------------------------------------
def main():
    log("=== GameInGame Demo-Generator gestartet ===")
    les = create_level()
    populate_scene()
    create_blueprint()
    save_all(les)
    take_screenshot()
    log("=== GameInGame Demo-Generator fertig ===")


if __name__ == "__main__":
    main()
