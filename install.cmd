@ECHO OFF

: path to output folder, relative to this file's directory
SET BUILD_DIR=%~dp0build\out\
: path to app folder in appdata
SET DEST_DIR=%APPDATA%\KeyboardLocker\

: exit if no files in output folder
IF NOT EXIST "%BUILD_DIR%" (
    echo "No files in %BUILD_DIR%, exiting..."
    exit
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
SET DEST_EXE=%DEST_DIR%KeyboardLocker.exe
IF EXIST "%DEST_EXE%" (
    echo "Backing up current KeyboardLocker.exe..."
    copy /Y "%DEST_EXE%" "%DEST_EXE%.bak"
)

: create dest dir if it doesn't exist
IF NOT EXIST "%DEST_DIR%" (
    echo "Creating %DEST_DIR%..."
    mkdir "%DEST_DIR%"
)

: copy output files to dest
echo "Copying output files to to %DEST_DIR%..."
xcopy "%BUILD_DIR%" "%DEST_DIR%" /Q /E /Y /I