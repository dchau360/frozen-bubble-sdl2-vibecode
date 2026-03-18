# Network Play Guide

## Overview
Frozen Bubble now supports network multiplayer through a client-server architecture.

## Game Modes

### 1. LAN Game (2-Player Local)
- **Menu**: Select "LAN game" from main menu
- **Usage**: Two players on the same computer using different keyboard controls
- **Controls**:
  - Player 1: Arrow keys (UP to shoot, LEFT/RIGHT to aim)
  - Player 2: C to shoot, X/V to aim

### 2. Network Game (Online Multiplayer)
- **Menu**: Select "Network game" from main menu
- **Usage**: Play against opponents over the network
- **Requirements**: A running Frozen Bubble server

## Starting the Server

The server binary is located at: `build/server/fb-server`

### Basic Server Usage

```bash
# Start server on default port (1511)
cd build/server
./fb-server

# The server will display:
# - Server version
# - Listening address and port
# - Connected players
# - Game room status
```

### Server Command Line Options

```bash
# Specify custom port
./fb-server -p 1234

# Show help
./fb-server --help
```

### Server in Background

```bash
# Run server in background
./fb-server &

# Check if server is running
ps aux | grep fb-server

# Stop server
killall fb-server
```

## Connecting to a Server

### Option 1: Host Server In-Game (Easiest)

1. Launch Frozen Bubble
2. Select "Network game" from main menu
3. Choose one of:
   - **H** - Host a game server (automatically starts local server)
   - **J** - Join a game server (connect to existing server)

If you chose **Host**:
- Server automatically starts on port 1511
- Host is set to 127.0.0.1
- Press ENTER to connect to your own server
- Server stops automatically when you exit

If you chose **Join**:
- Enter server details:
  - **Host**: IP address or hostname
  - **Port**: Server port (default: 1511)
  - Press TAB to switch between Host and Port fields
  - Press BACKSPACE to delete characters
- Press ENTER to connect

### Option 2: Manual Server (Advanced)

1. Start server separately (see "Starting the Server" section below)
2. Launch Frozen Bubble
3. Select "Network game" → "Join"
4. Enter server host and port
5. Press ENTER to connect

## Network Game Lobby Commands

Once connected to the server:

- **C** - Create a new game room
- **J** - Join an existing game (TODO: needs game list)
- **S** - Start the game (only when in a room)
- **P** - Part/leave current game room
- **ESC** - Disconnect from server

## Network Game Flow

1. Connect to server → Status: "In Lobby"
2. Press C to create game → Status: "In Game Room"
3. Wait for opponent to join (or they create and you join)
4. Press S to start → Status: "Playing"
5. Play the game - bubble shots are synchronized automatically
6. After game ends, press P to return to lobby

## Testing Network Play Locally

### Easy Way (In-Game Hosting)

**Client 1 (Host):**
```bash
cd build
./frozen-bubble-sdl2
```
- Select "Network game"
- Press **H** to host
- Press ENTER to connect
- Press C to create game, S to start

**Client 2 (Join):**
```bash
cd build
./frozen-bubble-sdl2
```
- Select "Network game"
- Press **J** to join
- Host: 127.0.0.1, Port: 1511
- Press ENTER to connect
- Press C to create/join game

### Manual Way (Separate Server)

**Terminal 1 - Start Server:**
```bash
cd build/server
./fb-server -q -d
```

**Terminal 2 - Start First Client:**
```bash
cd build
./frozen-bubble-sdl2
# Select "Network game" → "Join"
# Host: 127.0.0.1, Port: 1511
# Press ENTER, then C to create game
```

**Terminal 3 - Start Second Client:**
```bash
cd build
./frozen-bubble-sdl2
# Select "Network game" → "Join"
# Host: 127.0.0.1, Port: 1511
# Press ENTER, then C to join game
```

## Troubleshooting

### Cannot Connect to Server
- Verify server is running: `ps aux | grep fb-server`
- Check server port matches client: default is 1511
- For remote servers, ensure firewall allows connections
- Check server logs for error messages

### Network Game Not Starting
- Ensure both players are in the same game room
- Only the room creator can start the game with 'S'
- Check server console for game state messages

### Bubble Shots Not Synchronizing
- Check network connection quality
- Server logs will show SHOT messages if working
- Verify both clients are in IN_GAME state

## Network Protocol Notes

The game uses the Frozen Bubble network protocol:
- **fb-client/1.2** - Client identification
- **NICK** - Set player nickname
- **CREATE** - Create game room
- **JOIN** - Join game room
- **START** - Start game
- **PART** - Leave game room
- **SHOT** - Broadcast bubble shot (bubbleId, x, y, direction)

## Future Enhancements

Planned features:
- [ ] Game room list (JOIN command with room selection)
- [ ] Chat functionality (TALK command)
- [ ] Spectator mode
- [ ] Player ready/not ready indicators
- [ ] Malus bubble synchronization
- [ ] Game state recovery on reconnect
