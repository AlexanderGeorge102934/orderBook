---

# Order Book 

Implementation of an order book I made that supports both **Market Orders** and **Limit Orders**. Orders are pre-processed before insertion (Only if Limit), and any remaining limit orders are stored in the order book for future matching.

> **Inspiration:**
> This project takes structural inspiration by [Tzadiko's Orderbook](https://github.com/Tzadiko/Orderbook/tree/master) while implementing an original order book logic and multithreaded design. 

---

## Features

* Supports **Market** and **Limit** orders.
  * **Limit Orders:** Attempt to fill as much as possible. Any remaining unfilled portion is added to the order book.
  * **Market Orders:** Treated as *fill or kill* — they must be completely filled immediately or they are discarded.
* Pre-processes and matches orders before inserting into the book.
* Stores unmatched **Limit** orders in an internal order book.
* Supports multithreading 
* Unit tests included for core components.

---

## Prerequisites

To build and run this project:
1. Install Cmake (Minimum Ver. 3.14)
2. Clone the [GoogleTest repository](https://github.com/google/googletest).
3. Place the repository inside your project directory (e.g., `external/googletests`).
4. Download the [Boost Library ver 1.89.0](https://www.boost.org/) and place only the header-only subset folder inside the external folder as well labeled "boost"

> ⚠️ If you change the folder location, make sure to update the path in the root `CMakeLists.txt` using `add_subdirectory`.

---
## Technical Stack

### **Libraries (Dependencies)**

* **Boost (Header-Only):** For boost.asio and spsc_queue 
* **GoogleTest:** Framework for unit testing.
* **WinSock2 / MSWSock:** (Windows only) Handle network socket logic for the trading system.

### **Tools Used**

* **Compiler:** GCC 
* **Build System:** CMake (Minimun Ver. 3.14)
* **Profiler:** `perf` (Linux 6.8.12)
* **Virtualization:** VirtualBox 7.2.2 (used only to run `perf` in a Linux environment since I do not have a linux os)

---
## Build Instructions

1. **Open your terminal** in the project root directory.
2. **Generate and Build** using one of the following configurations:

### **Option A: Production Build **

```bash
# Generate
cmake -S . -B build-prod -DBUILD_TESTING_SUITE=OFF -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build-prod

```

### **Option B: Development & Testing Build**

Use this if you just want to run unit tests. Includes debug symbols.

```bash
# Generate
cmake -S . -B build-test -DBUILD_TESTING_SUITE=ON -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build-test

```

### **Option C: Profiling Build (for `perf`)**

Use this for performance analysis. It runs at production speed but keeps function names visible for the profiler.

```bash
# Generate
cmake -S . -B build-perf -DBUILD_TESTING_SUITE=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Build
cmake --build build-perf

```

---

### **⚠️ Windows (MinGW) Users**

If you are on Windows and using MinGW, CMake might default to `nmake`, which will cause an error. To get around this I fixed it by adding `-G "MinGW Makefiles"` to the generation command.
Also be sure to remove any prior build folders.

**Example for Testing Build on Windows:**

```bash
cmake -S . -B build-test -G "MinGW Makefiles" -DBUILD_TESTING_SUITE=ON -DCMAKE_BUILD_TYPE=Debug

```

> *Credit to [this Stack Overflow post](https://stackoverflow.com/questions/69338088/error-while-configuring-cmake-project-running-nmake-failed) for the fix.*

---

## Running Tests

If you built using **Option B** or **C**, your test executables will be located in:
`projectdir/build-<name>/tests/`

You can run them individually (e.g., `./TestOrderBook`) or run all tests at once using:

```bash
cd build-test
ctest

```

---

## Running the Main Program

The main executable is located in:

```
projectdir/build/src/main.exe
```

---

## Project Structure

```
projectdir/
├── include/
│   ├── OrderBook.h
│   └── ...
├── src/
│   ├── main.cpp
│   └── ...
├── tests/
│   ├── TestOrder.cpp
│   └── ...
├── external/
│   └── googletests/
│   └── boost/
├── build/
├── CMakeLists.txt
└── README.md
```

---
