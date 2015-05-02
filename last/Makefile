# James Jessen
# CptS 460 - Spring 2015

SHELL := /bin/bash

################################################################
## Makefile for James' & KC's MTX Operating System
################################################################

# Virtual Disk Images
DISK_DIR := disk
KC_VDISK := $(DISK_DIR)/vdisk
JJ_VDISK := $(DISK_DIR)/jj_vdisk
CD := $(DISK_DIR)/MTXinstallCD.iso

# Ports
TTYS_FILE := ttys.dat
SERIAL_PORT0 := $$(sed -n '1{p;q}' $(TTYS_FILE)) # line 1
SERIAL_PORT1 := $$(sed -n '2{p;q}' $(TTYS_FILE)) # line 2
PRINTER      := $$(sed -n '3{p;q}' $(TTYS_FILE)) # line 3
#SERIAL_PORT0 := 1
#SERIAL_PORT1 := 2
#PRINTER      := 3

# Virtual Machine
VM := qemu-system-i386
VM_RAM := 520#MB
VM_CPU := 2
VM_FLAGS = -hda $(JJ_VDISK) -m $(VM_RAM)M -smp $(VM_CPU) \
		   -serial $(SERIAL_PORT0) \
		   -serial $(SERIAL_PORT1) \
		   -parallel $(PRINTER) \
		   -localtime 
			#-cdrom $(CD) -boot d #Boot from CD

.PHONY : all run clean super_clean debug debug_make

################################################################
## Colors
################################################################

NO_COLOR    := \x1b[0m
OK_COLOR    := \x1b[32;01m
WARN_COLOR  := \x1b[33;01m
ERROR_COLOR := \x1b[31;01m

################################################################
## Definitions 
################################################################

# C
CC := bcc
CFLAGS := -c -ansi

# Assembly
SC := as86
SFLAGS :=

SCRIPT_DIR := script
OBJ_DIR := obj

# Linker
LINK := ld86
LFLAGS := mtxlib /usr/lib/bcc/libc.a 2>/dev/null

INIT := init
INIT_OBJECTS := $(addprefix $(OBJ_DIR)/, \
				  u.o init.o ucode.o)

LOGIN := login
LOGIN_OBJECTS := $(addprefix $(OBJ_DIR)/, \
				   u.o login.o ucode.o util.o)
SH := sh
SH_OBJECTS := $(addprefix $(OBJ_DIR)/, \
				   u.o sh.o ucode.o util.o)

PARTITION=1
SECTOR=$$(./ptable $(JJ_VDISK) $(PARTITION))
OFFSET=$$(expr $(SECTOR) \* 512)

################################################################
## Arrange Files 
################################################################


all: | $(CD) $(KC_VDISK) $(OBJ_DIR) $(INIT) $(LOGIN) $(SH)
	@cp $(KC_VDISK) $(JJ_VDISK); \
	echo "partition=$(PARTITION) sector=$(SECTOR) offset=$(OFFSET)"; \
	sudo mount -o loop,offset=$(OFFSET) $(JJ_VDISK) /mnt; \
	sudo cp -av $(INIT) /mnt/bin/$(INIT); \
	sudo cp -av $(LOGIN) /mnt/bin/$(LOGIN); \
	sudo cp -av $(SH) /mnt/bin/$(SH); \
	sudo cp -av passwd /mnt/etc/passwd; \
	sudo mkdir /mnt/user/james; \
	sudo umount /mnt

$(OBJ_DIR):
	@mkdir -p -- $@

$(DISK_DIR):
	@mkdir -p -- $@

$(CD): | $(DISK_DIR)
	@if [ ! -f $@ ]; then \
		echo -e "$(WARN_COLOR)Downloading $@...$(NO_COLOR)"; \
		wget http://www.eecs.wsu.edu/~cs460/samples/LAST/MTXinstallCD.iso; \
		mv MTXinstallCD.iso $(CD); \
	fi

$(KC_VDISK): | $(DISK_DIR)
	@if [ ! -f $@ ]; then \
		echo -e "$(WARN_COLOR)Downloading $@...$(NO_COLOR)"; \
		wget http://www.eecs.wsu.edu/~cs460/samples/LAST/vdisk; \
		mv vdisk $(KC_IMAGE); \
	fi

################################################################
## Compile 
################################################################

$(OBJ_DIR)/u.o: u.s
	$(SC) $< -o $@

$(OBJ_DIR)/init.o: init.c ucode.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/login.o: login.c ucode.h util.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/sh.o: sh.c ucode.h util.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/ucode.o: ucode.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/util.o: util.c
	$(CC) $(CFLAGS) $< -o $@

################################################################
## Link 
################################################################

$(INIT): $(INIT_OBJECTS)
	$(LINK) $+ $(LFLAGS) -o $@

$(LOGIN): $(LOGIN_OBJECTS)
	$(LINK) $+ $(LFLAGS) -o $@

$(SH): $(SH_OBJECTS)
	$(LINK) $+ $(LFLAGS) -o $@

################################################################
## Ports 
################################################################

$(TTYS_FILE):
	@if [ ! -f $@ ]; then \
		tab="--tab"; \
		cmd="bash -c 'tty >> $(TTYS_FILE); tty; echo ===========; sleep 9999999';bash"; \
		launch_port=""; \
		rm -f $(TTYS_FILE); \
		for i in 0 1 2; do \
		launch_port+=($$tab -e "$$cmd"); \
		done; \
		gnome-terminal "$${launch_port[@]}"; \
		sleep 0.5; \
	fi

rm_ttys:
	rm -f  $(TTYS_FILE) 

################################################################
## Make Commands 
################################################################

run: $(TTYS_FILE)
	@echo "$(VM) $(VM_FLAGS)"; \
	$(VM) $(VM_FLAGS)

port: | rm_ttys $(TTYS_FILE)

clean:
	@echo -e "$(WARN_COLOR)Cleaning$(NO_COLOR)"
	rm -fr $(OBJ_DIR)
	rm -f  $(JJ_VDISK) 
	rm -f  $(INIT) 
	rm -f  $(LOGIN) 
	#rm -f  $(TTYS_FILE) 

debug: | clean all run

q: | all run

################################################################
## Make Help
################################################################
help:
	@echo 
	@echo -e "$(WARN_COLOR)Makefile Commands:"
	@echo -ne "$(OK_COLOR)"
	@echo -e "make          compile everything"
	@echo -e "make run      run the virtual machine"
	@echo -e "make port     launch new terminals for viewing ports 
	@echo -e "make clean    remove all object files and the executable"
	@echo -e "make debug    cleans, compiles, and runs everything"
	@echo -e "make q        compiles and runs everything"
	@echo -e "$(NO_COLOR)"
