################################################################
## Makefile for James' MTX Operating System
################################################################

OS := jj_mtx
VM := qemu-system-i386
VM_FLAGS := -no-fd-bootchk

# Virtual Disk Image
DISK_DIR := ../disk
KC_IMAGE := $(DISK_DIR)/mtximage
JJ_IMAGE := $(DISK_DIR)/jj_mtximage

SHELL := /bin/bash

.PHONY : all run clean super_clean super_duper_clean debug

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
CC := bcc -c
CFLAGS := -ansi

# Assembly
SC := as86
SFLAGS :=

# Linker
LINK := ld86
LFLAGS := /usr/lib/bcc/libc.a

OBJ_DIR := obj

# Booter
BOOT_DIR := boot
BOOTER := $(BOOT_DIR)/booter
BOOT_OBJ_DIR := $(BOOT_DIR)/$(OBJ_DIR)
BOOT_OBJECTS := $(addprefix $(BOOT_OBJ_DIR)/, \
				bs.o main.o)

# Library
LIB_DIR := lib
LIB := $(LIB_DIR)/jj_lib.a
LIB_OBJ_DIR := $(LIB_DIR)/$(OBJ_DIR)
LIB_OBJECTS := $(addprefix $(LIB_OBJ_DIR)/, \
				getc.o putc.o setds.o math.o string.o io.o \
				queue.o list.o transfer.o)
# TEMPORARY
KC_LIB_DIR := $(LIB_DIR)/kclib_obj

# Users
USER_DIR := user
USER1 := $(USER_DIR)/u1
USER2 := $(USER_DIR)/u2
USERS := $(USER1) $(USER2)
USER_OBJ_DIR := $(USER_DIR)/$(OBJ_DIR)
USER1_OBJECTS := $(addprefix $(USER_OBJ_DIR)/, \
				u.o user.o u1.o crt0.o)
USER2_OBJECTS := $(addprefix $(USER_OBJ_DIR)/, \
				u.o user.o u2.o crt0.o)

# Kernel
# Assumes that Kernel is in the same directory as this Makefile
KERNEL := $(OS)
KERNEL_OBJ_DIR := $(OBJ_DIR)
KERNEL_OBJECTS := $(addprefix $(KERNEL_OBJ_DIR)/, \
				 ts.o wait.o fork_exec.o pipe.o   \
				 kernel.o int.o main.o)

# Decided to use the verbose but explicit method instead
# Specifies a list of directories that make should search
#VPATH := $(BOOT_DIR) $(LIB_DIR) $(USER_DIR)

HIDE_WARNINGS := 2>/dev/null

################################################################
## Operating System 
################################################################

all: $(KC_IMAGE) $(BOOTER) $(USERS) $(KERNEL) 
	@echo -e "$(WARN_COLOR)Replacing $(JJ_IMAGE) \
	with a fresh copy of $(KC_IMAGE)...$(NO_COLOR)"
	rm -f $(JJ_IMAGE)
	cp $(KC_IMAGE) $(JJ_IMAGE)
	
	@echo -e "$(WARN_COLOR)Dumping $(BOOTER) to \
	first block of $(JJ_IMAGE)...$(NO_COLOR)"
	dd if=$(BOOTER) of=$(JJ_IMAGE) bs=1024 count=1 conv=notrunc
	
	@echo -e "$(WARN_COLOR)Copying \
	$(OS) and $(USER) to $(JJ_IMAGE)...$(NO_COLOR)"
	sudo mount -o loop $(JJ_IMAGE) /mnt
	sudo cp $(OS) /mnt/boot/$(OS)
	sudo cp $(USER1) /mnt/bin/U1
	sudo cp $(USER2) /mnt/bin/U2
	sudo cp $(USER1) /mnt/bin/user_one
	sudo cp $(USER2) /mnt/bin/user_two
	ls -la /mnt/bin
	sudo umount /mnt

$(DISK_DIR):
	@mkdir -p -- $@

$(KC_IMAGE): | $(DISK_DIR)
	@if [ ! -f $@ ]; then \
		echo -e "$(WARN_COLOR)Downloading $(KC_IMAGE)...$(NO_COLOR)"; \
		wget http://www.eecs.wsu.edu/~cs460/samples/LAB5/mtximage; \
		mv mtximage $(KC_IMAGE); \
	fi

################################################################
## Booter
################################################################

$(BOOTER): $(BOOT_OBJECTS)
	$(LINK) $+ $(LFLAGS) -d -o $@ $(HIDE_WARNINGS)
	
	@SIZE=$$(du -b "$(BOOTER)" | cut -f 1); \
	if [ $$SIZE -le 1024 ] ; then \
		echo -ne "$(OK_COLOR)"; \
        echo -e "Booter is slim by `expr 1024 - $$SIZE` bytes"; \
		echo -ne "$(NO_COLOR)"; \
    else \
        echo -ne "$(ERROR_COLOR)"; \
        echo -e "Reduce booter size by `expr $$SIZE - 1024` bytes"; \
		echo -ne "$(NO_COLOR)"; \
    fi
	
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Made Booter"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"

$(BOOT_OBJECTS): | $(BOOT_OBJ_DIR)

$(BOOT_OBJ_DIR):
	@mkdir -p -- $@

$(BOOT_OBJ_DIR)/bs.o: $(BOOT_DIR)/bs.s
	$(SC) $< -o $@

$(BOOT_OBJ_DIR)/main.o: $(BOOT_DIR)/main.c $(BOOT_DIR)/bs.s $(BOOT_DIR)/ext2.h
	$(CC) $(CFLAGS) $< -o $@
	
################################################################
## Library
################################################################

$(LIB): $(LIB_OBJECTS) $(KC_LIB_DIR)/loader.o $(KC_LIB_DIR)/diskio.o
	ar cr $(LIB) $?

	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Made Library"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"

$(LIB_OBJECTS): | $(LIB_OBJ_DIR)

$(LIB_OBJ_DIR):
	@mkdir -p -- $@

$(LIB_OBJ_DIR)/getc.o: $(LIB_DIR)/getc.s type.h
	$(SC) $< -o $@

$(LIB_OBJ_DIR)/putc.o: $(LIB_DIR)/putc.s type.h
	$(SC) $< -o $@

$(LIB_OBJ_DIR)/setds.o: $(LIB_DIR)/setds.s type.h
	$(SC) $< -o $@
	
$(LIB_OBJ_DIR)/math.o: $(LIB_DIR)/math.c type.h
	$(CC) $(CFLAGS) $< -o $@

$(LIB_OBJ_DIR)/string.o: $(LIB_DIR)/string.c $(LIB_DIR)/math.h type.h 
	$(CC) $(CFLAGS) $< -o $@

$(LIB_OBJ_DIR)/io.o: $(LIB_DIR)/io.c type.h
	$(CC) $(CFLAGS) $< -o $@

$(LIB_OBJ_DIR)/queue.o: $(LIB_DIR)/queue.c type.h
	$(CC) $(CFLAGS) $< -o $@

$(LIB_OBJ_DIR)/list.o: $(LIB_DIR)/list.c type.h
	$(CC) $(CFLAGS) $< -o $@

$(LIB_OBJ_DIR)/transfer.o: $(LIB_DIR)/transfer.c type.h
	$(CC) $(CFLAGS) $< -o $@

################################################################
## User
################################################################

$(USER1): $(USER1_OBJECTS) $(LIB)
	$(LINK) $+ $(LFLAGS) -o $@ $(HIDE_WARNINGS)
	
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Made User1"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	
$(USER2): $(USER2_OBJECTS) $(LIB)
	$(LINK) $+ $(LFLAGS) -o $@ $(HIDE_WARNINGS)
	
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Made User2"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	
$(USER1_OBJECTS): | $(USER_OBJ_DIR)
$(USER2_OBJECTS): | $(USER_OBJ_DIR)

$(USER_OBJ_DIR):
	@mkdir -p -- $@

$(USER_DIR)/user.h: type.h $(LIB)

$(USER_OBJ_DIR)/u.o: $(USER_DIR)/u.s
	$(SC) $< -o $@

$(USER_OBJ_DIR)/crt0.o: $(USER_DIR)/crt0.c
	$(CC) $(CFLAGS) $< -o $@

$(USER_OBJ_DIR)/user.o: $(USER_DIR)/user.c $(USER_DIR)/user.h
	$(CC) $(CFLAGS) $< -o $@

$(USER_OBJ_DIR)/u1.o: $(USER_DIR)/u1.c $(USER_DIR)/user.h
	$(CC) $(CFLAGS) $< -o $@

$(USER_OBJ_DIR)/u2.o: $(USER_DIR)/u2.c $(USER_DIR)/user.h
	$(CC) $(CFLAGS) $< -o $@

################################################################
## Kernel 
################################################################

$(KERNEL): $(KERNEL_OBJECTS) $(LIB)
	$(LINK) $+ $(LFLAGS) -d -o $@ $(HIDE_WARNINGS)
	
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Made Kernel"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	
$(KERNEL_OBJECTS): | $(KERNEL_OBJ_DIR)

$(KERNEL_OBJ_DIR):
	@mkdir -p -- $@

$kernel.h: type.h $(LIB) 

$(KERNEL_OBJ_DIR)/ts.o: ts.s
	$(SC) $< -o $@

$(KERNEL_OBJ_DIR)/main.o: main.c ts.s kernel.h
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_OBJ_DIR)/wait.o: wait.c ts.s kernel.h
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_OBJ_DIR)/fork_exec.o: fork_exec.c ts.s kernel.h
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_OBJ_DIR)/pipe.o: pipe.c ts.s kernel.h
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_OBJ_DIR)/kernel.o: kernel.c ts.s kernel.h
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_OBJ_DIR)/int.o: int.c kernel.h
	$(CC) $(CFLAGS) $< -o $@

################################################################
## Make Commands 
################################################################

run: 
	@echo -e "$(WARN_COLOR)Attempting to boot $(OS) \
	from $(JJ_IMAGE) using $(VM)...$(NO_COLOR)"
	$(VM) -fda $(JJ_IMAGE) $(VM_FLAGS)
	@echo

clean:
	@echo -e "$(WARN_COLOR)Cleaning$(NO_COLOR)"
	rm -f $(JJ_IMAGE) 
	rm -f $(KERNEL_OBJ_DIR)/*.o
	rm -f $(BOOT_OBJ_DIR)/*.o
	rm -f $(LIB_OBJ_DIR)/*.o
	rm -f $(USER_OBJ_DIR)/*.o

super_clean: clean
	rm -f $(KERNEL) 
	rm -f $(BOOTER) 
	rm -f $(LIB) 	
	rm -f $(USER1) 	
	rm -f $(USER2) 	
	rm -fr $(KERNEL_OBJ_DIR)
	rm -fr $(BOOT_OBJ_DIR)
	rm -fr $(LIB_OBJ_DIR)
	rm -fr $(USER_OBJ_DIR)

super_duper_clean: super_clean
	@echo -e "$(WARN_COLOR)Warning: Deleting $(KC_IMAGE)... $(NO_COLOR)"
	rm -r $(DISK_DIR)

show_warnings:
HIDE_WARNINGS :=

debug: | show_warnings all run

debug_make: | super_clean show_warnings all

################################################################
## Make Help
################################################################
help:
	@echo 
	@echo -e "$(WARN_COLOR)Makefile Commands:"
	@echo -ne "$(OK_COLOR)"
	@echo -e "make          compiles everything"
	@echo -e "make run      runs the executable"
	@echo -e "make clean    removes all object files and the executable"
	@echo -e "make debug    cleans, compiles, and runs everything"
	@echo -e "$(NO_COLOR)"
