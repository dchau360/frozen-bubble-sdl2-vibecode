# Frozen Bubble SDL2 Port - Module Verification Plan

This plan breaks the codebase into independent modules for systematic verification against the original Frozen Bubble Perl code. Each module should be handled by a separate Claude instance for focused, line-by-line comparison.

## How to Use This Plan

When starting a new Claude instance:
1. Read this MODULE_PLAN.md file
2. Ask the user: "Which module should I work on?"
3. Follow the verification steps for that module
4. Document findings and fixes
5. Mark module as complete when done

---

## Module Overview

| Module | Original Code | C++ Port | Lines | Status |
|--------|---------------|----------|-------|--------|
| 1. Core Game Loop | frozen-bubble: 1800-2500 | bubblegame.cpp: 2300-2700 | ~700 | Not Started |
| 2. Bubble Physics | frozen-bubble: 590-813, 2100-2250 | bubblegame.cpp: 17-196, 1381-1550 | ~500 | **Completed** |
| 3. Chain Reactions | frozen-bubble: 814-865 | bubblegame.cpp: 1609-1749 | ~140 | **In Progress** |
| 4. Malus System | frozen-bubble: 956-1050 | bubblegame.cpp: 1950-2150 | ~200 | Not Started |
| 5. Network Protocol | frozen-bubble: 2400-2700 | networkclient.cpp: 1-800 | ~600 | Not Started |
| 6. Network Messages | frozen-bubble: 1100-1500 | bubblegame.cpp: 2750-3200 | ~850 | Not Started |
| 7. Win/Loss Logic | frozen-bubble: 1933-1950 | bubblegame.cpp: 2200-2280 | ~80 | **Completed** |
| 8. Round Transitions | frozen-bubble: 2321-2500 | bubblegame.cpp: 2399-2900 | ~550 | **Completed** |
| 9. UI/Menus | frozen-bubble: 3000-4500 | mainmenu.cpp: 1-1800 | ~1500 | Not Started |
| 10. Player Layout | Stuff.pm: 65-135 | bubblegame.cpp: 400-600 | ~200 | Not Started |

---

## Module 1: Core Game Loop & State Management

**Objective**: Verify main game loop, state transitions, player state tracking

**Original Code**: `frozen-bubble` lines 1800-2500
**C++ Port**: `bubblegame.cpp` lines 2300-2700

### Key Functions to Compare:
- Original: `iter_players()` (line 2105+)
- C++ Port: `Render()` main loop

### Verification Checklist:
- [ ] Player state updates (ALIVE, LOST, LEFT)
- [ ] Living player count tracking
- [ ] State transition logic (game → finished)
- [ ] Frame-by-frame update sequence
- [ ] Input processing order

### Known Issues:
- None currently

### Testing:
```bash
# Start 3-player game, verify all players update correctly
./build/frozen-bubble-sdl2
```

---

## Module 2: Bubble Physics & Collision

**Objective**: Verify bubble shooting, collision detection, sticking logic

**Original Code**: `frozen-bubble` lines 590-813, 2100-2250
**C++ Port**: `bubblegame.cpp` lines 17-196 (SingleBubble), 1381-1550 (UpdateSingleBubbles)

### Key Functions to Compare:
- Original: Bubble movement (line 2174-2175)
- Original: Wall bounce (line 2176-2185)
- Original: `get_array_closest_pos()` (line 636-641)
- Original: `real_stick_bubble()` (line 731)
- C++ Port: `SingleBubble::UpdatePosition()`
- C++ Port: `GetClosestFreeCell()`
- C++ Port: `UpdateSingleBubbles()`

### Verification Checklist:
- [x] Bubble trajectory calculation
- [x] Speed/acceleration values
- [x] Collision detection with grid
- [x] Sticking position calculation (even/odd row offsets)
- [x] Angle calculations
- [x] Wall bounce logic

### Bugs Fixed:
1. **GetClosestFreeCell() - Position Calculation** (bubblegame.cpp:1383-1411)
   - Original uses: `get_array_closest_pos()` with midpoint calculation (frozen-bubble line 636-641, 2208-2209)
   - Old C++ used: Distance search through all empty cells
   - New C++ uses: Direct grid position calculation matching original algorithm

2. **GetClosestFreeCell() - Occupancy Checking** (bubblegame.cpp:1413-1464) **CRITICAL FIX 2026-01-11**
   - **Bug**: Calculated position was not checked for occupancy → bubbles overwrote instead of stacking
   - **Root Cause**: Data structure mismatch - Original Perl uses list (multiple bubbles can have same cx/cy), C++ uses 2D array (one bubble per cell)
   - **Fix**: Added BFS to find nearest free adjacent cell if calculated position is occupied
   - **Lesson**: Must trace COMPLETE code path, not just individual function algorithms (see CLAUDE.md)

### Intentional Deviations from Original:
**IMPORTANT:** Speed constants differ from original Perl values, but this is INTENTIONAL:

- **BUBBLE_SPEED**: Original = 10, C++ Port = 5 (10/2) ✓ Keep as-is
- **LAUNCHER_SPEED**: Original = 0.03, C++ Port = 0.009 (0.015*0.6) ✓ Keep as-is
- **MALUS_BUBBLE_SPEED**: Original = 30, C++ Port = 15 (30/2) ✓ Keep as-is

**Reason:** The C++ port runs at 60 FPS while the original runs at 50 FPS (TARGET_ANIM_SPEED = 20ms).
Using the original speed values at 60 FPS makes movement and shooting too fast.
The C++ port's adjusted values produce the correct gameplay feel matching the original.

**Testing confirmed:** Original values (10, 0.03, 30) at 60 FPS = TOO FAST ❌
**Testing confirmed:** Adjusted values (5, 0.009, 15) at 60 FPS = Correct feel ✓

### Status:
**✅ COMPLETED** - Verified and tested 2026-01-11
- ✓ GetClosestFreeCell() rewritten to match original algorithm
- ✓ Speed constants verified as intentionally different (documented above)
- ✓ Gameplay feel confirmed to match original
- ✓ User testing completed and confirmed

### Testing:
```bash
# Build completed successfully
cd /Users/dericchau/ai/fb2-port/frozen-bubble-sdl2/build
cmake --build .

# Testing completed:
./frozen-bubble-sdl2
# ✓ Bubble speed feels correct (matches original feel)
# ✓ Launcher rotation speed correct
# ✓ Wall bounces working
# ✓ Sticking positions use proper grid calculation
```

---

## Module 3: Chain Reactions

**Objective**: Verify chain reaction assignment and cascading

**Original Code**: `frozen-bubble` lines 814-865
**C++ Port**: `bubblegame.cpp` lines 1609-1749

### Key Functions to Compare:
- Original: Chain reaction block in `real_stick_bubble()` (line 814-865)
- C++ Port: `AssignChainReactions()`

### Verification Checklist:
- [x] Chain target finding (free position next to group)
- [x] Prevent multiple chains to same group (`chained_bubbles` tracking)
- [x] Chain bubble movement to destination
- [x] Chain bubble exploding target group
- [x] Sorting by `distance_to_root` (line 828) - **FIXED 2026-01-11**
- [ ] Cascading chain reactions (chain → explode → more chains) - Needs testing

### Bugs Fixed:
1. **Missing distance_to_root calculation** (bubblegame.cpp:1651-1703)
   - Original calculates distance from root bubbles (frozen-bubble lines 801-810)
   - C++ port had no distance tracking
   - Added BFS from top row to calculate distances for all bubbles

2. **Missing distance_to_root sorting** (bubblegame.cpp:1746-1751)
   - Original sorts groups by distance before assigning chains (frozen-bubble line 828)
   - C++ port processed groups in random grid order
   - Added sorting to prioritize groups closer to the root

3. **Wrong loop order** (bubblegame.cpp:1757-1862)
   - Original iterates: grouped_bubbles (sorted) → find falling bubble
   - Old C++ iterated: falling bubble → search grid
   - Fixed to match original algorithm structure

### Impact:
Without distance_to_root sorting, chain reactions were assigned to **random groups** instead of prioritizing **groups closest to root**. This caused incorrect/unpredictable chain behavior.

### Known Issues:
- ~~Excessive chain assignments~~ **FIXED 2026-01-11**
- ~~Missing distance_to_root sorting~~ **FIXED 2026-01-11**
- Cascading chain reactions need testing

### Status:
**NEEDS TESTING** - Code fixes complete, awaiting gameplay verification - 2026-01-11
- ✓ distance_to_root calculation implemented
- ✓ Sorting by distance implemented
- ✓ Loop order corrected
- ⏳ Needs in-game testing with chain reactions enabled

### Testing:
```bash
# Build completed successfully
cd /Users/dericchau/ai/fb2-port/frozen-bubble-sdl2/build
cmake --build .

# Test in-game:
./frozen-bubble-sdl2
# Enable chain reactions in settings
# Create scenarios with falling bubbles that can chain
# Verify chains target groups closer to root first
# Test cascading chain reactions
grep "Chain target found" frozen-bubble-*.log | wc -l
```

---

## Module 4: Malus System (Attack Bubbles)

**Objective**: Verify malus calculation, distribution, sending, receiving

**Original Code**: `frozen-bubble` lines 956-1050, 2220-2260
**C++ Port**: `bubblegame.cpp` lines 1950-2150

### Key Functions to Compare:
- Original: Malus calculation (line 958: `$malus = @{$destroyed[$player]} + @{$falling[$player]} - 2`)
- Original: `malus_change()` (line 2220)
- C++ Port: `SendMalusToOpponent()`
- C++ Port: `ProcessMalusQueue()`

### Verification Checklist:
- [ ] Malus calculation formula correct
- [ ] Malus split distribution (2-5 players)
- [ ] Victim generates attack bubbles (NOT attacker)
- [ ] Attack bubble visual generation timing
- [ ] Attack bubble sticking logic
- [ ] Self-attack prevention
- [ ] Message format: `g{destNick}:{count}`

### Known Issues:
- **Self-bombing bug** - Players seeing their own malus attacks
- Verify victim/attacker roles match original

### Testing:
```bash
# Start 3-player game
# Create chain reaction on one player
# Verify other players receive malus, not self
grep "Sending malus\|Awarding.*malus" frozen-bubble-creator.log
```

---

## Module 5: Network Protocol & Connection

**Objective**: Verify network connection, protocol version, message framing

**Original Code**: `Net.pm` lines 1-300
**C++ Port**: `networkclient.cpp` lines 1-400

### Key Functions to Compare:
- Original: `fb_server_init()` (Net.pm)
- Original: Protocol handshake (line 50-100)
- C++ Port: `Connect()`
- C++ Port: `SendCommand()`

### Verification Checklist:
- [ ] Protocol version: FB/1.2
- [ ] Message framing (text-based in lobby, binary in-game)
- [ ] Connection handshake sequence
- [ ] Lobby commands (LIST, CREATE, JOIN, START)
- [ ] Keepalive/ping messages
- [ ] Graceful disconnect handling

### Known Issues:
- None currently

### Testing:
```bash
# Start server and connect 2 clients
# Verify handshake messages match original
# Test disconnect/reconnect
```

---

## Module 6: Network Game Messages

**Objective**: Verify in-game network message handlers (s, f, g, m, M, l, n, F, A)

**Original Code**: `frozen-bubble` lines 1100-1500, 2400-2700
**C++ Port**: `bubblegame.cpp` lines 2750-3200

### Key Functions to Compare:
- Original: Message handlers in main loop (line 2400+)
- C++ Port: `HandleNetworkMessages()` switch statement

### Message Types:
- `s` - Stick: bubble stuck to grid
- `f` - Fire: player shot bubble
- `g` - Give malus: attack message
- `m` - Malus visual: attack bubble falling
- `M` - Malus stuck: attack bubble landed
- `l` - Lost: player died (original line 1918)
- `n` - New game: ready for next round (line 2464)
- `F` - Finish: winner found (line 1943)
- `A` - Attack target: targeting specific player (line 1345)

### Verification Checklist:
- [ ] Each message format matches exactly
- [ ] Message timing (when to send)
- [ ] Message recipients (broadcast vs specific)
- [ ] State changes triggered by messages
- [ ] Error handling for malformed messages

### Known Issues:
- ~~'F' message sending bug~~ **FIXED 2026-01-11**
- ~~'n' message auto-response in 3+ players~~ **FIXED 2026-01-11**
- Self-bombing related to 'g'/'m' message handling

### Testing:
```bash
# Play 3-player game and log all messages
# Compare message sequences with original game
grep ">>> Sending game data\|Processing game message" frozen-bubble-*.log
```

---

## Module 7: Win/Loss Detection & Logic

**Objective**: Verify game end conditions, winner detection, loss handling

**Original Code**: `frozen-bubble` lines 1933-1950, 1910-1930
**C++ Port**: `bubblegame.cpp` lines 2200-2280

### Key Functions to Compare:
- Original: Winner detection (line 1933-1944)
- Original: Player loss (line 1910-1925)
- C++ Port: `CountLivingPlayers()`
- C++ Port: Winner detection in render loop

### Verification Checklist:
- [x] Detect when 1 player remains (winner)
- [x] Set gameFinish = true immediately
- [x] Send 'F' message with winner nickname
- [x] Losing player sends 'l' message
- [x] Spectator mode (losing players watch winner)
- [x] Handle all players dead (draw)

### Known Issues:
- None currently

### Status:
**COMPLETED** - Verified and fixed 2026-01-11

---

## Module 8: Round Transitions & Next Game

**Objective**: Verify round end → next round transition, 'n' message sync

**Original Code**: `frozen-bubble` lines 2321-2500
**C++ Port**: `bubblegame.cpp` lines 2399-2900

### Key Functions to Compare:
- Original: "finished" state handler (line 2321-2380)
- Original: 'n' message logic (line 2464-2468)
- C++ Port: `opponentReadyForNewGame` logic
- C++ Port: `ReloadGame()`

### Verification Checklist:
- [x] Wait for all players to press ENTER
- [x] Each player sends 'n' message
- [x] Auto-respond 'n' only in 2-player games
- [x] Synchronize level reload
- [x] Winner advances to next level
- [x] Losers replay same level
- [x] Clear game state properly

### Known Issues:
- None currently

### Status:
**COMPLETED** - Verified and fixed 2026-01-11

---

## Module 9: UI/Menus & Lobby

**Objective**: Verify menu screens, network lobby, player list display

**Original Code**: `frozen-bubble` lines 3000-4500
**C++ Port**: `mainmenu.cpp` lines 1-1800

### Key Functions to Compare:
- Original: Menu rendering (line 3000+)
- Original: Network lobby (line 4000+)
- C++ Port: `Render()`
- C++ Port: `NetPanelRender()`

### Verification Checklist:
- [ ] Main menu layout
- [ ] Options panel
- [ ] Network lobby player list
- [ ] Player name display
- [ ] Ready/Not Ready indicators
- [ ] Start game button (host only)
- [ ] Font sizes match original

### Known Issues:
- **Player name font too large** - Should use smaller font on mini windows

### Testing:
```bash
# Navigate all menus
# Host game and check lobby display
# Compare font sizes with original
```

---

## Module 10: Multiplayer Player Layout

**Objective**: Verify 2-5 player screen layouts, positions, sizes

**Original Code**: `Stuff.pm` lines 65-135 (POS_MP definitions)
**C++ Port**: `bubblegame.cpp` lines 400-600

### Key Functions to Compare:
- Original: `@POS_MP` array (Stuff.pm line 65)
- C++ Port: `NewGame()` player positioning

### Layouts to Verify:
- [ ] 2 players: side-by-side full grids
- [ ] 3 players: center full + 2 mini top corners
- [ ] 4 players: center full + 3 mini (top-left, top-right, bottom-left)
- [ ] 5 players: center full + 4 mini corners

### Verification Checklist:
- [ ] Grid offsets match original
- [ ] Mini player scale factor (16x16 vs 32x32 bubbles)
- [ ] Player nickname positions
- [ ] Malus bubble sizes (mini vs full)
- [ ] Win counter positions

### Known Issues:
- Player name positioning needs adjustment
- Font size for mini players

### Testing:
```bash
# Test each player count (2, 3, 4, 5)
# Verify layout matches original screenshots
# Check mini player rendering
```

---

## Verification Workflow

### For Each Module:

1. **Setup**
   ```bash
   cd /Users/dericchau/ai/fb2-port/frozen-bubble-sdl2
   # Clean logs before testing
   rm frozen-bubble-*.log
   ```

2. **Read Original Code**
   - Open `bin/frozen-bubble` at specified line range
   - Understand the Perl logic
   - Note key variables, formulas, conditionals

3. **Read C++ Port Code**
   - Open corresponding C++ file at specified line range
   - Trace through the logic
   - Map Perl variables to C++ equivalents

4. **Line-by-Line Comparison**
   - For each logical block in original:
     - Find equivalent in C++ port
     - Verify logic matches
     - Check for off-by-one errors
     - Verify formulas are identical
     - Check boundary conditions

5. **Document Differences**
   - Note intentional adaptations (Perl→C++ differences)
   - Flag bugs or logic mismatches
   - Document missing features

6. **Fix Issues**
   - Implement fixes matching original behavior
   - Add comments referencing original line numbers
   - Build and test changes

7. **Testing**
   - Run specified test scenarios
   - Check logs for expected behavior
   - Compare with original game behavior

8. **Mark Complete**
   - Update this MODULE_PLAN.md status
   - Document in TODO.md
   - Move to next module

---

## Merge Strategy

Once all modules are verified:

1. **Create verification branch**
   ```bash
   git checkout -b verified-modules
   ```

2. **Each module creates a feature branch**
   ```bash
   git checkout -b module-1-core-loop
   # Make fixes
   git commit -m "Module 1: Core Loop verified and fixed"
   ```

3. **Merge modules one at a time**
   ```bash
   git checkout verified-modules
   git merge module-1-core-loop
   git merge module-2-bubble-physics
   # etc.
   ```

4. **Integration testing**
   ```bash
   # Test all modules working together
   # Run full 2-5 player games
   # Verify no regressions
   ```

5. **Merge to main**
   ```bash
   git checkout main
   git merge verified-modules
   ```

---

## Current Status Summary

| Status | Module Count |
|--------|--------------|
| ✅ Completed | 3 (Bubble Physics, Win/Loss, Round Transitions) |
| 🔄 In Progress | 1 (Chain Reactions) |
| ⏳ Not Started | 6 |

**Next Priority**: Module 4 (Malus System) - Fixes self-bombing bug

---

## Notes for Claude Instances

- **Context Limit**: Each module is small enough to fit in context with original code
- **Reference Format**: Always cite original line numbers: `(original line XXX)`
- **Testing Required**: Each module must have working tests before marked complete
- **Documentation**: Update TODO.md after completing each module
- **Cross-Module Issues**: If you find issues spanning multiple modules, document in this file

---

## Quick Start Commands

```bash
# View original Perl code
less +814 /Users/dericchau/ai/fb2-port/frozen-bubble-sdl2/bin/frozen-bubble

# View C++ code
code /Users/dericchau/ai/fb2-port/frozen-bubble-sdl2/src/bubblegame.cpp

# Build after changes
cd /Users/dericchau/ai/fb2-port/frozen-bubble-sdl2/build
cmake --build .

# Test (clean logs first)
rm frozen-bubble-*.log
./frozen-bubble-sdl2

# Check logs
grep "interesting_pattern" frozen-bubble-creator.log
```

---

**Last Updated**: 2026-01-11 22:30 PST
**Project**: Frozen Bubble SDL2 Port
**Repository**: `/Users/dericchau/ai/fb2-port/frozen-bubble-sdl2`
