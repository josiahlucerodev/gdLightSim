# Build Tooling 
- godot 4.4 beta 3: https://godotengine.org/download/archive/4.4-beta3/ 
- python: https://www.python.org/downloads/
- scons: terminal$ pip install scons
- c++ compiler
    * windows: msvc - https://visualstudio.microsoft.com/visual-cpp-build-tools/
    * linux (debian): gcc - sudo apt install build-essential

- notes:
    * on windows godot download must be renamed from Godot_v4.4-beta3_win64.exe to godot.exe 
    * on windows must edit PATH enviroment variable with path to godot directory. Reference: https://www.c-sharpcorner.com/article/how-to-addedit-path-environment-variable-in-windows-11/
    * on windows must edit PATH enviroment variable with path to scons directory. Reference: https://stackoverflow.com/questions/59097128/how-do-i-get-scons-to-work-on-the-windows-10-command-line

# Getting started:
1. Clone
    - `git clone --recursive https://github.com/josiahlucerodev/gdLightSim`
    - `cd gdLightSim`
2. Build.
    - `scons`
3. Run.
    - `godot --path project/ --editor`

# Programming 
- vscode: https://code.visualstudio.com/download
    * c/c++ Extension Pack 