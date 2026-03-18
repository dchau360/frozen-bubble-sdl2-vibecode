# Quick Start Guide

## Playing Frozen Bubble

### Single Player
1. Run `./frozen-bubble-sdl2` (from build directory)
2. Select "1-player game"
3. Choose game mode:
   - **Training**: Practice levels
   - **Puzzle**: Complete predefined puzzles
   - **Random**: Random bubble layouts

### 2-Player Local
1. Run `./frozen-bubble-sdl2`
2. Select "LAN game" or "2-player game"
3. Choose whether to enable chain reactions (Y/N)

**Controls:**
- **Player 1**: Arrow keys (↑ shoot, ←→ aim, ↓ center)
- **Player 2**: C (shoot), X/V (aim), D (center)

### Network Multiplayer

#### Easiest Way (In-Game Hosting)

**Player 1 (Host):**
1. Run `./frozen-bubble-sdl2`
2. Select "Network game"
3. Press **H** to host (starts server automatically)
4. Press ENTER to connect
5. Press C to create game, S to start

**Player 2 (Join):**
1. Run `./frozen-bubble-sdl2`
2. Select "Network game"
3. Press **J** to join
4. Enter Host: 127.0.0.1, Port: 1511
5. Press ENTER to connect
6. Press C to create/join game, S to start

#### Manual Server Way

**Terminal 1 - Start Server:**
```bash
cd build
../start-server.sh -d
```

**Terminal 2 - Player 1:**
```bash
cd build
./frozen-bubble-sdl2
```
- Select "Network game" → "Join"
- Host: 127.0.0.1, Port: 1511
- Press ENTER to connect
- Press C to create game
- Press S to start when ready

**Terminal 3 - Player 2:**
```bash
cd build
./frozen-bubble-sdl2
```
- Select "Network game" → "Join"
- Host: 127.0.0.1, Port: 1511
- Press ENTER to connect
- Press C to create game (or J to join existing)
- Press S to start

#### Play Over Network

**Option 1: Host In-Game**
1. Host machine: Launch game, select "Network game"
2. Press **H** to host
3. Note your local IP: `ifconfig` (Mac/Linux) or `ipconfig` (Windows)
4. Other players: Select "Network game" → **J** to join
5. Enter host's IP address, port 1511

**Option 2: Dedicated Server**
1. Server machine: `./start-server.sh -d`
2. Note server IP address
3. All players: Select "Network game" → **J** to join
4. Enter server IP and port 1511
5. Follow lobby commands to create/join/start game

## Keyboard Shortcuts

### In-Game (All Modes)
- **ESC** - Pause / Return to menu
- **C** - Toggle colorblind mode
- **F11** - Mute audio

### Menu Navigation
- **↑↓** or **←→** - Navigate menu
- **ENTER** - Select option
- **ESC** - Back / Quit

### Network Lobby
- **C** - Create game
- **J** - Join game
- **S** - Start game
- **P** - Part (leave game room)
- **ESC** - Disconnect

## Troubleshooting

### Game won't start
- Check that DATA_DIR is set correctly in build
- Ensure all assets are in share/ directory

### Network connection fails
- Verify server is running: `ps aux | grep fb-server`
- Check firewall settings
- Ensure port 1511 is not in use: `lsof -i :1511`

### "Cannot create socket" error
- Port may be in use
- Try different port: `./start-server.sh -p 1512`

### Audio not working
- Check SDL2_mixer installation
- Verify audio files exist in share/sounds/

### No bubbles visible
- Check if colorblind mode is enabled (press C to toggle)
- Verify graphics settings in menu

## Tips

- Chain reactions give score multipliers in single player
- Use the ceiling to bounce shots in tricky situations
- In network mode, bubble shots sync automatically
- Server can handle multiple concurrent games
- Use debug mode `-d` when testing to see detailed logs

## Advanced

### Custom Server Configuration
```bash
# High player capacity
./build/server/fb-server -q -m 100 -p 1511

# LAN discovery enabled
./build/server/fb-server -q -l -p 1511

# Custom transmission rate
./build/server/fb-server -q -t 200000 -p 1511
```

### Building from Source
```bash
mkdir build && cd build
cmake ..
cmake --build .
cd ..
```

### Installing
```bash
cd build
sudo cmake --build . --target install
```

### Uninstalling
```bash
cd build
sudo cmake --build . --target uninstall
```

## Getting Help

- See [NETWORK_PLAY.md](NETWORK_PLAY.md) for network details
- Check server logs when debugging connection issues
- Use `-d` flag for debug output
- Server help: `./build/server/fb-server -h`
