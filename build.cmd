@ECHO OFF

set NAME=KeyboardLocker

del out/%NAME%.exe 2>NUL

cd %~dp0\lib

echo Cleaning output directory...
for %%x in (dll,o) do (
	del %NAME%.%%x 2>NUL
)

echo Compiling C...
gcc -c %NAME%.c -o %NAME%.o

: check if compilation failed
if NOT "%errorlevel%" == "0" (
	echo.
	echo Error when compiling C. See above for details.
	exit
)

echo Converting to DLL...
gcc -shared -o %NAME%.dll %NAME%.o

del %NAME%.o 2> NUL

cd ..

build-ahk
