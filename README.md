## A simple keyboard locking program written in C and AutoHotkey

`lib\KeyboardLocker.bin` is a modified AHK Unicode 32-bit interpreter BIN file that contains overridden and custom icons for this program. This removes the need to distribute any *.ico* files with this software. Note that, for development purposes, the icons have been included in the ico folder.

* Running `build.cmd` will compile C to an object file, convert that to a `dll`, delete the `o` file, and compile the AutoHotkey code by calling `build-ahk.cmd`.
* The AutoHotkey build script passes the AHK file and the BIN file to `ahk2exe`, which is packaged with the AutoHotkey installation and is assumed to be in the PATH.
* The end result is an EXE in the `out/` directory that includes the icons and DLL all in one.

### Download [from GitHub releases][1].

[1]: https://github.com/jmariner/KeyboardLocker/releases/latest
