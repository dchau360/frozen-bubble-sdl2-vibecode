# Frozen-Bubble: SDL2
<p align="center">
  <img src="https://github.com/user-attachments/assets/c68db5c9-7e72-4d19-8e98-c598a3f5e54e">
</p>


An ongoing port of the original Frozen-Bubble 2 into a newer C++ codebase using SDL2. I am planning on reimplementing most features from the original Frozen-Bubble 2 into the codebase, but some of them might be out of my knowledge. Feel free to contribute if you want!

Checklist: https://github.com/Erizur/frozen-bubble-sdl2/issues/1

# Building
You need to obtain the following libraries: 
- SDL2
- SDL2_image
- SDL2_mixer
- SDL2_ttf
- [iniparser](https://github.com/ndevilla/iniparser)

Make sure these are detected by CMake. After that, you're ready to compile!
You will be able to compile the server-related code if you're trying to compile for macOS and Linux.
For the moment, the DATA_DIR path is fixed to the path of the repository, unless you specify your own path.

# Game Modes

## Single Player
Classic Frozen Bubble gameplay with chain reactions and scoring system.

## 2-Player Local (LAN Game)
- Select "LAN game" or "2-player game" from menu
- Two players on same computer with different keyboard controls
- Player 1: Arrow keys
- Player 2: C/X/V/D keys

## Network Multiplayer
Complete online multiplayer with integrated server hosting.

### Quick Start - Network Play

**Easiest way (host in-game):**
1. Launch game and select "Network game"
2. Press **H** to host (auto-starts server)
3. Press ENTER, then C to create game, S to start
4. Other players: Press **J** to join, enter your IP

**Alternative (manual server):**
```bash
./start-server.sh -d    # Run with debug output
```

See [NETWORK_PLAY.md](NETWORK_PLAY.md) for detailed documentation.

### Server Options
```bash
./start-server.sh -h           # Show help
./start-server.sh -p 1234      # Custom port
./start-server.sh -d           # Debug mode
./start-server.sh -l           # LAN discovery
```
