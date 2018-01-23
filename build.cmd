@ECHO OFF

set NAME=KeyboardLocker

echo Cleaning output directory...
del out\%NAME%.* 2> NUL

echo Compiling C...
gcc -c src/%NAME%.c -o out/%name%.o

: check if compilation failed
if NOT "%errorlevel%" == "0" (
	echo.
	echo Error when compiling C. See above for details.
	exit
)

echo Converting to DLL...
gcc -shared -o out/%NAME%.dll out/%NAME%.o

: echo Killing EXE...
: taskkill /IM %NAME%.exe > NUL 2> NUL
: echo Building EXE...
: gcc -o out/%NAME%.exe src/%NAME%_runner.c -L./out -l%NAME%

build-ahk

echo Building Complete.