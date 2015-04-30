# James Jessen
# CptS 460 - Spring 2015

SHELL := /bin/bash

################################################################
## Makefile for James' & KC's MTX Operating System
################################################################

# Run 'make ports' launch terminals and update port tty's 
TTYS_FILE=ttys.dat
SERIAL_PORT0=$$(sed -n '1{p;q}' $(TTYS_FILE))
SERIAL_PORT1=$$(sed -n '2{p;q}' $(TTYS_FILE))
PRINTER=$$(sed -n '3{p;q}' $(TTYS_FILE))

# Virtual Disk Images
DISK_DIR := disk
KC_VDISK := $(DISK_DIR)/vdisk
JJ_VDISK := $(DISK_DIR)/jj_vdisk
CD := $(DISK_DIR)/MTXinstallCD.iso

#Virtual Machine
VM := qemu-system-i386
VM_RAM := 520 #MB
VM_CPU := 2
VM_FLAGS = -hda $(JJ_VDISK) -m $(VM_RAM)M -smp $(VM_CPU) \
			-serial $(SERIAL_PORT0) \
			-serial $(SERIAL_PORT1) \
			-parallel $(PRINTER) \
			-localtime -cdrom $(CD) #-boot d

PARTITION=1
SECTOR=$$(./ptable disk/jj_vdisk 1)
OFFSET=$$(expr $(SECTOR) \* 512)

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
LFLAGS := mtxlib /usr/lib/bcc/libc.a

INIT := init
INIT_OBJECTS := $(addprefix $(OBJ_DIR)/, \
				  u.o init.o ucode.o)

LOGIN := login
LOGIN_OBJECTS := $(addprefix $(OBJ_DIR)/, \
				   u.o login.o ucode.o util.o)

HIDE_WARNINGS := 2>/dev/null

################################################################
## Operating System 
################################################################

all: | $(CD) $(KC_VDISK) $(INIT) $(LOGIN) $(SH)
	mcp $(INIT) /bin/init
	mcp $(LOGIN) /bin/login
	$(VM) $(VM_FLAGS)

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
## Operating System 
################################################################

$(INIT): $(INIT_OBJECTS)
	$(LINK) $+ $(LFLAGS) -o $@ $(HIDE_WARNINGS)

$(LOGIN): $(LOGIN_OBJECTS)
	$(LINK) $+ $(LFLAGS) -o $@ $(HIDE_WARNINGS)

#$(SH): sh.c

$(OBJ_DIR)/u.o: u.s
	$(SC) $< -o $@

$(OBJ_DIR)/init.o: init.c ucode.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/login.o: login.c ucode.h util.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/ucode.o: ucode.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/util.o: util.c
	$(CC) $(CFLAGS) $< -o $@

################################################################
## Make Commands 
################################################################

run:
	$(VM) $(VM_FLAGS)

clean:
	@echo -e "$(WARN_COLOR)Cleaning$(NO_COLOR)"
	rm -f $(JJ_VDISK) 
	rm -f $(OBJ_DIR)/*.o

super_clean: clean
	rm -f $(INIT) 
	rm -f $(LOGIN) 
	rm -fr $(OBJ_DIR)
#rm -f $(SH) 	

ports:
	tab="--tab"; \
	cmd="bash -c 'tty >> $(TTYS_FILE); tty; echo ===========; sleep 9999999';bash"; \
	launch_port=""; \
	rm -f $(TTYS_FILE); \
	for i in 0 1 2; do \
	    launch_port+=($$tab -e "$$cmd"); \
	done; \
	gnome-terminal "$${launch_port[@]}"; \
	sleep 0.5 

print_ports:
	@echo "$(SERIAL_PORT0) = SerialPort0"
	@echo "$(SERIAL_PORT1) = SerialPort1"
	@echo "$(PRINTER) = Printer"

show_warnings:
HIDE_WARNINGS =

debug: | show_warnings all run

debug_make: | super_clean show_warnings all

################################################################
## Make Help
################################################################
help:
	@echo 
	@echo -e "$(WARN_COLOR)Makefile Commands:"
	@echo -ne "$(OK_COLOR)"
	@echo -e "make              compiles everything"
	@echo -e "make ports        opens serial ports & printer (terminals), saves ttys to file"
	@echo -e "make print_ports  see which terminal corresponds to what port/type" 
	@echo -e "make run      	runs the executable"
	@echo -e "make clean    	removes all object files and the executable"
	@echo -e "make debug    	cleans, compiles, and runs everything"
	@echo -e "$(NO_COLOR)"
