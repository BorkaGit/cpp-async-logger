
# C++ Async Logger

A lightweight asynchronous file logger written in **C++20**. Log messages are submitted from application threads to a thread-safe queue and written to a file by a dedicated background thread.

This is a learning project focused on C++ concurrency, RAII, synchronization, and clean separation between a library and its example application.

## Features

- **Asynchronous logging:** `Log()` queues a message instead of performing file I/O on the calling thread.
- **Thread-safe producers:** multiple threads can submit messages concurrently.
- **Dedicated writer thread:** a single consumer writes queued messages to a file.
- **Efficient waiting:** `std::condition_variable` lets the writer sleep when there is no work.
- **Graceful shutdown:** the destructor drains queued messages and joins the writer thread.
- **Append mode:** existing log contents are preserved between runs.
- **Simple CMake setup:** the logger is compiled as a static library and linked by a sample executable.

## Requirements

- A C++20-compatible compiler
- CMake 3.16 or newer
- Threading support (`std::thread`)

Developed and tested on Ubuntu under WSL2. Other platforms are not yet verified.

## Project structure

```text
cpp-async-logger/
├── CMakeLists.txt
├── include/
│   └── AsyncLogger.hpp
├── src/
│   ├── AsyncLogger.cpp
│   └── main.cpp
└── README.md
```

## Build and run

From the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/AsyncLoggerApp
```

The sample application writes messages to `application.log` in its **working directory**.

To count the lines in the log file:

```bash
wc -l application.log
```

The current sample submits **400 messages**: 100 from the main thread and 100 from each of three additional producer threads. If the log file was empty before running, a successful run should produce 400 lines.

> The logger opens files in append mode, so repeated runs add more lines to the same file.

## Usage

```cpp
#include "AsyncLogger.hpp"

int main() {
    AsyncLogger logger("application.log");

    logger.Log("Application started");
    logger.Log("Processing request");

    // The destructor waits for queued messages to be written.
}
```

`Log()` returns after queuing the message; it does **not** wait for the writer to finish writing it. The logger must remain alive while other threads call `Log()`.

## How it works

```text
Main thread ────┐
Producer #1 ────┤
Producer #2 ────┼──> Thread-safe queue ──> Writer thread ──> application.log
Producer #3 ────┘
```

1. Application threads call `Log()` to enqueue messages under a mutex.
2. The writer thread waits on a condition variable until messages arrive or shutdown is requested.
3. The writer removes messages from the queue and performs file I/O without holding the queue mutex.
4. During destruction, the logger requests shutdown and waits until the writer has processed the remaining queue.

## Current limitations and planned improvements

This is an early version, not yet intended as a production logging library.

- [ ] Add a `Flush()` operation that waits for earlier messages and flushes the file stream.
- [ ] Improve error reporting for write failures in the background thread.
- [ ] Add automated unit and concurrency tests.
- [ ] Add log levels and timestamps.
- [ ] Add cross-platform CI builds.

## Technical focus

C++20 · `std::thread` · `std::mutex` · `std::condition_variable` · Producer–Consumer · RAII · CMake
