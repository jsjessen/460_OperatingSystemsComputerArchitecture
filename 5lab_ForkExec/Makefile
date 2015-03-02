################################################################
## Makefile for the KERNEL of James' MTX Operating System
################################################################
OS = my_mtx
VM = qemu-system-i386

SHELL := /bin/bash

.PHONY : all run q clean debug
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
CC = bcc -c
CFLAGS = -ansi

SC = as86
SFLAGS =

LINK = ld86
LFLAGS = /usr/lib/bcc/libc.a -d

# Virtual Disk Image
DISK_DIR = disk
KC_IMAGE = $(DISK_DIR)/mtximage
MY_IMAGE = $(DISK_DIR)/my_mtximage

BOOT_DIR = boot
BOOTER = $(BOOT_DIR)/booter

LIB_DIR = lib
LIB = $(LIB_DIR)/mylib.a

USER_DIR = user
USER = $(USER_DIR)/u1

OBJ_DIR  = obj

SUB_DIRS = $(LIB_DIR) $(BOOT_DIR) $(USER_DIR)

OBJECTS = ts.o wait.o kernel.o vkernel.o int.o main.o

################################################################
## File Preperations
################################################################

all: $(OBJ_DIR) $(KC_IMAGE) | $(BOOTER) $(USER) printOS $(OS) 
	@echo -e "$(WARN_COLOR)Replacing $(MY_IMAGE) \
	with a fresh copy of $(KC_IMAGE)...$(NO_COLOR)"
	rm -f $(MY_IMAGE)
	cp $(KC_IMAGE) $(MY_IMAGE)
	
	@echo -e "$(WARN_COLOR)Dumping $(BOOTER) to \
	first block of $(MY_IMAGE)...$(NO_COLOR)"
	dd if=$(BOOTER) of=$(MY_IMAGE) bs=1024 count=1 conv=notrunc
	
	@echo -e "$(WARN_COLOR)Copying \
	$(OS) and $(USER) to $(MY_IMAGE)...$(NO_COLOR)"
	sudo mount -o loop $(MY_IMAGE) /mnt
	sudo cp $(OS) /mnt/boot/$(OS)
	sudo cp user/u1 /mnt/bin/u1 
	sudo umount /mnt

$(OBJ_DIR):
	@mkdir -p -- $@

$(DISK_DIR):
	@mkdir -p -- $@

$(KC_IMAGE): $(DISK_DIR)
	@if [ ! -f $@ ]; then \
		echo -e "$(WARN_COLOR)Downloading $(KC_IMAGE)...$(NO_COLOR)"; \
		wget http://www.eecs.wsu.edu/~cs460/samples/LAB5/mtximage; \
		mv mtximage $(KC_IMAGE); \
	fi

################################################################
## Make Booter, Library, User 
################################################################

$(BOOTER):
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Making Booter"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	@$(MAKE) -C $(BOOT_DIR)
	@echo

$(LIB):
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Making Library"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	@$(MAKE) -C $(LIB_DIR)
	@echo

$(USER): $(LIB)
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Making User"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	@$(MAKE) -C $(USER_DIR)
	@echo

################################################################
## OS Compilation 
################################################################

$(OS): $(addprefix $(OBJ_DIR)/, $(OBJECTS)) $(LIB)
	@echo -e "$(LINK) $^ $(LFLAGS) -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(LINK) $^ $(LFLAGS) -o $@
	@echo -ne "$(NO_COLOR)"

$kernel.h: type.h $(LIB) 

$(OBJ_DIR)/ts.o: ts.s
	@echo -e "$(SC) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(SC) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/main.o: main.c ts.s kernel.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/wait.o: wait.c ts.s kernel.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/kernel.o: kernel.c ts.s kernel.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/vkernel.o: vkernel.c ts.s kernel.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/int.o: int.c kernel.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

################################################################
## Commands 
################################################################

run: 
	@echo -e "$(WARN_COLOR)Attempting to boot $(OS) \
	from $(MY_IMAGE) using $(VM)...$(NO_COLOR)"
	$(VM) -fda $(MY_IMAGE) -no-fd-bootchk
	@echo

q: all run

clean:
	@rm -f $(OS) $(MY_IMAGE) $(BOOTER) $(LIB) $(OBJ_DIR)/*.o
	@cd $(BOOT_DIR) && $(MAKE) clean
	@cd $(LIB_DIR)  && $(MAKE) clean
	@cd $(USER_DIR) && $(MAKE) clean
	@echo -e "$(WARN_COLOR)Cleaning Complete$(NO_COLOR)"

debug: clean all run

printOS:
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Making OS"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"

################################################################
## Help
################################################################
help:
	@echo 
	@echo -e "$(WARN_COLOR)Makefile Commands:"
	@echo -ne "$(OK_COLOR)"
	@echo -e "make          compiles everything"
	@echo -e "make run      runs the executable"
	@echo -e "make q        compiles and runs everything"
	@echo -e "make clean    removes all object files and the executable"
	@echo -e "make debug    cleans, compiles, and runs everything"
	@echo -e "$(NO_COLOR)"
