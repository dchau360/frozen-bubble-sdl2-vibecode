# Frozen Bubble SDL2 Port - TODO List

## Future Features

### Malus Attack Indicator / "Attacking Me" Display
**Priority**: Medium
**Status**: Not Started

Display a visual indicator showing which players are currently targeting you for malus attacks.

**Requirements**:
- Show icons/indicators for players who are targeting you
- Update when players change their target
- Display near your player area for easy visibility

**Original Game Implementation** (frozen-bubble:1345-1356, 1477-1489):
- Uses 'A' network message to broadcast target selection
- `A{targetNick}` = "I am now targeting this player"
- `A` (empty) = "I am not targeting anyone"
- Displays small icons in `attackingme` area showing which players are targeting you
- Icons are player-specific (rp1, rp2, rp3, rp4 graphics)

**Implementation Plan**:
1. Handle 'A' message in network protocol
   - Format: `A{targetNick}` or `A` (empty)
   - Track which players are targeting local player
   - Update `attackingme` list dynamically
2. Load "attacking me" indicator graphics
   - Small player icons (mini portraits)
   - Display up to 4 indicators side-by-side
3. Render indicators near player area
   - Position: Above or beside local player grid
   - Show all players currently targeting you
4. Send 'A' messages when local player changes target
   - Requires implementing single-player targeting mode first
   - Broadcast when cycling through targets

**Note**: This is part of the Single Player Targeting feature. When a player selects a specific opponent to attack (instead of splitting malus), they broadcast this via 'A' message so the target can display the indicator.

**Related Code Locations**:
- `SendMalusToOpponent()` - bubblegame.cpp:1969
- 'g' message handler - bubblegame.cpp:2879-2905
- `ProcessMalusQueue()` - bubblegame.cpp:2022
- Malus bubble rendering - bubblegame.cpp:2600+

---

## Module Verification System

**NEW APPROACH**: Breaking codebase into focused modules for line-by-line verification against original.

📋 **See MODULE_PLAN.md for complete verification plan**

Each module will be verified by a separate Claude instance:
- Read START_HERE.md to begin
- Choose a module from MODULE_PLAN.md
- Compare original Perl → C++ port line-by-line
- Fix discrepancies
- Test thoroughly
- Mark complete

**Current Status**: 2/10 modules completed (Win/Loss, Round Transitions)

---

## Known Issues / Bugs to Fix

### High Priority - Module 4 (Malus System)
- [ ] **Self-bombing bug** - Players seeing their own malus attacks hit themselves
  - Verify victim/attacker logic matches original line 2220-2260
  - Check 'g' message routing and processing
  - See MODULE_PLAN.md Module 4 for details

### Visual Issues - Module 9 (UI/Menus)
- [ ] **Player name font size** - Player names on mini windows should use smaller font
- [ ] **Player name alignment** - Player nicknames need better positioning relative to mini windows

### Gameplay Issues - Future Feature
- [ ] **Single Player Targeting Mode** - Implement ability to target specific opponent for malus (sendmalustoone)
  - Requires key binding to cycle targets
  - 'A' network message to broadcast selection
  - Visual indicator for current target
  - See frozen-bubble line 1330-1343

---

## Recently Completed

### v0.x.x (2026-01-11)
- [x] Fixed excessive chain reaction assignments (2026-01-11)
  - Added check to prevent assigning chains to bubbles that are part of groups targeted by earlier chain reactions
  - When a chain is assigned, the entire target group is marked as "chained" and unavailable for other chains
  - Matches original frozen-bubble line 829-850 behavior
  - Prevents wasted chain assignments to groups that will explode from earlier chains
- [x] Fixed premature return to menu in 3+ player games (2026-01-11)
  - Fixed 'F' message sending bug: incorrect condition `!bArray.playerState` changed to `bubbleArrays[winnerIdx].playerState != LEFT`
  - Fixed auto-response 'n' message bug: disabled auto-response in 3+ player games (bubblegame.cpp:2886)
  - In 3+ player games, local player must explicitly press ENTER to send 'n' and proceed to next round
  - Added diagnostic logging to QuitToTitle() and ReturnToMenu() to track premature exits
  - Game now correctly waits for winner to press ENTER before starting next round
- [x] Fixed network game start bug - creator was kicked to menu when clicking Start (2026-01-11)
  - Moved SetupNewGame() call before early return check in NetPanelRender()
  - Game now properly transitions to 3-player mode
- [x] Fixed spectator mode - losing players now stay connected instead of crashing/disconnecting (2026-01-11)
  - Don't render shooter bubbles (curLaunch/nextBubble) for LOST players to prevent invalid texture crashes
  - Matches original frozen-bubble behavior (line 2027: `if (!is_mp_game())` spectator logic)
- [x] Added timestamped log files - each client now creates unique log (frozen-bubble-YYYYMMDD-HHMMSS.log) (2026-01-11)
- [x] Fixed malus bubble rendering - now uses correct size (mini 16x16 or full 32x32) based on player window
- [x] Fixed 'm' and 'M' message handlers - now finds correct opponent array instead of hardcoding to array 1
- [x] Added detailed logging for 'g' message handling to diagnose self-attacking issues
- [x] Fixed malus system - victim/attacker logic (2026-01-10)
- [x] Fixed malus distribution for 2-5 players with split mode (2026-01-10)
- [x] Fixed chain reactions - now assigned once when bubbles fall, not every frame (2026-01-10)
- [x] Fixed game crash on start - initialize curLaunch/nextBubble before sync (2026-01-10)
- [x] Implemented proper cascading chain reactions (2026-01-10)

---

## Development Notes

### Chain Reactions
Chain reactions now work correctly:
- Assigned ONCE in `AssignChainReactions()` when bubbles fall
- Cascades properly: CR lands → destroys group → more bubbles fall → more CRs assigned
- Matches original frozen-bubble line 814-865 behavior

### Malus Distribution
Malus is split equally among all living opponents:
- Formula: `(malusCount + livingOpponents.size() - 1) / livingOpponents.size()` (ceiling division)
- Automatically excludes dead players
- Supports 2-5 player games
