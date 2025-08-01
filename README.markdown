# Logging Server

This is a TCP-based logging server implemented in C++ using Boost.Asio. The server listens on port 4000, accepts connections from clients, and writes log records to files organized by client names. Each client sends its name followed by log records, which are stored with sequence numbers for traceability.

## Features

- **Asynchronous I/O**: Utilizes Boost.Asio for efficient handling of multiple client connections concurrently using coroutines.
- **Thread-Safe Logging**: Ensures safe file operations with mutexes for each client's log file.
- **Client Name Validation**: Prevents directory traversal attacks by validating client names.
- **Structured Log Files**: Logs are written to files in the `test` directory with sequence numbers for each entry.
- **Test Script**: Includes a shell script (`testclient.sh`) to verify server functionality.

## Requirements

- C++20 compliant compiler (e.g., GCC 10+, MSVC 2019+)
- CMake 3.15 or higher
- Boost libraries (version 1.70 or higher, specifically Boost.Asio)
- Netcat (for testing with `testclient.sh` on Unix-like systems)

## Building the Project

1. Clone the repository:
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

## Running the Server

Start the server by running:
```bash
./build/server
```

The server listens on port 4000 for incoming TCP connections.

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

The `testclient.sh` script automates testing by:
1. Starting the server in the background.
2. Sending test data using netcat.
3. Verifying the content of the generated log file.
4. Stopping the server.

Run the test with:
```bash
./testclient.sh
```

## Directory Structure

| File              | Description                                      |
|-------------------|--------------------------------------------------|
| `server.cpp`      | Main server implementation using Boost.Asio.     |
| `client.hpp`      | Header for the Client class handling log files.  |
| `client.cpp`      | Implementation of the Client class.              |
| `setting.hpp`     | Defines configuration settings (e.g., port 4000).|
| `CMakeLists.txt`  | CMake configuration for building the project.    |
| `testclient.sh`   | Shell script for testing server functionality.   |

## Design Decisions

- **Boost.Asio**: Chosen for its robust asynchronous I/O capabilities, enabling scalable handling of multiple clients.
- **Coroutines**: Used to simplify asynchronous code and improve readability.
- **Thread Safety**: Mutexes ensure safe file access for each client’s log file.
- **Simple Protocol**: Clients send a name followed by log records, making it easy to implement clients in any language supporting TCP.

## Future Improvements

- Configurable port and log directory via a configuration file or command-line arguments.
- Client authentication to restrict access to authorized clients.
- Log rotation to manage file sizes.
- Comprehensive unit and integration tests using a framework like Google Test.
- Cross-platform support by adjusting CMake for non-Windows environments.
- Containerization with Docker for easier deployment.