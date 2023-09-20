#Warn
#NoEnv
#Persistent
#SingleInstance FORCE
FileInstall, KeyboardLocker.dll, %A_Temp%\KeyboardLocker.dll, 1
try {
	Init()
} catch ex {
	MsgBox % ex.Message
	ExitApp
}

Init() {

	global lockStatus := 0, isExiting = 0
	try {
		global keyboardLockerDll := DllCall("LoadLibrary", "str", A_Temp . "\KeyboardLocker.dll", "ptr")
	} catch {
		MsgBox % "KeyboardLocker DLL was not found. Exiting App."
		ExitApp
	}

	; icons are embedded into the custom BIN file used to compile this AHK file
	exeIcon := LoadPicture(A_ScriptName, "Icon0 32w 32h", IMAGE_ICON)
	global lockedIconHandle := LoadPicture(A_ScriptName, "Icon2 16w 16h", IMAGE_ICON)
	global unlockedIconHandle := LoadPicture(A_ScriptName, "Icon3 16w 16h", IMAGE_ICON)

	CallFunc("setCallback", "ptr", RegisterCallback("OnUpdate", "", 1))

	; === set up tray icon ===

	Menu, Tray, NoStandard

	; informational first line - renamed in OnUpdate
	Menu, Tray, Add, Loading..., Noop
	Menu, Tray, Icon, 1&, HICON:*%exeIcon%
	Menu, Tray, Disable, 1&

	; separator line between info line and actual items
	Menu, Tray, Add

	; keyboard lock checkbox
	Menu, Tray, Add, Keyboard &lock, ToggleLock
	Menu, Tray, Default, 3&

	; enable main window options and add item to open that window
	Menu, Tray, MainWindow
	Menu, Tray, Add, Debug, OpenDebug

	; exit button
	Menu, Tray, Add, E&xit, Exit

	; initialize unlocked state
	OnUpdate(0)

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

GetStatus() {
	global lockStatus, lockedIconHandle, unlockedIconHandle,
	obj := {}
	obj.Tip := lockStatus ? "Keyboard Locked" : "Keyboard Unlocked"
	obj.StatusText := lockStatus ? "Locked: Type ""UNLOCK"" to unlock" : "Unlocked: Ctrl+Alt+L to lock"
	obj.Icon := "HICON:*" . (lockStatus ? lockedIconHandle : unlockedIconHandle)
	obj.CheckMode := lockStatus ? "Check" : "Uncheck"
	return obj
}

OpenDebug() {
	ListLines
}

ToggleLock() {
	global lockStatus
	CallFunc("setLocked", "int", !lockStatus)
	; lockStatus is updated to the opposite when the DLL calls the OnUpdate callback
}

; try to call the DLL function funcName and quit on failure
CallFunc(funcName, params*) {
	try {
		params.Push("Cdecl")
		DllCall("KeyboardLocker\" . funcName, params*)
	} catch e {
		MsgBox % "Unknown error (" . e.Message . ") when calling KeyboardLocker DLL function "
			. funcName . ". Exiting App."
		ExitApp
	}
}

; when the lock status changes. called by the DLL
OnUpdate(newStatus) {

	global isExiting
	global lockStatus := newStatus

	; ensure the final force-unlock call does not result in a tray menu update
	if (isExiting = 1)
		return

	stat := GetStatus()

	; windows occasionally hangs when these happen. only noticable on unlock. it seems both the
	; tray tip and the tray icon updates can cause this. unsure about the check/uncheck updates
	Menu, Tray, Tip, % stat.Tip
	Menu, Tray, Rename, 1&, % stat.StatusText
	Menu, Tray, Icon, % stat.Icon,, 1
	Menu, Tray, % stat.CheckMode, 3&

	SoundPlay, *-1
}

Noop() {
}

Exit() {
	global keyboardLockerDll
	global isExiting := 1

	; force unlock
	CallFunc("setLocked", "int", 0)
	; free the preloaded DLL
	DllCall("FreeLibrary", "ptr", keyboardLockerDll)
	; delete temp dll
	FileDelete, %A_Temp%\KeyboardLocker.dll

	ExitApp
}
