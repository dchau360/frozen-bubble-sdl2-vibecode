# Network Play Testing Guide

## What Was Fixed

1. **ENTER key bug**: Fixed so it doesn't try to connect when on the host/join choice screen
2. **Reconnection support**: NetworkClient now properly handles connection attempts even if singleton exists
3. **Better error logging**: Added detailed logs to help debug server startup and connection issues
4. **Longer server startup delay**: Increased from 500ms to 1000ms to ensure server is ready
5. **More server search paths**: Added `./build/server/fb-server` to search paths

## Testing Steps

### Test 1: Host and Connect (Same Machine)

**Terminal 1 - Host:**
```bash
cd build
./frozen-bubble-sdl2
```

1. Select "Network game"
2. Press **H** to host
3. Check console output for:
   ```
   Starting local server on port 1511...
   Waiting for server to initialize...
   Server started with PID XXXX on port 1511
   ```
4. You should see "Hosting Server" screen with:
   - Host: 127.0.0.1
   - Port: 1511
   - "Server running..."
5. Press ENTER to connect
6. Should see "Connected to server 127.0.0.1:1511"
7. Should enter lobby showing "Status: In Lobby"

**Terminal 2 - Join:**
```bash
cd build
./frozen-bubble-sdl2
```

1. Select "Network game"
2. Press **J** to join
3. Enter Host: 127.0.0.1, Port: 1511
4. Press ENTER
5. Should see "Connected to server 127.0.0.1:1511"
6. Should enter lobby showing "Status: In Lobby"

### Test 2: Create and Start Game

**In Host Terminal:**
1. Press **C** to create game
2. Status should change to "In Game Room"

**In Join Terminal:**
1. Press **C** to create/join game
2. Status should change to "In Game Room"

**In Either Terminal:**
1. Press **S** to start game
2. Game should begin (currently limited network sync)

### Test 3: Server Cleanup

**In Host Terminal:**
1. Press ESC to disconnect
2. Check console for "Server stopped"
3. Verify server process is gone:
   ```bash
   ps aux | grep fb-server
   ```
   Should show no server process

### Test 4: Error Handling

**With No Server:**
1. Start game without hosting
2. Select "Network game" → **J**
3. Enter 127.0.0.1:1511
4. Press ENTER
5. Should see error: "Failed to connect to 127.0.0.1:1511"

## Expected Console Output

### When Hosting Successfully
```
Starting local server on port 1511...
Trying server path: ./server/fb-server
Waiting for server to initialize...
Server started with PID 12345 on port 1511
Connected to server 127.0.0.1:1511
```

### When Server Binary Not Found
```
Starting local server on port 1511...
Trying server path: ./server/fb-server
Trying server path: ../server/fb-server
Trying server path: ../../server/fb-server
Trying server path: ./build/server/fb-server
Failed to start server - binary not found in any path
```

### When Connection Fails
```
Failed to connect: Connection refused
Failed to connect to 127.0.0.1:1511
```

## Common Issues

### "Failed to connect" even when hosting
- **Cause**: Server binary not found or failed to start
- **Check**: Look for "Trying server path" messages
- **Fix**: Run from correct directory (build/) or use manual server

### Server doesn't stop when exiting
- **Cause**: Process cleanup issue
- **Check**: `ps aux | grep fb-server`
- **Fix**: Manual cleanup: `killall fb-server`

### Port already in use
- **Cause**: Previous server still running
- **Check**: `lsof -i :1511`
- **Fix**: Kill existing server or change port

### Second client can't connect
- **Cause**: Server not fully initialized or connection logic bug
- **Check**: Wait 2-3 seconds after "Server running..." appears
- **Fix**: Increased delay should help, or use manual server

## Debugging Tips

### Enable Verbose Logging
The server already runs in debug mode (`-d` flag), which logs to console.

### Check Server Logs
If you started server manually:
```bash
./build/server/fb-server -q -d -p 1511
```
You'll see all connection attempts and commands.

### Check Network Connection
```bash
# See if server is listening
lsof -i :1511

# Test connection manually
telnet 127.0.0.1 1511
```

### Check Server Binary
```bash
# Verify it exists
ls -la build/server/fb-server

# Verify it's executable
file build/server/fb-server
```

## Success Criteria

✅ Host can start server and connect
✅ Second client can connect to hosted server
✅ Both clients enter lobby
✅ Both clients can create/join game room
✅ Server stops when host exits
✅ Error messages are clear and helpful

## Known Limitations

- Game state synchronization is basic (bubble shots only)
- No player list display yet
- Join command (J in lobby) not fully implemented
- Malus bubbles not synchronized
- No spectator mode
- Windows not supported (fork() limitation)
