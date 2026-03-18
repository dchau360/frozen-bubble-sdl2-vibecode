# Frozen Bubble 2 SDL2 Port - Task List

## Completed Features

### Foundation
- [x] Project structure and CMake build system
- [x] SDL2 game loop with 60fps frame limiter
- [x] SDL2 rendering pipeline (renderer, logical size 640x480)
- [x] Window icon, fullscreen toggle (F12)
- [x] TTF text rendering (TTFText wrapper)
- [x] Asset loading with error handling and DATA_DIR verification
- [x] Logger (per-player log files: creator/joiner1-4)
- [x] Game settings (settings.ini persistence via GameSettings)
- [x] Audio mixer (music + SFX via SDL_mixer)

### Game Mechanics
- [x] Bubble physics (trajectory, wall reflection, collision detection)
- [x] Bubble placement (GetClosestFreeCell with BFS for occupied cells)
- [x] Bubble color matching and removal (flood fill)
- [x] Chain reactions (falling bubbles after matches)
- [x] Compressor/ceiling descent system
- [x] Danger zone detection and game over
- [x] Level loading from file (100 levels)
- [x] Random level generation
- [x] Score system (per-player)
- [x] Shooter angle control (keyboard)
- [x] Penguin and shooter animations

### Single Player
- [x] Level progression (curLevel tracking, LoadLevel)
- [x] Level completion detection (all bubbles cleared)
- [x] SubmitScore (saves level grid + highscore entry)
- [x] Highscore screen shown at end of levelset
- [x] Pick start level (number input panel, starts from chosen level)
- [x] Multiplayer training mode (2-min timer, random malus injection, score accumulation)
- [x] curLevel reset to startLevel on NewGame (was never reset before)

### Multiplayer Layout
- [x] 2-player local (side-by-side full grids)
- [x] 2-player network
- [x] 3-player network (center full + 2 mini corners)
- [x] 4-player network (center full + 3 mini corners)
- [x] 5-player network (center full + 4 mini corners)
- [x] curLaunchRct/nextBubbleRct/onTopRct/frozenBottomRct set for all player slots
- [x] ReloadGame resets bubbleOffset, turnsToCompress, curLaunchRct for all player counts

### Network / Multiplayer
- [x] TCP client (NetworkClient)
- [x] Server (separate process, handles lobby and game room)
- [x] Protocol: fire ('f'), stick ('s'), malus ('g'), malus bubble ('m'/'M'), win ('F'), death ('l'), new game ('n')
- [x] Malus/attack system (split mode and single-target mode)
- [x] Single-player targeting (keys 1-4 to target opponents, 0 to clear)
- [x] Attack/attackme sprite overlays (imgAttack, imgAttackMe)
- [x] 'A' network message for targeting sync
- [x] Player death handling (death propagation, sendMalusToOne cleanup)

### UI / Menus
- [x] Main menu (title screen, navigation)
- [x] Single player menu
- [x] Multiplayer menu (LAN/Net game, local 2P)
- [x] Pause (PAUSE key)
- [x] Game state transitions (TitleScreen <-> MainGame <-> Highscores)

### Highscore System
- [x] Top-10 levelset highscores (ranked by level desc, time asc)
- [x] Level grid thumbnails (small rendered images of best levels reached)
- [x] Name entry panel (text input with cursor blink)
- [x] Save/load highscores from prefPath
- [x] AppendToLevels / CheckAndAddScore
- [x] pendingHighscore flag (show score screen after completing all levels)

## Known Remaining Warnings (non-critical)
- [ ] `-Wwritable-strings` from `LoadPenguin` calls with string literal arguments (pre-existing, would require changing function signatures)

## Not Implemented (by design)
- Geolocation spots on server list (requires STATUSGEO server support not in C++ port)
- Level editor
- Automated tests
