NAME=KeyboardLocker-TouchPortal
SRC=src
OUT=out
PLUGIN_OUT=%APPDATA%\TouchPortal\plugins\$(NAME)

plugin: $(OUT)/$(NAME).exe

$(OUT)/$(NAME).exe: $(SRC)/index.js $(SRC)\plugin.json package.json
	yarn run pkg --compress GZip $(SRC)/index.js -c package.json -t node18-windows-x64 -o $(OUT)/$(NAME).exe
	copy $(SRC)\plugin.json $(OUT)\entry.tp >NUL
	copy $(SRC)\icon.png $(OUT)\icon.png >NUL

deploy: plugin
	xcopy $(OUT) "$(PLUGIN_OUT)" /Q /E /Y /I >NUL

run: $(OUT)/$(NAME).exe
	$(OUT)/$(NAME).exe

undeploy:
	taskkill /T /IM TouchPortal.exe /FI "STATUS eq RUNNING" >NUL
	rmdir /S /Q "$(PLUGIN_OUT)" >NUL

clean:
	del /Q $(OUT)\* >NUL