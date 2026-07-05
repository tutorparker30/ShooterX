@echo off
setlocal

REM ==========================================
REM User Settings
REM ==========================================

set UE_ROOT=C:\UnrealProject\UnrealEngine-5.7.4-release

set PROJECT_DIR=C:\UnrealProject\ShooterX
set PROJECT=%PROJECT_DIR%\ShooterX.uproject
set PROJECT_NAME=ShooterX

set LOG_DIR=%PROJECT_DIR%\Saved\Logs

REM ==========================================
REM Create Log Directory
REM ==========================================

if not exist "%LOG_DIR%" (
    mkdir "%LOG_DIR%"
)

REM ==========================================
REM Client Packaging
REM ==========================================

echo.
echo ==========================================
echo Client Packaging Start
echo ==========================================

call "%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
-project="%PROJECT%" ^
-noP4 ^
-platform=Win64 ^
-clientconfig=Development ^
-target=%PROJECT_NAME% ^
-cook ^
-map=Title+Lobby+L_Convolution_Blockout+L_Expanse ^
-build ^
-stage ^
-pak ^
-stagingdirectory="%PROJECT_DIR%\Saved\StagedBuilds\Windows" ^
> "%LOG_DIR%\PackageGame.txt" 2>&1

if errorlevel 1 (
    echo.
    echo Client Packaging Failed.
    echo See "%LOG_DIR%\PackageClient.txt"
    pause
    exit /b 1
)

echo Client Packaging Success.

REM ==========================================
REM Server Packaging
REM ==========================================

echo.
echo ==========================================
echo Server Packaging Start
echo ==========================================

call "%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
-project="%PROJECT%" ^
-noP4 ^
-platform=Win64 ^
-serverconfig=Development ^
-server ^
-servertarget=%PROJECT_NAME%Server ^
-noclient ^
-cook ^
-map=Title+Lobby+L_Convolution_Blockout+L_Expanse ^
-build ^
-stage ^
-pak ^
-stagingdirectory="%PROJECT_DIR%\Saved\StagedBuilds\WindowsServer" ^
> "%LOG_DIR%\PackageServer.txt" 2>&1

if errorlevel 1 (
    echo.
    echo Server Packaging Failed.
    echo See "%LOG_DIR%\PackageServer.txt"
    pause
    exit /b 1
)

echo Server Packaging Success.

REM ==========================================
REM Finish
REM ==========================================

echo.
echo ==========================================
echo Packaging Finished Successfully.
echo Client Log : %LOG_DIR%\PackageClient.txt
echo Server Log : %LOG_DIR%\PackageServer.txt
echo ==========================================

pause