# HeatmapBuilder
This project aims to generate heatmap pictures from measurements of Laboratory of Quantum Cascade Lasers
## Build
```bash
mkdir build
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.9.2\msvc2022_64"
cmake --build build --config Release
"C:\Qt\6.9.2\msvc2022_64\bin\windeployqt.exe" --release ".\build\Release\QtHeatmapViewer.exe"
```
