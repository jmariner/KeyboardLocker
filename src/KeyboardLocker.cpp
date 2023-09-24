// https://www.unknowncheats.me/forum/c-and-c-/83707-setwindowshookex-example.html
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <winsock2.h>
#include <windows.h>
#include <tray.hpp>
#include <czmq.h>

#define DEBUG false
#define UNLOCK_STR "UNLOCK"
#define UNLOCK_LEN 6
#define DEF_PORT 5555

// resources
#define UNLOCKED_ICON 010
#define LOCKED_ICON 011
#define LOCK_SFX 020
#define UNLOCK_SFX 021

// constants
std::vector<int> KEY_WHITELIST = {
    VK_VOLUME_MUTE, VK_VOLUME_DOWN, VK_VOLUME_UP, VK_MEDIA_NEXT_TRACK, VK_MEDIA_PREV_TRACK, VK_MEDIA_STOP, VK_MEDIA_PLAY_PAUSE};

// globals
bool running = true;
HHOOK hook = NULL;
bool initialized = false;
bool lockStatus = false;
int disableStrCount = 0;
int activeModifierKeys = 0;
bool lockKeybindPressed = false;
zsock_t *outSocket = NULL;
Tray::Tray *trayPtr;
std::shared_ptr<Tray::Label> trayTitleLabelPtr;
std::shared_ptr<Tray::Toggle> trayMainTogglePtr;
Tray::Icon unlockedIcon(LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(UNLOCKED_ICON)));
Tray::Icon lockedIcon(LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(LOCKED_ICON)));

enum ModifierKeys { Alt = 1, Control = 2, Shift = 4, Win = 8 };

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
		// handle Ctrl+Alt+L to enable lock

		// update active modifier keys
		if (data.vkCode == VK_LMENU || data.vkCode == VK_RMENU) {
			if (isKeyUp)
				activeModifierKeys &= ~Alt;
			else
				activeModifierKeys |= Alt;
		}
		else if (data.vkCode == VK_LCONTROL || data.vkCode == VK_RCONTROL) {
			if (isKeyUp)
				activeModifierKeys &= ~Control;
			else
				activeModifierKeys |= Control;
		}
		else if (data.vkCode == VK_LSHIFT || data.vkCode == VK_RSHIFT) {
			if (isKeyUp)
				activeModifierKeys &= ~Shift;
			else
				activeModifierKeys |= Shift;
		}
		else if (data.vkCode == VK_LWIN || data.vkCode == VK_RWIN) {
			if (isKeyUp)
				activeModifierKeys &= ~Win;
			else
				activeModifierKeys |= Win;
		}

		// check if Ctrl+Alt+L is pressed
		if (activeModifierKeys == (Control | Alt) && data.vkCode == 'L' && !isKeyUp) {
			lockKeybindPressed = true;
		}

		// check if Ctrl+Alt+L are all released.
		// TODO this will trigger while L is still pressed if Ctrl or Alt are released first. fix this if it becomes an issue
		if (activeModifierKeys == 0 && lockKeybindPressed) {
			lockKeybindPressed = false;
			setLocked(true);
		}
	}

	return blockKey ? 1 : CallNextHookEx(hook, nCode, wParam, lParam);
}

// enable or disable keyboard locking
void setLocked(bool lockKeyboard) {

	bool changed = lockStatus != lockKeyboard;
	lockStatus = lockKeyboard;

	// update tray icon
	trayPtr->setIcon(lockStatus ? lockedIcon : unlockedIcon);
	trayPtr->setTooltip(lockStatus ? "Keyboard Locked" : "Keyboard Unlocked");
	trayTitleLabelPtr.get()->setText(lockStatus ? "Locked: Type \"UNLOCK\" to Unlock" : "Unlocked: Ctrl+Alt+L to Lock");
	trayMainTogglePtr.get()->setToggledNoCallback(lockStatus);

	if (changed) {
		// play SFX
		PlaySound(MAKEINTRESOURCE(lockStatus ? LOCK_SFX : UNLOCK_SFX), GetModuleHandle(nullptr), SND_RESOURCE | SND_ASYNC);
	}

	if (changed || !initialized) {
		// update publisher if it exists
		if (outSocket != NULL) {
			zstr_sendx(outSocket, "ChangeKeyboardLockState", lockStatus ? "true" : "false", NULL);
		}
	}

#if DEBUG
	std::cout << "Keyboard lock " << (lockStatus ? "enabled" : "disabled") << std::endl;
#endif
}

void doExit() {
	trayPtr->exit();
	running = false;
}

int main(int argc, char **argv) {

	ShowWindow(GetConsoleWindow(), SW_HIDE);

	// set up keyboard hook
	hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookCallback, NULL, 0);

	// parse args until -p or --port is found, then read next arg as 2 comma-separated port numbers
	int outPort = DEF_PORT;
	int inPort = -1;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
			if (i + 1 < argc) {
				char *portStr = argv[i + 1];
				char *port2Str = strchr(portStr, ',');
				if (port2Str != NULL) {
					*port2Str = '\0';
					port2Str++;
					inPort = atoi(port2Str);
				}
				outPort = atoi(portStr);
			}
			break;
		}
	}

	if (inPort == -1)
		inPort = outPort + 1;

	// create output socket on port outPort
	outSocket = zsock_new(ZMQ_REQ);
	int rc = zsock_connect(outSocket, "tcp://localhost:%d", outPort);
	assert(rc == 0);

#if DEBUG
	std::cout << "Input socket created on port " << outPort << std::endl;
#endif

	// create input socket on port inPort
	zsock_t *inSocket = zsock_new(ZMQ_REP);
	rc = zsock_bind(inSocket, "tcp://localhost:%d", inPort);
	assert(rc == inPort);

#if DEBUG
	std::cout << "Output socket created on port " << inPort << std::endl;
#endif

	Tray::Tray tray("Keyboard Locker", unlockedIcon);
	trayPtr = &tray;

	// set up tray app
	trayTitleLabelPtr = tray.addEntry(Tray::Label("Keyboard Locker"));
	tray.addEntry(Tray::Separator());
	trayMainTogglePtr = tray.addEntry(Tray::Toggle("Keyboard Lock", false, setLocked));
	tray.addEntry(Tray::Button("Exit", doExit));
	trayMainTogglePtr->setDefault();
	setLocked(false);

	// set up poller for both in and out sockets
	zpoller_t *poller = zpoller_new(inSocket, outSocket, NULL);

	initialized = true;

	while (running) {
		tray.pump();

		// check for messages via poller
		void *socket = zpoller_wait(poller, 0);
		if (socket == inSocket) {
			char *command;
			char *state;
			if (zstr_recvx(socket, &command, &state, NULL) > -1) {
				if (strcmp(command, "ChangeKeyboardLockState") == 0) {
					// set locked depending on state, which can be Toggle, Enable, or Disable, using a ternary operator
					setLocked(strcmp(state, "Toggle") == 0 ? !lockStatus : strcmp(state, "Enable") == 0);
				}

				zstr_free(&command);
				zstr_free(&state);
			}

			zstr_send(inSocket, "OK");
		}
		else if (socket == outSocket) {
			char *msg = zstr_recv(socket);

#if DEBUG
			// log reply message
			std::cout << "Reply: " << msg << std::endl;
#endif

			zstr_free(&msg);
		}
	}

	// clean up
	zpoller_destroy(&poller);
	zsock_destroy(&inSocket);
	zsock_destroy(&outSocket);
	UnhookWindowsHookEx(hook);

#if DEBUG
	// print exiting
	std::cout << "Exiting" << std::endl;
#endif

	return 0;
}