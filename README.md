---

# Order Book 

The purpose of this project is to implement a C++ trading order book supporting Market and Limit orders. It features multithreaded order processing with a custom thread pool, comprehensive unit tests with GoogleTest, and uses Boost for networking and lock-free queues.

> **Inspiration:**
> This project was inspired by [Tzadiko's Orderbook](https://github.com/Tzadiko/Orderbook/tree/master)

---

## Features

* **Market and Limit Orders**
  * **Limit Orders:** Attempt to fill as much as possible. Any remaining unfilled portion is stored in the order book for future matching.
  * **Market Orders:** Treated as *fill or kill* — must be completely filled immediately or are discarded.
* **Order Matching Engine:** Pre-processes and matches incoming orders against existing orders before insertion.
* **Internal Order Book:** Stores unmatched Limit orders organized by price level using data structures (maps with linked lists).
* **Multithreaded Processing:** Custom thread pool that utilizes available hardware threads with thread-safe operations.
* **Comprehensive Testing:** Unit tests using GoogleTest for core components including Order, OrderBook, and Trade logic.
* **Performance Profiling:** Multiple build configurations (Debug, Release, and profiling builds) with support for `perf` analysis.

---

## Prerequisites

To build and run this project:
1. Install Cmake (Minimum Ver. 3.14)
2. Clone the [GoogleTest repository](https://github.com/google/googletest).
3. Place the repository inside your project directory (e.g., `external/googletests`).
4. Download the [Boost Library ver 1.89.0](https://www.boost.org/) and place only the header-only subset folder inside the external folder as well labeled "boost"

> If you change the folder location, make sure to update the path in the root `CMakeLists.txt` using `add_subdirectory`.

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

### **Windows (MinGW) Users**

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

Simply run the executable and after connecting to the socket of whatever computer is hosting the executable send commands like 

```
BUY LIMIT 100 50 
```
or 
```
SELL MARKET 100 50 
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

## Side Note

I included a `display()` function tool inside my orderbook class. This is to help visually see what's happening but this is NOT practical. I don't plan on creating an optimal function to display the current state of the orderbook but I also want to at least allow people curious enough to run my code to visually see what's happening. The display function was added at the very end of this project. You can simply remove the only reference of it in the `src/TradingSystem.cpp` file if you want and it will function with the original purpose, mentioned at the beginning, in mind.

## Final Thoughts

Ultimately, this project was mainly just a deep dive into a variety of topics rather than building a commercial grade orderbook. I'm not an expert in HFT.

### Key Takeaways

Key Takeaways from this project for me were understanding:

* **System design choices**
* **Data structures**
* **Multithreading**
* **Networking**
* **The build lifecycle**
* **Sampling profilers**
* **Unit testing**
* **RAII**
* **Memory optimization**
* **Seperation of concerns**
* **Self documenting and clean code**
* **The boost library**

This was a project I have been working on for quite some time and I'm happy to have done it. No one needs to go to this extent when building a project to learn something but if you want to learn from any of the topics that I mentioned in the key takeaways I would highly recommend it.

---
