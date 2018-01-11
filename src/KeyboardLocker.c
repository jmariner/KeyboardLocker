// https://www.unknowncheats.me/forum/c-and-c-/83707-setwindowshookex-example.html
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>

#define DEBUG true
#define DISABLE_STR "UNLOCK"
#define LAST_COUNT 6
#define S_CHAR sizeof(char)

// constants
int KEY_WHITELIST[] = {
	VK_VOLUME_MUTE,
	VK_VOLUME_DOWN,
	VK_VOLUME_UP,
	VK_MEDIA_NEXT_TRACK,
	VK_MEDIA_PREV_TRACK,
	VK_MEDIA_STOP,
	VK_MEDIA_PLAY_PAUSE
};
int WHITELIST_SIZE = sizeof(KEY_WHITELIST) / sizeof(KEY_WHITELIST[0]);

// globals
bool ready = false;
HHOOK hook;
char lastKeyChars[LAST_COUNT] = { 0 }; // the last LAST_COUNT keypresses. non-chars become zero
char* lastStr;

void setBlocked(bool);
void enable();
void disable();

void init() {
	lastStr = malloc((LAST_COUNT + 1) * S_CHAR);
	ready = true;
}

void handleKeyDown(char keyChar) {
	char tempStr[LAST_COUNT];

	// copy from index 1 to the end of lastKeyChars to tempStr
	memcpy(tempStr, lastKeyChars + 1, LAST_COUNT-1);

	// add the new char to the end
	tempStr[LAST_COUNT-1] = keyChar;

	// copy it all back into lastKeyChars
	memcpy(lastKeyChars, tempStr, LAST_COUNT);

	// also copy it over to lastStr and append a null terminator to make it a string
	memcpy(lastStr, lastKeyChars, LAST_COUNT);
	lastStr[LAST_COUNT] = '\0';
}

// LowLevelKeyboardProc
LRESULT CALLBACK keyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {

	bool block = false;

	if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_KEYUP)) {

		KBDLLHOOKSTRUCT data = *((KBDLLHOOKSTRUCT*)lParam);
		char keyChar = MapVirtualKey(data.vkCode, MAPVK_VK_TO_CHAR);

		block = true;
		for (int i = 0; i < WHITELIST_SIZE; i++) {
			if (KEY_WHITELIST[i] == data.vkCode) {
				block = false;
				break;
			}
		}

		if (wParam == WM_KEYDOWN) {

			handleKeyDown(keyChar);

			if (DEBUG) {

				printf("vkCode: %03d (%#02x) char: \'%c\'\n",
					data.vkCode, data.vkCode,
					keyChar
				);

				printf("lastStr: \"%s\"\n", lastStr);
			}
		}
		else {
			if (strcmp(lastStr, DISABLE_STR) == 0)
				setBlocked(false);
		}

	}

	return block ? 1 : CallNextHookEx(hook, nCode, wParam, lParam);
}

void setBlocked(bool blockKeyboard) {

	if (!ready) init();

	if (blockKeyboard) {
		if (hook == NULL)
			hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookCallback, NULL, 0);
	}
	else if (hook != NULL) {
		UnhookWindowsHookEx(hook);
		hook = NULL;
	}
}

void enable() { setBlocked(true); }
void disable() { setBlocked(false); }