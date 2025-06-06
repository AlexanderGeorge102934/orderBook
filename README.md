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

1. Clone the [GoogleTest repository](https://github.com/google/googletest).
2. Place the repository inside your project directory (e.g., `external/googletests`).

> ⚠️ If you change the folder location, make sure to update the path in the root `CMakeLists.txt` using `add_subdirectory`.

---

## Build Environment

* **OS:** Windows (MinGW)
* **Compiler:** GCC
* **C++ Version:** C++20
* **Others:** macOS/Linux should work using system default compilers.

---

## Build Instructions

1. Navigate to your project root directory.

2. Create your build files:

   ```bash
   cmake -S . -B build
   ```

   > **Note:**
   > On Windows using MinGW, CMake may default to `nmake`. To avoid this:

   ```bash
   cmake -S . -B build -G "MinGW MakeFiles"
   ```

3. Build the project:

   ```bash
   cmake --build build
   ```

---

## Running Tests

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
