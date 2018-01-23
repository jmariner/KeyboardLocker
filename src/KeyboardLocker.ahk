#Warn
#Persistent
Init()

Init() {

	global lockStatus := 0
	try {
		global keyboardLockerDll := DllCall("LoadLibrary", "str", "KeyboardLocker.dll", "ptr")
	} catch {
		MsgBox % "KeyboardLocker DLL was not found. Closing App."
		ExitApp
	}

	; icons are embedded into the custom BIN file used to compile this AHK file
	global lockedIconHandle := LoadPicture(A_ScriptName, "Icon6 16w 16h", IMAGE_ICON)
	global unlockedIconHandle := LoadPicture(A_ScriptName, "Icon7 16w 16h", IMAGE_ICON)

	CallFunc("setCallback", "ptr", RegisterCallback("OnUpdate", "", 1))

	; set up tray icon
	Menu, Tray, NoStandard
	Menu, Tray, Add, Keyboard lock, ToggleLock
	Menu, Tray, Add, Exit, Exit

	OnUpdate(0)

	TrayTip, KeyboardLocker, KeyboardLocker running..., 3, 1

}

; control - alt - L
^!l::
	; don't block control-alt-l up
	KeyWait, Ctrl
	KeyWait, Alt
	KeyWait, l
	; lock keyboard
	CallFunc("setLocked", "int", 1)
return

ToggleLock() {
	global lockStatus
	CallFunc("setLocked", "int", !lockStatus)
}

; try to call the DLL function funcName and quit on failure
CallFunc(funcName, params*) {
	try {
		params.Insert("Cdecl")
		DllCall("KeyboardLocker\" . funcName, params*)
	} catch e {
		MsgBox % "Unknown error (" . e.Message . ") when calling KeyboardLocker DLL function "
			. funcName . ". Exiting App."
		ExitApp
	}
}

; when the lock status changes. called by the DLL
OnUpdate(newStatus) {

	global lockedIconHandle, unlockedIconHandle
	global lockStatus := newStatus

	; windows occasionally hangs when these happen. only noticable on unlock. it seems both the
	; tray tip and the tray icon updates can cause this. unsure about the check/uncheck updates
	if (newStatus = 1) {
		Menu, Tray, Tip, Type "UNLOCK" to unlock your keyboard
		Menu, Tray, Icon, HICON:*%lockedIconHandle%
		Menu, Tray, Check, 1&
	}
	else {
		Menu, Tray, Tip, Press Ctrl+Alt+L to lock your keyboard
		Menu, Tray, Icon, HICON:*%unlockedIconHandle%
		Menu, Tray, Uncheck, 1&
	}
}

Exit() {
	global keyboardLockerDll
	; force unlock
	CallFunc("setLocked", "int", 0)
	; free the preloaded DLL
	DllCall("FreeLibrary", "ptr", keyboardLockerDll)

	ExitApp
}
