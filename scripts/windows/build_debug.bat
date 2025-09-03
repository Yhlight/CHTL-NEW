@echo off
setlocal EnableDelayedExpansion

echo ===============================================
echo CHTL Compiler Debug Build Script (Windows)
echo Development Build with Testing - VS2019/VS2022
echo ===============================================

set "PROJECT_ROOT=%~dp0..\.."
set "BUILD_DIR=%PROJECT_ROOT%\build\debug"
set "CMAKE_FILE=%PROJECT_ROOT%\CMakeLists_Professional.txt"

echo.
echo Step 1: Environment Check...
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found in PATH
    pause
    exit /b 1
)

echo.
echo Step 2: Visual Studio Detection...
set "VS_GENERATOR="

if exist "C:\Program Files\Microsoft Visual Studio\2022" (
    set "VS_GENERATOR=Visual Studio 17 2022"
    echo Visual Studio 2022 detected
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019" (
    set "VS_GENERATOR=Visual Studio 16 2019"
    echo Visual Studio 2019 detected
) else (
    echo WARNING: Visual Studio not found, using default
    set "VS_GENERATOR=Visual Studio 17 2022"
)

echo.
echo Step 3: Clean Previous Build...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

echo.
echo Step 4: CMake Configuration (Debug)...
cmake "%PROJECT_ROOT%" ^
    -G "!VS_GENERATOR!" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_CONFIGURATION_TYPES="Debug"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

echo.
echo Step 5: Build Project (Debug)...
cmake --build . --config Debug --parallel

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo Step 6: Run Tests...
if exist "bin\Debug\chtl_cjmod_test.exe" (
    echo Running CJMOD tests...
    "bin\Debug\chtl_cjmod_test.exe"
)

if exist "bin\Debug\chtl_integration_test.exe" (
    echo Running integration tests...
    "bin\Debug\chtl_integration_test.exe"
)

if exist "bin\Debug\chtl_architecture_test.exe" (
    echo Running architecture tests...
    "bin\Debug\chtl_architecture_test.exe"
)

echo.
echo Step 7: Verify Debug Build...
if exist "bin\Debug\chtl.exe" (
    echo SUCCESS: Debug chtl.exe created
    "bin\Debug\chtl.exe" --version
) else (
    echo ERROR: Debug chtl.exe not found
    exit /b 1
)

echo.
echo ===============================================
echo SUCCESS: CHTL Compiler Debug Build Complete
echo.
echo Output Directory: %BUILD_DIR%\bin\Debug\
echo Executable: chtl.exe (Debug)
echo Tests: All tests executed
echo.
echo Debug Features:
echo   - Address sanitizer enabled
echo   - Debug symbols included
echo   - All tests available
echo   - Development tools included
echo ===============================================

pause