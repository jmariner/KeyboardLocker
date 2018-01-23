@ECHO OFF

set NAME=KeyboardLocker

echo Killing AHK EXE...
taskkill /IM %NAME%.ahk.exe > NUL 2> NUL

echo Building AHK EXE...

: ahk2exe does not actually exit after being called, so anything after this will not run.
: note to self: ahk2exe uses the registry to save previously-used values and will default to them when no value is passed.
ahk2exe /in src/%NAME%.ahk /out out/%NAME%.ahk.exe /bin lib/KeyboardLockerAHKU32.bin
