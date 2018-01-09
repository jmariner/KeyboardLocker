#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <stdbool.h>

void enable();

int main() {
	
	enable();

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {}

	system("pause");
	return 0;
}