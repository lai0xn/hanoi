# Towers of Hanoi: Visualization & Benchmark

A high-performance C++ application that visualizes the Towers of Hanoi algorithm and benchmarks the performance difference between **Recursive** and **Iterative** solutions. Built using **C++17**, **OpenGL**, and **ImGui**.

## 🎥 Demo

![Project Screenshot](demo/demo.png)

## 🚀 Features

* **Interactive Visualization:**
    * Watch the algorithm solve the puzzle step-by-step.
    * Control playback speed, pause, step forward, and reset.
    * **Toggle Algorithms:** Switch between Recursive and Iterative solutions visually.
    * Modern, dark-themed UI with a split-screen layout (Sidebar + Game View).
* **Performance Benchmarking:**
    * Run performance tests on large datasets ($N$ disks).
    * Compare execution time (ms) between Iterative and Recursive approaches.
    * Export results to CSV (`hanoi_results.csv`) for analysis.
* **Cross-Platform Core:**
    * Supports macOS (OpenGL 3.2 Core Profile) and Linux/Windows (OpenGL 3.0).

# Setup Guide for Windows
This guide will help you set up the development environment and run the Hanoi Tower project on Windows.
## Prerequisites
* MinGW-w64 (C++ compiler)
* GLFW library
## Step 1: Install MSYS2

### Download and Install
1. Go to [https://www.msys2.org/](https://www.msys2.org/)
2. Download the installer (msys2-x86_64-*.exe)
3. Run the installer and follow the installation wizard
4. Install to the default location: `C:\msys64`

### Update MSYS2
1. After installation, **MSYS2 MINGW64** will open automatically
2. Run this command to update the package database:
   ```bash
   pacman -Syu
   ```
3. When prompted, press `Y` and hit Enter
4. The window will close. **Reopen MSYS2 MINGW64** from the Start menu

### Install Development Tools
In the MSYS2 MINGW64 terminal, run:
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-glfw
```
Press `Y` when prompted to proceed with installation.

---

## Step 2: Add MinGW to Windows PATH

1. Press `Win + R`, type `sysdm.cpl`, and press Enter
2. Go to the **Advanced** tab
3. Click **Environment Variables**
4. Under **System variables**, find and select **Path**, then click **Edit**
5. Click **New** and add: `C:\msys64\mingw64\bin`
6. Click **OK** on all windows to save
7. **Restart your terminal/PowerShell** for changes to take effect

---

## Step 3: Verify Installation

Open a new **PowerShell** or **Command Prompt** window and run:
```bash
g++ --version
```

You should see version information. If you get "not recognized" error, make sure you restarted your terminal after adding to PATH.

---

## Step 4: Build the Project

Navigate to the project directory:
```bash
cd path\to\hanoi
```

Compile the project with g++:
```
bash g++ -I./imgui -I./imgui/backends -I./include src/main.cpp src/hanoi.cpp imgui/*.cpp imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp -lglfw3 -lopengl32 -lgdi32 -o hanoi.exe
```

---

## Step 5: Run the Program

After successful compilation, run:
```bash
.\hanoi.exe
```

Or simply double-click `hanoi.exe` in the project folder.



## Quick Command Reference

| Command | Description |
|---------|-------------|
| `g++ [files] -o hanoi.exe` | Compile the project |
| `.\hanoi.exe` | Run the program |
| `g++ --version` | Check compiler version |
