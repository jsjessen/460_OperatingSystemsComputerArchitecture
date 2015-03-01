################################################################
## Makefile for the KERNEL of James' MTX Operating System
################################################################

OS = jj_mtx
VM = qemu-system-i386

SHELL := /bin/bash
.PHONY : all run q clean
################################################################
## Colors
################################################################
NO_COLOR    = \x1b[0m
OK_COLOR    = \x1b[32;01m
WARN_COLOR  = \x1b[33;01m
ERROR_COLOR = \x1b[31;01m

################################################################
## Definitions 
################################################################
AS86 =as86 -0
LD86 =ld86 -0

CC = bcc -c
CFLAGS = -ansi

SC = $(AS86)
SFLAGS = 

LINK = $(LD86)
LFLAGS = -d /usr/lib/bcc/libc.a

# Virtual Disk Image
DISK_DIR = disk
KC_IMAGE = $(DISK_DIR)/mtximage
MY_IMAGE = $(DISK_DIR)/my_mtximage

BOOT_DIR = boot
BOOTER = $(BOOT_DIR)/booter

LIB_DIR = lib
LIB = $(LIB_DIR)/mylib.a

USER_DIR = user

CUR_DIR  = .
OBJ_DIR  = obj

SUB_DIRS = $(LIB_DIR) $(BOOT_DIR) $(USER_DIR)

OBJECTS = ts.o wait.o kernel.o vkernel.o int.o main.o

################################################################
## Compilation 
################################################################

all: clean $(SUB_DIRS) $(OBJ_DIR) $(OS)
	@echo -e "$(WARN_COLOR)Dumping $(BOOTER) to first block of $(MY_IMAGE)...$(NO_COLOR)"
	dd if=$(BOOTER) of=$(MY_IMAGE) bs=1024 count=1 conv=notrunc
	
	@echo -e "$(WARN_COLOR)Mounting $(MY_IMAGE)...$(NO_COLOR)$(NO_COLOR)"
	sudo mount -o loop $(MY_IMAGE) /mnt
	
	@echo -e "$(WARN_COLOR)Copying $(OS) to $(MY_IMAGE)/boot/$(OS)...$(NO_COLOR)"
	sudo cp $(OS) /mnt/boot/$(OS)
	
	@echo -e "$(WARN_COLOR)Copying u1 to $(MY_IMAGE)/bin/u1...$(NO_COLOR)"
	sudo cp user/u1 /mnt/bin/u1 
	
	@echo -e "$(WARN_COLOR)Unmounting $(my_image)...$(NO_COLOR)"
	sudo umount /mnt

# Make obj directory if it doesn't exist
$(OBJ_DIR):
	@mkdir -p -- $@

# Compile dependent makefiles 
$(SUB_DIRS):
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "$@"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	@$(MAKE) -C $@

$(OS): $(addprefix $(OBJ_DIR)/, $(OBJECTS))
	@echo -e "$(OK_COLOR)$(LINK) $^ $(LIB) $(LFLAGS) -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(LINK) $^ $(LIB) $(LFLAGS) -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/ts.o: ts.s
	@echo -e "$(OK_COLOR)$(SC) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(SC) $< -o $@
	@echo -ne "$(NO_COLOR)"

kernel.h: $(SUB_DIRS) 

$(OBJ_DIR)/main.o: main.c ts.s kernel.h type.h
	@echo -e "$(OK_COLOR)$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/wait.o: wait.c ts.s kernel.h type.h
	@echo -e "$(OK_COLOR)$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/kernel.o: kernel.c ts.s kernel.h type.h
	@echo -e "$(OK_COLOR)$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/vkernel.o: vkernel.c ts.s kernel.h type.h
	@echo -e "$(OK_COLOR)$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/int.o: int.c kernel.h type.h
	@echo -e "$(OK_COLOR)$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

################################################################
## Commands 
################################################################

run: all
	@echo -e "$(WARN_COLOR)Attempting to boot $(OS) from $(MY_IMAGE) using $(VM)...$(NO_COLOR)"
	@$(VM) -fda $(MY_IMAGE) -no-fd-bootchk #-localtime -serial mon:stdio

q: all run

clean:
	@$(RM) $(OS) $(OBJ_DIR)/*.o

################################################################
## Help
################################################################
help:
	@echo 
	@echo -e "$(WARN_COLOR)Makefile Commands:"
	@echo -ne "$(OK_COLOR)"
	@echo -e "make          compiles everything"
	@echo -e "make run      runs the executable"
	@echo -e "make q        compiles and runs"
	@echo -e "make clean    removes all object files and the executable"
	@echo -e "$(NO_COLOR)"
