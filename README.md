# Lab 8 — Variant 12: Video Surveillance
C++20 console application using SQLite, sqlite_orm, CMake and vcpkg manifest mode.

Requirements: Visual Studio 2026 with C++ workload, MSVC x64/x86, Windows 11 SDK, CMake, vcpkg.

The CMake preset assumes vcpkg at:
`D:/vs 2 mogna ydalit cherez uninstall/VC/vcpkg`
Change `CMakePresets.json` if the folder differs.

From the project folder:
```powershell
cmake --preset vs2026-x64
cmake --build --preset debug
```
Run:
```powershell
.\out\build\vs2026-x64\Debug\Lab8Variant12.exe
```
The application creates `surveillance.db` and seeds 5 cameras and 15 events on each run.
