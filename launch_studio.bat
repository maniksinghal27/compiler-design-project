@echo off
echo ========================================================
echo   COMPILER STUDIO: Control-Flow Intelligence Suite
echo ========================================================
echo.

echo [1/3] Building native Flex/Bison compiler in WSL...
wsl bash -c "cd /mnt/c/Users/asmin/compiler-design-project && make clean && make"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Native compiler build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [2/3] Checking Frontend Build...
if not exist "frontend\dist\index.html" (
    echo Building React/TypeScript frontend...
    cd frontend && call npm run build && cd ..
)

echo.
echo [3/3] Launching Compiler Studio Server on http://127.0.0.1:5000...
echo Open your web browser at: http://127.0.0.1:5000
python backend\server.py
pause
