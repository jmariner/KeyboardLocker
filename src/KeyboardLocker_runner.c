#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <stdbool.h>

void setLocked(bool);

int main() {

	// enable keyboard lock
	setLocked(true);

	// loop GetMessage to keep application running and accepting keyboard events
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {}
}
