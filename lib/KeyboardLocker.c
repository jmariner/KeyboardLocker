// https://www.unknowncheats.me/forum/c-and-c-/83707-setwindowshookex-example.html
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

#define DEBUG false
#define UNLOCK_STR "UNLOCK"
#define UNLOCK_LEN 6

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
typedef void (*update_callback)(int);

// globals
HHOOK hook = NULL;
bool lockStatus = false;
int disableStrCount = 0;
update_callback onUpdate = NULL;

void setLocked(bool);
void setCallback(update_callback);

void handleKeyDown(char keyChar) {
	if (keyChar == UNLOCK_STR[disableStrCount]) {

		#if DEBUG
			printf("Unlock key pressed: '%c'\n", keyChar);
		#endif

		disableStrCount++;

		if (disableStrCount == UNLOCK_LEN) {
			setLocked(false);
			disableStrCount = 0;
		}

	}
	else {
		disableStrCount = 0;
	}
}

// callback for key press. format: LowLevelKeyboardProc
LRESULT CALLBACK keyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {

	// by default allow everything so only the correct keydown and keyup are blocked
	bool whitelisted = true;

	if (lockStatus) {
		if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_KEYUP)) {

			KBDLLHOOKSTRUCT data = *((KBDLLHOOKSTRUCT*)lParam);
			char keyChar = MapVirtualKey(data.vkCode, MAPVK_VK_TO_CHAR);

			whitelisted = false;
			for (int i = 0; i < WHITELIST_SIZE; i++) {
				if (KEY_WHITELIST[i] == data.vkCode) {
					whitelisted = true;
					break;
				}
			}

			if (wParam == WM_KEYDOWN) {

				#if DEBUG
					printf("code: %3d (%#02x) key: '%c' blocked: %s\n",
						data.vkCode, data.vkCode,
						keyChar == '\r' || keyChar == '\b' ? '\0' : keyChar,
						whitelisted ? "false" : "true"
					);
				#endif

				handleKeyDown(keyChar);
			}

		}
	}
	else {
		// This is here to see if the keyboard unhooking is ever too slow to be
		// ready for the next key press.
		#if DEBUG
			printf("Keyboard input received while disabled.");
		#endif
	}

	return whitelisted ? CallNextHookEx(hook, nCode, wParam, lParam) : 1;
}

// enable or disable keyboard locking
void setLocked(bool lockKeyboard) {

	lockStatus = lockKeyboard;

	#if DEBUG
		printf("Keyboard lock %sabled.\n", lockStatus ? "en" : "dis");
	#endif

	if (lockStatus) {
		if (hook == NULL)
			hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookCallback, NULL, 0);
	}
	else if (hook != NULL) {
		UnhookWindowsHookEx(hook);
		hook = NULL;
	}

	if (onUpdate != NULL)
		(*onUpdate)( (int) lockStatus );
}

// function to set the update callback
void setCallback(update_callback updateCb) {
	onUpdate = updateCb;
}
