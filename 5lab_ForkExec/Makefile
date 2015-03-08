################################################################
## Makefile for James' MTX Operating System
################################################################

OS = jj_mtx
VM = qemu-system-i386

SHELL := /bin/bash

.PHONY : all run clean debug

################################################################
## Definitions 
################################################################

CC = bcc -c
CFLAGS = -ansi

SC = as86
SFLAGS =

LINK = ld86
LFLAGS = /usr/lib/bcc/libc.a

# Virtual Disk Image
DISK_DIR = ../disk
KC_IMAGE = $(DISK_DIR)/mtximage
JJ_IMAGE = $(DISK_DIR)/jj_mtximage

BOOT_DIR = boot
BOOTER = $(BOOT_DIR)/booter
BOOTER_OBJECTS = bs.o main.o

LIB_DIR = lib
LIB = $(LIB_DIR)/jjlib.a
LIB_OBJECTS = getc.o putc.o setds.o math.o string.o io.o \
			  queue.o list.o transfer.o

USER_DIR = user
USER1 = $(USER_DIR)/u1 
USER2 = $(USER_DIR)/u2 
USER_OBJECTS = u.o user.o u1.o u2.o

OBJ_DIR  = obj

SUB_DIRS = $(LIB_DIR) $(BOOT_DIR) $(USER_DIR)

KERNEL_OBJECTS = ts.o wait.o kernel.o vkernel.o int.o main.o

################################################################
## Colors
################################################################

NO_COLOR    = \x1b[0m
OK_COLOR    = \x1b[32;01m
WARN_COLOR  = \x1b[33;01m
ERROR_COLOR = \x1b[31;01m

################################################################
## Operating System 
################################################################

all: $(OBJ_DIR) $(KC_IMAGE) $(BOOTER) $(USER) $(KERNEL) 
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
	sudo cp user/u1 /mnt/bin/u1 
	sudo cp user/u2 /mnt/bin/u2 
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
## Booter
################################################################

$(BOOTER): | $(OBJ_DIR) $(BOOTER) 
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Making Booter"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	@$(MAKE) -C $(BOOT_DIR)
	@echo
	
	@SIZE=$$(du -b "$(BOOTER)" | cut -f 1); \
	if [ $$SIZE -le 1024 ] ; then \
		echo -ne "$(OK_COLOR)"; \
        echo -e "--------------------------------"; \
        echo -e "Booter is slim by `expr 1024 - $$SIZE` bytes"; \
        echo -e "--------------------------------"; \
		echo -ne "$(NO_COLOR)"; \
    else \
        echo -ne "$(ERROR_COLOR)"; \
        echo -e "--------------------------------"; \
        echo -e "Reduce booter size by `expr $$SIZE - 1024` bytes"; \
        echo -e "--------------------------------"; \
		echo -ne "$(NO_COLOR)"; \
    fi

$(OBJ_DIR):
	@mkdir -p -- $@

$(BOOTER): $(addprefix $(OBJ_DIR)/, $(OBJECTS))
	$(LINK) $^ $(LFLAGS) -d -o $@

$(OBJ_DIR)/bs.o: bs.s
	$(SC) $< -o $@

$(OBJ_DIR)/main.o: main.c bs.s ext2.h
	$(CC) $(CFLAGS) $< -o $@

################################################################
## Library
################################################################

all: | $(OBJ_DIR) $(LIB) 

$(OBJ_DIR):
	@mkdir -p -- $@

# Temporarily using KC's loader.o & diskio.o until I learn how
$(LIB): $(addprefix $(OBJ_DIR)/, $(OBJECTS))
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Making Library"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	@$(MAKE) -C $(LIB_DIR)
	@echo
	
	rm -f $(LIB)
	
	@echo -e "$(WARN_COLOR)Compiling $(LIB)...$(NO_COLOR)"
	ar cr $(LIB) $^
	
	@echo -e "$(WARN_COLOR)Adding KC loader to $(LIB)...$(NO_COLOR)"
	ar r $(LIB) kclib_obj/loader.o
	
	@echo -e "$(WARN_COLOR)Adding KC diskio to $(LIB)...$(NO_COLOR)"
	ar r $(LIB) kclib_obj/diskio.o

$(OBJ_DIR)/getc.o: getc.s ../type.h
	$(SC) $< -o $@

$(OBJ_DIR)/putc.o: putc.s ../type.h
	$(SC) $< -o $@

$(OBJ_DIR)/setds.o: setds.s ../type.h
	$(SC) $< -o $@
	
$(OBJ_DIR)/math.o: math.c ../type.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/string.o: string.c ../type.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/io.o: io.c ../type.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/queue.o: queue.c ../type.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/list.o: list.c ../type.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/transfer.o: transfer.c ../type.h
	$(CC) $(CFLAGS) $< -o $@

################################################################
## User
################################################################

all: | $(OBJ_DIR) $(USER1) $(USER2)

$(OBJ_DIR):
	@mkdir -p -- $@

$(USER1): $(addprefix $(OBJ_DIR)/, $(OBJECTS)) $(LIB)
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Making User"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	@$(MAKE) -C $(USER_DIR)
	@echo
	
	$(LINK) $^ $(LFLAGS) -o $@

$(USER2): $(addprefix $(OBJ_DIR)/, $(OBJECTS)) $(LIB)
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Making User"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	@$(MAKE) -C $(USER_DIR)
	@echo
	
	$(LINK) $^ $(LFLAGS) -o $@

$(OBJ_DIR)/u.o: u.s
	$(SC) $< -o $@

$(OBJ_DIR)/user.o: user.c user.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/u1.o: u1.c user.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/u2.o: u2.c user.h
	$(CC) $(CFLAGS) $< -o $@

################################################################
## Kernel 
################################################################

$(KERNEL): $(addprefix $(OBJ_DIR)/, $(OBJECTS)) $(LIB)
	@echo -ne "$(WARN_COLOR)"
	@echo "=========================================="
	@echo "Making Kernel"
	@echo "------------------------------------------"
	@echo -ne "$(NO_COLOR)"
	@$(MAKE) -C $(BOOT_DIR)
	@echo
	
	$(LINK) $^ $(LFLAGS) -o $@

$kernel.h: type.h $(LIB) 

$(OBJ_DIR)/ts.o: ts.s
	$(SC) $< -o $@

$(OBJ_DIR)/main.o: main.c ts.s kernel.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/wait.o: wait.c ts.s kernel.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/kernel.o: kernel.c ts.s kernel.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/vkernel.o: vkernel.c ts.s kernel.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/int.o: int.c kernel.h
	$(CC) $(CFLAGS) $< -o $@

################################################################
## Make Commands 
################################################################

run: 
	@echo -e "$(WARN_COLOR)Attempting to boot $(OS) \
	from $(JJ_IMAGE) using $(VM)...$(NO_COLOR)"
	$(VM) -fda $(JJ_IMAGE) -no-fd-bootchk
	@echo

clean:
	rm -f $(OS) $(JJ_IMAGE) $(OBJ_DIR)/*.o
	rm -f $(BOOTER) $(BOOT_DIR)/$(OBJ_DIR)/*.o
	rm -f $(LIB)    $(LIB_DIR)/$(OBJ_DIR)/*.o
	rm -f $(USER)   $(USER_DIR)/$(OBJ_DIR)/*.o
	@echo -e "$(WARN_COLOR)Cleaning Complete$(NO_COLOR)"

debug: clean all run

################################################################
## Make Help
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
