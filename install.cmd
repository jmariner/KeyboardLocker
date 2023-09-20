@ECHO OFF

: path to exe in build folder, relative to this file's directory
SET BUILD_FILE=%~dp0build\KeyboardLocker.exe
: path to exe in appdata
SET DEST_FILE=%APPDATA%\KeyboardLocker\KeyboardLocker.exe

: exit if build file doesn't exist
IF NOT EXIST "%BUILD_FILE%" (
    ECHO "Build file doesn't exist: %BUILD_FILE%"
    EXIT
)

: if KeyboardLocker.exe is running, kill it
echo "Checking if KeyboardLocker.exe is running..."
tasklist /FI "IMAGENAME eq KeyboardLocker.exe" 2>NUL | find /I /N "KeyboardLocker.exe">NUL
IF "%ERRORLEVEL%"=="0" (
    echo "KeyboardLocker.exe is running, killing it..."
    taskkill /T /IM KeyboardLocker.exe
    echo "Waiting 3 seconds for KeyboardLocker.exe to exit..."
    timeout /T 3 /NOBREAK >NUL
)

: make a backup of the current KeyboardLocker.exe
IF EXIST "%DEST_FILE%" (
    echo "Backing up current KeyboardLocker.exe..."
    copy /Y "%DEST_FILE%" "%DEST_FILE%.bak"
)

: create dest dir if it doesn't exist
IF NOT EXIST "%APPDATA%\KeyboardLocker" (
    echo "Creating %APPDATA%\KeyboardLocker..."
    mkdir "%APPDATA%\KeyboardLocker"
)

: copy KeyboardLocker.exe to dest
echo "Copying KeyboardLocker.exe to %DEST_FILE%..."
copy /Y build\KeyboardLocker.exe "%DEST_FILE%"