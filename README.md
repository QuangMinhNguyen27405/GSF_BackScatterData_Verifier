# Backscatter Data Verifier

This repository contains a program written in C that scans `.gsf` (Generic Sensor Format) files within a specified folder, checks if they contain backscatter data, and logs the results in a text file.


## 🚀 Quick Start

### 🗂 Download & Run

1. Click on the **Code** dropdown on the [repository page](https://github.com/FREAC/GSF_BackScatterData_Verifier).
2. Download the `.zip` file and extract it.
3. Run the verifier on **Windows**:

```bash
check_gsf.exe "C:\path\to\gsf\folder" "C:\path\to\output\folder"
```

### Example Log File Output

The program will generate a log file named `gsf_backscatter_log.txt` to the "C:\path\to\output\folder" folder. The log file will contain entries for each `.gsf` file that does and does not contain backscatter data, along with a summary of the total files checked and the count of files missing backscatter data.

## 🛠 Development Setup

### 🔧 Requirements

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

## 🧱 Build From Source

1. **Clone the Repository**
   ```bash
   git clone https://github.com/FREAC/GSF_BackScatterData_Verifier.git
   cd GSF_BackScatterData_Verifier/src
   ```

2. **Open in VS Code**

   Open the repository folder in VS Code.

3. **Compile the Code**
   
#### 🪟 On Windows:
Use the provided `build.bat` script:
```bash
./build.bat
```

#### 🐧 On Linux/macOS:
Use the provided makefile:
```bash
make
```

4. **Run the Program**

   Execute the compiled program with the path to the folder containing `.gsf` files:
   ```bash
   ./check_gsf.exe "C:\path\to\gsf\folder" "C:\path\to\output\folder" 
   ```

   Open the Terminal in VS Code (`Ctrl + ~`).

## Contributing

We welcome contributions! Feel free to open issues or submit pull requests to enhance functionality, documentation, or bug fixes.
