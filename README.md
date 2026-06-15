# Macro Recorder for Geometry Dash

A powerful and lightweight macro recorder mod for Geometry Dash, built with the Geode SDK. Record your gameplay, save it to a file, and play it back seamlessly on any level. Supports both Windows and Android!

#  Repository Structure
Create these files in your project folder exactly as shown:

macro-geode/
├── .gitignore
├── LICENSE
├── README.md
├── logo.svg
├── mod.json
└── src/
    ├── main.cpp
    └── main.hpp

## ✨ Features
* **Record & Play:** Capture your exact inputs and replay them flawlessly.
* **Save & Load:** Store your macros locally and load them anytime.
* **Clean UI:** A beautiful, integrated in-game popup to manage your macros.
* **Cross-Platform:** Fully compatible with Windows and Android versions of Geometry Dash.

## 📥 Installation

### Windows
1. Ensure you have Geode installed.
2. Download the latest .geode file from the Releases page.
3. Place the .geode file into your Geode mods folder.
4. Restart Geometry Dash.

### Android
1. Ensure you have Geode installed on your Android device.
2. Download the .geode file from the Releases page.
3. Open Geometry Dash, go to the Geode menu, and select "Install from file".
4. Navigate to your downloaded .geode file and install it.
5. Restart the game.

## 🎮 How to Use
1. Open Geometry Dash and click the **Macro** button on the main menu.
2. Enter a name for your macro file in the text box.
3. Click **Record** and play the level.
4. Click **Stop** when finished.
5. Click **Save File** to save your recording.
6. To play it back, load the level, open the Macro menu, click **Load File**, and then click **Play**.

## 🛠️ Building from Source

### Prerequisites
* Geode CLI
* CMake
* A C++ compiler (Visual Studio for Windows, NDK for Android)

### Steps
```bash
git clone <your-repo-url>
cd macro-geode
geode build
