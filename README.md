---

# Order Book 

Implementation of an order book I made that supports both **Market Orders** and **Limit Orders**. Orders are pre-processed before insertion (Only if Limit), and any remaining limit orders are stored in the order book for future matching.

> **Inspiration:**
> This project is inspired by [Tzadiko's Orderbook](https://github.com/Tzadiko/Orderbook/tree/master)

---

## Features

* Supports **Market** and **Limit** orders.
  * **Limit Orders:** Attempt to fill as much as possible. Any remaining unfilled portion is added to the order book.
  * **Market Orders:** Treated as *fill or kill* — they must be completely filled immediately or they are discarded.
* Pre-processes and matches orders before inserting into the book.
* Stores unmatched **Limit** orders in an internal order book.
* Multithreaded processing of orders from a text file.
* Unit tests included for core components.

---

## Prerequisites

To build and run this project:
1. Install Cmake (Minimum Ver. 3.14)
2. Clone the [GoogleTest repository](https://github.com/google/googletest).
3. Place the repository inside your project directory (e.g., `external/googletests`).

> ⚠️ If you change the folder location, make sure to update the path in the root `CMakeLists.txt` using `add_subdirectory`.

---

## Build Environment

* **OS:** Windows 11 
* **Compiler:** GCC port from MinGW
* **C++ Version:** C++20
* **Others:** macOS/Linux should work using system default compilers.

---

---

## Tools

* **Profiler:** perf 6.8.12

## Build Instructions

1. Go to your project root directory.

2. Create your build files:

   ```bash
   cmake -S . -B build
   ```

   > **For profiling** 
   ```bash
   cmake -S . -B build-perf -DCMAKE_BUILD_TYPE=RelWithDebInfo
   ```

   > **Note:**
   > On Windows using MinGW, CMake may default to `nmake`. To avoid this:


   ```bash
   cmake -S . -B build -G "MinGW Makefiles"
   ```
   > *Credit to [this Stack Overflow post](https://stackoverflow.com/questions/69338088/error-while-configuring-cmake-project-running-nmake-failed).*


3. Build the project:

   ```bash
   cmake --build build
   ```

---

## Running Tests

⚠️ Before building and executing the test file (TestOrderBook.exe), you must move the "Simple Getters" member functions labeled in the OrderBook class from private to public to test the class AND go to projectdir/tests and uncomment the last portion of the CMakeLists.txt file AND follow the instructions of what to comment out in projectdir/src/OrderBook.cpp 

After building:

* Navigate to `projectdir/build/tests`.
* You will find the following test executables:

  * `TestOrder.exe`
  * `TestOrderBook.exe`
  * `TestTrade.exe`

You can run these individually. Test source files are located in `projectdir/tests`.

---

## Running the Main Program

The main executable is located in:

```
projectdir/build/src/main.exe
```

This program uses `orderbook.cpp` and other components to:

* Read orders from a text file
* Validate the orders
* Process them in a multithreaded environment
* **Note** whenever you rerun the main executable be sure to delete the output.txt otherwise you won't see the output of the trades when you make changes to data.txt
---

## Project Structure

```
projectdir/
├── src/
│   ├── main.cpp
│   └── ...
├── tests/
│   ├── TestOrder.cpp
│   └── ...
├── external/
│   └── googletests/
├── build/
├── CMakeLists.txt
└── README.md
```

---
