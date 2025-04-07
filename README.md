# Backscatter Data Checker

This repository contains a program written in C that scans `.gsf` (Generic Sensor Format) files within a specified folder, checks if they contain backscatter data, and logs the results in a text file.

## Features

- Ensures the "text" folder exists (creates it if missing).
- Iterates through all `.gsf` files in the given directory.
- Checks for backscatter data using `gsfRead()`.
- Logs results to `text/gsf_backscatter_log.txt`.
- Summarizes findings, displaying total checked files and missing backscatter counts.

## Requirements

### For Windows

1. **MinGW (GCC Compiler)**
   - Download MinGW-w64 from [MinGW-w64](https://www.mingw-w64.org/downloads/).
   - Install it and add `C:\MinGW\bin` (or its equivalent) to the system PATH.
2. **VS Code Extensions**
   - Install the C/C++ extension (`ms-vscode.cpptools`).
   - Install the Code Runner extension (optional, for easy execution).

### For Linux/macOS

1. **GCC Compiler**
   - Ensure GCC is installed (`gcc` command should be available).
2. **VS Code Extensions**
   - Install the C/C++ extension (`ms-vscode.cpptools`).
   - Install the Code Runner extension (optional).

## Setup

1. **Clone the Repository**
   ```bash
   git clone https://github.com/FREAC/BackScatter_Haeder_Data.git
   cd BackScatter_Haeder_Data
   ```

2. **Install Dependencies**

   Follow the instructions in the [Requirements](#requirements) section.

3. **Open in VS Code**

   Open the repository folder in VS Code.

## Usage

1. **Create a New Folder for Your Project**

   Inside the repository, create a new folder for your project.

2. **Create the Source File**

   Inside the folder, create a file named `check_gsf.c`.

   Copy and paste the provided C code into `check_gsf.c`.

3. **Compile the Code**
   ```bash
   gcc -o check_gsf check_gsf.c -lgsf
   ```

4. **Run the Program**

   Execute the compiled program with the path to the folder containing `.gsf` files:
   ```bash
   ./check_gsf.exe C:\path\to\gsf\folder
   ```

   Open the Terminal in VS Code (`Ctrl + ~`).

## Example Log File Output

The program will generate a log file named `gsf_backscatter_log.txt` in the `text` folder. The log file will contain entries for each `.gsf` file that does not contain backscatter data, along with a summary of the total files checked and the count of files missing backscatter data.

## Contributing

Feel free to contribute to this repository by submitting pull requests or opening issues.
