# In-Game Server Hosting Feature

## Overview
Frozen Bubble now includes an integrated server hosting feature that eliminates the need to manually start a server from the terminal. This makes network multiplayer much more accessible.

## How It Works

### User Flow
1. Launch Frozen Bubble
2. Select "Network game" from main menu
3. Choose between:
   - **H** - Host a game server (starts local server automatically)
   - **J** - Join a game server (connect to existing server)

### When You Choose "Host"
- The game automatically forks a child process running `fb-server`
- Server starts on the configured port (default: 1511)
- Server runs in quiet mode (`-q`) to prevent external registration
- Server runs in debug mode (`-d`) for console logging
- Host address is automatically set to 127.0.0.1
- After 500ms delay, the connection screen appears
- You can then connect to your own server and create a game

### When You Choose "Join"
- Standard connection screen appears
- You manually enter host and port
- Connect to any running server (local or remote)

### Server Lifecycle
- **Started**: When you press H in the host/join choice screen
- **Running**: Server process runs as child of game process
- **Stopped**: Automatically when you:
  - Press ESC to leave network panel
  - Disconnect from server
  - Return to main menu
  - Quit the game

## Technical Implementation

### Server Process Management
```cpp
// Starting server
pid_t pid = fork();
if (pid == 0) {
    // Child process - exec server binary
    execl("./server/fb-server", "fb-server", "-q", "-d", "-p", portStr, NULL);
}
// Parent stores PID for cleanup
serverPid = pid;
```

### Server Binary Search Paths
The game searches for the server binary in these locations:
1. `./server/fb-server` (from game directory)
2. `../server/fb-server` (from build directory)
3. `../../server/fb-server` (from nested locations)
4. `/usr/local/bin/fb-server` (system-wide install)

### Server Cleanup
```cpp
void StopLocalServer() {
    kill(serverPid, SIGTERM);      // Send termination signal
    waitpid(serverPid, &status, WNOHANG);  // Non-blocking wait
    serverPid = -1;
    serverHosting = false;
}
```

## Benefits

### For Users
- **No terminal required**: Everything in-game
- **Automatic setup**: Server starts with correct settings
- **Simple workflow**: H to host, J to join
- **Automatic cleanup**: Server stops when done
- **Visual feedback**: Status shows "Hosting: Yes" in lobby

### For Developers
- **Cross-platform**: Works on macOS, Linux
- **Process isolation**: Server runs in separate process
- **Clean lifecycle**: Fork/exec/kill pattern
- **Error handling**: Multiple search paths for server binary
- **Status tracking**: PID and flag management

## Code Structure

### Files Modified
- **mainmenu.h**: Added server state variables and methods
- **mainmenu.cpp**:
  - Added StartLocalServer() - forks and execs server
  - Added StopLocalServer() - terminates server process
  - Added host/join choice screen (networkInputMode == 2)
  - Added H/J key handlers
  - Updated ESC handler to stop server
  - Updated ReturnToMenu() to cleanup server

### State Variables
```cpp
bool serverHosting = false;    // Is server running?
int serverPid = -1;            // Server process ID
int networkInputMode = 0;      // 0=host, 1=port, 2=choice screen
```

### Flow States
1. **Choice Screen** (networkInputMode == 2)
   - Shows H/J options
   - Press H → StartLocalServer() → go to connection screen
   - Press J → go to connection screen

2. **Connection Screen** (networkInputMode == 0 or 1, !networkInLobby)
   - Shows host/port inputs
   - Shows "Server running..." if hosting
   - Press ENTER → connect
   - Press ESC → stop server (if hosting) and return

3. **Lobby Screen** (networkInLobby)
   - Shows connection status
   - Shows "Hosting: Yes" if hosting
   - Normal lobby commands
   - Press ESC → disconnect and stop server

## Platform Notes

### macOS/Linux
- Uses POSIX fork()/exec()/kill()
- Works out of the box
- Requires `<unistd.h>`, `<sys/wait.h>`, `<signal.h>`

### Windows
- **Not currently supported** (fork() not available)
- Alternative approaches for Windows:
  - Use CreateProcess() instead of fork()
  - Use Windows job objects for cleanup
  - Use TerminateProcess() instead of kill()

## Future Enhancements

Potential improvements:
- [ ] Windows support using CreateProcess()
- [ ] Server log display in-game
- [ ] Custom server port selection before hosting
- [ ] Server settings (max players, game modes)
- [ ] Server status indicator (running/stopped/error)
- [ ] Automatic local IP detection and display
- [ ] UPnP/NAT-PMP for automatic port forwarding
- [ ] Server performance metrics

## Testing

### Test In-Game Hosting
1. Launch game
2. Select "Network game" → press H
3. Verify "Server running..." appears
4. Check console logs for "Server started with PID"
5. Press ENTER to connect
6. Verify connection succeeds
7. Press ESC to exit
8. Check console logs for "Server stopped"
9. Verify process is cleaned up: `ps aux | grep fb-server`

### Test Server Binary Not Found
1. Rename server binary temporarily
2. Launch game and press H
3. Should see error: "Failed to start server"
4. Restore binary name

### Test Multiple Clients
1. Client 1: Host with H, connect, create game
2. Client 2: Join with J, enter 127.0.0.1:1511, connect
3. Both should see each other in lobby
4. Start and play game
5. Client 1: Exit - server should stop
6. Client 2: Should disconnect gracefully

## Troubleshooting

### Server Won't Start
- Check server binary exists: `ls ./server/fb-server`
- Check execute permissions: `chmod +x ./server/fb-server`
- Check port availability: `lsof -i :1511`
- Review console logs for exec error

### Server Doesn't Stop
- Check for orphaned processes: `ps aux | grep fb-server`
- Manual cleanup: `killall fb-server`
- Check SIGTERM handler in server

### Can't Connect to Own Server
- Verify server actually started (check logs)
- Wait 1-2 seconds after seeing "Server running..."
- Check firewall isn't blocking localhost
- Try manual server to isolate issue

## Performance Considerations

- **Fork overhead**: ~1ms on modern systems
- **Startup delay**: 500ms hardcoded delay for server initialization
- **Memory**: Server process separate, ~5-10MB
- **CPU**: Minimal when idle, scales with player count
- **Cleanup**: Non-blocking waitpid() prevents hangs

## Security Considerations

- Server runs with same privileges as game
- Only listens on localhost when auto-started
- No external registration (-q flag)
- SIGTERM for clean shutdown
- No shell injection (uses execl, not system)
