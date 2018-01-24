@ECHO OFF

set NAME=KeyboardLocker

echo Killing AHK EXE...
taskkill /IM %NAME%.exe >NUL 2>NUL

echo Building AHK EXE...
set IN=lib/%NAME%.ahk
set OUT=out/%NAME%.exe
set BIN=lib/%NAME%.bin

: ahk2exe does not actually exit after being called, so anything after this will not run.
: note to self: ahk2exe uses the registry to save previously-used values and will default to them when no value is passed.
ahk2exe /in %IN% /out %OUT% /bin %BIN%
