# Temperature Conversion System with Live Weather API

A C++ console application built as a college case study that converts temperatures between Celsius, Fahrenheit, and Kelvin — with live weather data fetched from the OpenWeatherMap API.

---

## Features

- **Manual Temperature Conversion** — Convert between Celsius, Fahrenheit, and Kelvin with validated input and absolute-zero checks.
- **Live City Weather** — Fetch real-time temperature for any city worldwide using the OpenWeatherMap API.
- **Temperature Classification** — Automatically classifies temperatures (Freezing, Cold, Comfortable, Warm, Hot, Boiling).
- **Conversion History** — Tracks all conversions performed during a session.
- **Export to File** — Save the full conversion history to `conversion_history.txt`.

---

## Menu Options

| Option | Description |
|--------|-------------|
| 1 | Convert from Celsius |
| 2 | Convert from Fahrenheit |
| 3 | Convert from Kelvin |
| 4 | View Conversion History |
| 5 | Export History to File |
| 6 | Exit |
| 7 | Get Live City Temperature & Convert |

---

## OOP Concepts Used

| Concept | Where |
|---------|-------|
| **Classes & Objects** | `Temperature`, `HistoryManager` |
| **Encapsulation** | Private data members with public getters and methods |
| **Constructors** | Parameterized constructor in `Temperature` |
| **Vectors (STL)** | `vector<Record>` for dynamic history storage |
| **File Handling** | `ofstream` for exporting history to a text file |
| **API Integration** | HTTP requests via libcurl to OpenWeatherMap |

---

## Tech Stack

- **Language:** C++
- **Libraries:** `<iostream>`, `<iomanip>`, `<vector>`, `<fstream>`, `<limits>`, `<string>`, `<curl/curl.h>`
- **API:** [OpenWeatherMap](https://openweathermap.org/api) (current weather data)
- **Build:** g++ with libcurl

---

## Prerequisites

- C++ compiler (g++ / clang++)
- [libcurl](https://curl.se/libcurl/) installed on your system

### Install libcurl

**macOS:**
```bash
brew install curl
```

**Ubuntu / Debian:**
```bash
sudo apt-get install libcurl4-openssl-dev
```

---

## Build & Run

```bash
g++ -o main main.cpp -lcurl
./main
```

---

## Sample Output

```
+------------------------------------------+
|      TEMPERATURE CONVERSION SYSTEM       |
+------------------------------------------+
|  MANUAL CONVERSIONS                      |
|  1. Convert from Celsius                 |
|  2. Convert from Fahrenheit              |
|  3. Convert from Kelvin                  |
+------------------------------------------+
|  LIVE WEATHER                            |
|  7. Get City Temperature & Convert       |
+------------------------------------------+
|  HISTORY                                 |
|  4. View Conversion History              |
|  5. Export History to File               |
+------------------------------------------+
|  6. Exit                                 |
+------------------------------------------+

+------------------------------------------+
|           CONVERSION RESULTS             |
+------------------------------------------+
| Input       :   100.00 C                 |
+------------------------------------------+
| Celsius     :   100.00 °C                |
| Fahrenheit  :   212.00 °F                |
| Kelvin      :   373.15 K                 |
+------------------------------------------+
| Status      : Hot                        |
+------------------------------------------+
```

---

## Project Structure

```
c++case study/
├── main.cpp                  # Source code
├── main                      # Compiled binary
├── conversion_history.txt    # Exported history (generated at runtime)
├── README.md                 # Project documentation
└── build/
    └── Debug/
```

---

## Author

Kushal — College Case Study Project
