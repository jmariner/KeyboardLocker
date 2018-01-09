@ECHO OFF

set NAME=keyboard

gcc -c src/%NAME%.c -o out/%name%.o

if NOT "%errorlevel%" == "0" (
	pause
	exit
)

gcc -shared -o out/%NAME%.dll out/%NAME%.o

gcc -o out/%NAME%.exe src/%NAME%_runner.c -L./out -l%NAME%

echo Building Complete.