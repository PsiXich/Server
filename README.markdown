# Logging Server

This is a TCP-based logging server implemented in C++ using Boost.Asio. The server listens on a configurable port (default 4000), accepts connections from clients, and writes log records to files organized by client names in the "test" directory. Each client sends its name followed by log records, which are stored with sequence numbers for traceability. The server can be stopped securely via console input with a password.

## Features

- **Asynchronous I/O**: Utilizes Boost.Asio for efficient handling of multiple client connections concurrently using coroutines.
- **Thread-Safe Logging**: Ensures safe file operations with mutexes for each client's log file.
- **Client Name Validation**: Prevents directory traversal attacks by validating client names (rejects empty names or those containing ".." or "/").
- **Structured Log Files**: Logs are written to files in the `test` directory with sequence numbers for each entry (e.g., `[0001] ---=== Client "name" ===---`).
- **Configuration File**: Uses `config.cfg` to set port, log directory (though currently hardcoded to "test"), and password for stopping the server.
- **Secure Shutdown**: Server can be stopped by typing "stop" in the console and entering the configured password.
- **Test Script**: Includes a Python script (`test.py`) to verify server functionality using socket connections.

## Requirements

- C++20 compliant compiler (e.g., GCC 10+, MSVC 2019+)
- CMake 3.15 or higher
- Boost libraries (version 1.70 or higher, specifically Boost.Asio, Boost.System, and Boost.Program_Options)
- Python 3 (for running the `test.py` script; optional for testing)
- Netcat or similar tool (for manual testing on Unix-like systems)

## Building the Project

1. Clone the repository (if applicable):
   ```bash
   git clone https://github.com/yourusername/yourrepository.git
   cd yourrepository
   ```

2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure the project with CMake:
   ```bash
   cmake ..
   ```

4. Build the project:
   ```bash
   cmake --build .
   ```

The `server` executable will be generated in the `build` directory.

## Configuration

The server reads settings from `config.cfg` in the current directory. Example content:
```
port = 4000
log_dir = test
password = pass
```

- `port`: The TCP port to listen on (default: 4000).
- `log_dir`: Intended log directory (default: test), but currently hardcoded to "test" in the code.
- `password`: Password required to stop the server via console (default: pass).

## Running the Server

Start the server by running:
```bash
./server
```

The server listens on the configured port for incoming TCP connections. To stop the server:
1. Type `stop` in the console.
2. Enter the configured password when prompted.

The server will gracefully shut down.

## Client Interaction

Clients connect to the server via TCP, send their name followed by a newline (`\n`), and then send log records, each terminated by a newline. The server creates a log file named `test/<client_name>.log` for each client.

Example using netcat:
```bash
echo -e "myclient\nlog record 1\nlog record 2\n" | nc localhost 4000
```

This creates a file `test/myclient.log` with content like:
```
[0001] ---=== Client "myclient" ===---
[0002] log record 1
[0003] log record 2
[0004] ^^^ ^^^ ^^^ Client stopped ^^^ ^^^ ^^^
```

## Testing

The `test.py` script automates testing by:
1. Connecting to the server (default: localhost:4000).
2. Sending a client name (default: "test_client").
3. Sending sample log records.
4. Allowing time for the server to process.

Run the test with:
```bash
python test.py
```

Verify the generated `test/test_client.log` file matches the expected format.

## Directory Structure

| File              | Description                                      |
|-------------------|--------------------------------------------------|
| `server.cpp`      | Main server implementation using Boost.Asio.     |
| `client.hpp`      | Header for the Client class handling log files.  |
| `client.cpp`      | Implementation of the Client class.              |
| `CMakeLists.txt`  | CMake configuration for building the project.    |
| `config.cfg`      | Configuration file for port, log_dir, and password. |
| `test.py`         | Python script for testing server functionality.  |

## Design Decisions

- **Boost.Asio**: Chosen for its robust asynchronous I/O capabilities, enabling scalable handling of multiple clients.
- **Coroutines**: Used to simplify asynchronous code and improve readability.
- **Thread Safety**: Mutexes ensure safe file access for each client’s log file.
- **Simple Protocol**: Clients send a name followed by log records, making it easy to implement clients in any language supporting TCP.
- **Configuration Parsing**: Uses Boost.Program_Options to read from `config.cfg` for flexible settings (though log_dir usage is pending implementation).
- **Console Shutdown**: Added for controlled server stopping with password protection.

## Future Improvements

- Fully implement configurable log directory (currently parsed but hardcoded).
- Client authentication to restrict access to authorized clients.
- Log rotation to manage file sizes.
- Comprehensive unit and integration tests using a framework like Google Test.
- Cross-platform refinements (e.g., handle Windows-specific libraries more dynamically).
- Containerization with Docker for easier deployment.
- Command-line overrides for config file settings.