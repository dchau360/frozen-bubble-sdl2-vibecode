# 🎯 Start Here - Module Verification System

Welcome! You're working on verifying the Frozen Bubble SDL2 port against the original Perl code.

## Your First Step

**Read the full plan:**
```bash
cat /Users/dericchau/ai/fb2-port/frozen-bubble-sdl2/MODULE_PLAN.md
```

## Your Second Step

**Ask the user:**
> "Which module should I work on? (1-10, or type 'list' to see available modules)"

## Available Modules

1. ⏳ Core Game Loop & State Management
2. ✅ Bubble Physics & Collision *(Completed)*
3. 🔄 Chain Reactions *(In Progress)*
4. ⏳ **Malus System** *(Priority - has self-bombing bug)*
5. ⏳ Network Protocol & Connection
6. ⏳ Network Game Messages
7. ✅ Win/Loss Detection *(Completed)*
8. ✅ Round Transitions *(Completed)*
9. ⏳ UI/Menus & Lobby
10. ⏳ Multiplayer Player Layout

Legend: ✅ Done | 🔄 In Progress | ⏳ Not Started

## Quick Reference

**Original Code**: `/Users/dericchau/ai/fb2-port/frozen-bubble-sdl2/bin/frozen-bubble`
**C++ Port**: `/Users/dericchau/ai/fb2-port/frozen-bubble-sdl2/src/`

**Build**: `cd /Users/dericchau/ai/fb2-port/frozen-bubble-sdl2/build && cmake --build .`
**Logs**: `/Users/dericchau/ai/fb2-port/frozen-bubble-sdl2/frozen-bubble-*.log`

## The Process

For your chosen module:
1. Read original Perl code (line ranges in MODULE_PLAN.md)
2. Read C++ port code (file locations in MODULE_PLAN.md)
3. Compare line-by-line
4. Document differences
5. Fix bugs
6. Test thoroughly
7. Update MODULE_PLAN.md status

## Important

- MCPs are DISABLED - use only core Claude Code tools
- Focus on ONE module at a time
- Always reference original line numbers in comments
- Test before marking complete

---

**Start by asking the user which module to work on!**
