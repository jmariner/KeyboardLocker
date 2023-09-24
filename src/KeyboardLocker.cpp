// https://www.unknowncheats.me/forum/c-and-c-/83707-setwindowshookex-example.html
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <winsock2.h>
#include <windows.h>
#include <tray.hpp>
#include <czmq.h>

#define DEBUG true
#define UNLOCK_STR "UNLOCK"
#define UNLOCK_LEN 6
#define PORT 5555

// resources
#define UNLOCKED_ICON 010
#define LOCKED_ICON 011
#define LOCK_SFX 020
#define UNLOCK_SFX 021

// constants
std::vector<int> KEY_WHITELIST = {
    VK_VOLUME_MUTE, VK_VOLUME_DOWN, VK_VOLUME_UP, VK_MEDIA_NEXT_TRACK, VK_MEDIA_PREV_TRACK, VK_MEDIA_STOP, VK_MEDIA_PLAY_PAUSE};

// globals
HHOOK hook = NULL;
bool lockStatus = false;
int disableStrCount = 0;
zsock_t *publisher = NULL;
Tray::Tray *trayPtr;
std::shared_ptr<Tray::Label> trayTitleLabelPtr;
Tray::Icon unlockedIcon(LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(UNLOCKED_ICON)));
Tray::Icon lockedIcon(LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(LOCKED_ICON)));

void setLocked(bool);

// callback for key press. format: LowLevelKeyboardProc
LRESULT CALLBACK keyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {

	if (nCode != HC_ACTION)
		return CallNextHookEx(hook, nCode, wParam, lParam);

	auto &data = *(PKBDLLHOOKSTRUCT)lParam;
	bool isKeyUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
	bool blockKey = lockStatus;
	char keyChar = MapVirtualKey(data.vkCode, MAPVK_VK_TO_CHAR);

	if (blockKey) {
		// check if data.vkCode is in whitelist using find
		if (std::find(KEY_WHITELIST.begin(), KEY_WHITELIST.end(), data.vkCode) != KEY_WHITELIST.end()) {
#if DEBUG
			std::cout << "Key " << data.vkCode << (isKeyUp ? "UP" : "DOWN") << " allowed due to being on whitelist" << std::endl;
#endif
			blockKey = false;
		}
	}

	if (blockKey) {
		if (!isKeyUp) {
			if (keyChar == UNLOCK_STR[disableStrCount]) {

				disableStrCount++;

#if DEBUG
				std::cout << "Received " << keyChar << " (" << disableStrCount << "/" << UNLOCK_LEN << ") of unlock string" << std::endl;
#endif

				if (disableStrCount == UNLOCK_LEN) {
					setLocked(false);
					disableStrCount = 0;
				}
			}
			else {
				disableStrCount = 0;
			}
		}
	}
	else {
		// TODO handle Ctrl+Alt+L to enable lock
	}

	return blockKey ? 1 : CallNextHookEx(hook, nCode, wParam, lParam);
}

// enable or disable keyboard locking
void setLocked(bool lockKeyboard) {

	bool changed = lockStatus != lockKeyboard;
	lockStatus = lockKeyboard;

#if DEBUG
	std::cout << "Keyboard lock " << (lockStatus ? "enabled" : "disabled") << std::endl;
#endif

	// update tray icon
	trayPtr->setIcon(lockStatus ? lockedIcon : unlockedIcon);
	trayPtr->setTooltip(lockStatus ? "Keyboard Locked" : "Keyboard Unlocked");
	trayTitleLabelPtr.get()->setText(lockStatus ? "Locked: Type \"UNLOCK\" to Unlock" : "Unlocked: Ctrl+Alt+L to Lock");

	if (changed) {
		// play SFX
		PlaySound(MAKEINTRESOURCE(lockStatus ? LOCK_SFX : UNLOCK_SFX), GetModuleHandle(nullptr), SND_RESOURCE | SND_ASYNC);

		// update publisher if it exists
		if (publisher != NULL)
			zstr_sendx(publisher, "ChangeKeyboardLockState", lockStatus ? "true" : "false", NULL);
	}
}

int main() {

	// set up keyboard hook
	hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookCallback, NULL, 0);

	// create publisher socket on port PORT
	publisher = zsock_new(ZMQ_PUB);
	int rc = zsock_connect(publisher, "tcp://localhost:%d", PORT);
	assert(rc == 0);

#if DEBUG
	std::cout << "Publisher created on port " << PORT << std::endl;
#endif

	Tray::Tray tray("Keyboard Locker", unlockedIcon);
	trayPtr = &tray;

	// set up tray app
	trayTitleLabelPtr = tray.addEntry(Tray::Label("Keyboard Locker"));
	tray.addEntry(Tray::Separator());
	tray.addEntry(Tray::Toggle("Keyboard Lock", false, setLocked))->setDefault();
	tray.addEntry(Tray::Button("Exit", [&] { tray.exit(); }));

	setLocked(false);

	tray.run(); // this blocks execution until tray is exited

	// close publisher socket
	if (publisher != NULL)
		zsock_destroy(&publisher);

	// unhook keyboard hook
	if (hook != NULL)
		UnhookWindowsHookEx(hook);

	return 0;
}