@ECHO OFF

set NAME=KeyboardLocker

echo Killing AHK EXE...
taskkill /IM %NAME%.ahk.exe > NUL 2> NUL

echo Building AHK EXE...
ahk2exe /in src/%NAME%.ahk /out out/%NAME%.ahk.exe /icon src/%NAME%.ico /bin lib/AutoHotkeyU32.bin

echo Done