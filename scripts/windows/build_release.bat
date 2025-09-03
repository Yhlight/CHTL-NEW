@echo off
setlocal EnableDelayedExpansion

echo ===============================================
echo CHTL Compiler Release Build Script (Windows)
echo Professional Build System - VS2019/VS2022
echo ===============================================

set "PROJECT_ROOT=%~dp0..\.."
set "BUILD_DIR=%PROJECT_ROOT%\build\release"
set "CMAKE_FILE=%PROJECT_ROOT%\CMakeLists_Professional.txt"

echo.
echo Step 1: Environment Check...
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found in PATH
    echo Please install CMake and add it to PATH
    pause
    exit /b 1
)

echo CMake found: 
cmake --version | findstr "cmake version"

echo.
echo Step 2: Visual Studio Detection...
set "VS_YEAR="
set "VS_GENERATOR="

rem Check for VS2022
if exist "C:\Program Files\Microsoft Visual Studio\2022" (
    set "VS_YEAR=2022"
    set "VS_GENERATOR=Visual Studio 17 2022"
    echo Visual Studio 2022 detected
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019" (
    set "VS_YEAR=2019"
    set "VS_GENERATOR=Visual Studio 16 2019"
    echo Visual Studio 2019 detected
) else (
    echo WARNING: Visual Studio not found, using default generator
    set "VS_GENERATOR=Visual Studio 17 2022"
)

echo Using generator: !VS_GENERATOR!

echo.
echo Step 3: Clean Previous Build...
if exist "%BUILD_DIR%" (
    echo Removing previous build directory...
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

echo.
echo Step 4: CMake Configuration...
cmake "%PROJECT_ROOT%" ^
    -G "!VS_GENERATOR!" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_CONFIGURATION_TYPES="Release"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

echo.
echo Step 5: Build Project...
cmake --build . --config Release --parallel

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo Step 6: Verify Build Results...
if exist "bin\Release\chtl.exe" (
    echo SUCCESS: chtl.exe created
    echo File size:
    dir "bin\Release\chtl.exe" | findstr "chtl.exe"
    
    echo.
    echo Testing compiler:
    "bin\Release\chtl.exe" --version
) else (
    echo ERROR: chtl.exe not found
    exit /b 1
)

echo.
echo Step 7: Package Creation...
cmake --build . --target package --config Release

echo.
echo ===============================================
echo SUCCESS: CHTL Compiler Release Build Complete
echo.
echo Output Directory: %BUILD_DIR%\bin\Release\
echo Executable: chtl.exe
echo Package: CHTL-Compiler-1.0.0-win64.zip
echo.
echo Features:
echo   - CJMOD powerful extension system
echo   - 100 syntax features implementation
echo   - Dual-language separation architecture
echo   - UTF-8 Chinese character support
echo   - Professional build quality
echo ===============================================

pause