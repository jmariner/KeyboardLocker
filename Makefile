NAME=KeyboardLocker
CC=gcc
APP_DIR=$(realpath ./lib)
OUT_DIR=$(realpath ./out)

exe: $(OUT_DIR)/$(NAME).exe

$(OUT_DIR)/$(NAME).exe: $(APP_DIR)/$(NAME).dll $(APP_DIR)/$(NAME).ahk $(APP_DIR)/$(NAME).bin
	ahk2exe /in $(APP_DIR)/$(NAME).ahk /out $(OUT_DIR)/$(NAME).exe /bin $(APP_DIR)/$(NAME).bin

$(APP_DIR)/$(NAME).dll: $(OUT_DIR)/$(NAME).o
	$(CC) -shared -o $(APP_DIR)/$(NAME).dll $(OUT_DIR)/$(NAME).o

$(OUT_DIR)/$(NAME).o: $(APP_DIR)/$(NAME).c
	$(CC) -c $(APP_DIR)/$(NAME).c -o $(OUT_DIR)/$(NAME).o
