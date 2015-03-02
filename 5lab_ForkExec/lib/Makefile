################################################################
## Makefile for the KERNEL of James' MTX Operating System
################################################################
LIB = mylib.a
SHELL := /bin/bash

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

OBJ_DIR  = obj

OBJECTS = getc.o putc.o setds.o math.o string.o io.o \
		  queue.o list.o transfer.o

################################################################
## Library Compilation 
################################################################

all: | $(OBJ_DIR) $(LIB) 

$(OBJ_DIR):
	@mkdir -p -- $@

# Temporarily using KC's loader.o & diskio.o until I can create my own
$(LIB): $(addprefix $(OBJ_DIR)/, $(OBJECTS))
	rm -f $(LIB)
	
	@echo -e "$(WARN_COLOR)Compiling $(LIB)...$(NO_COLOR)"
	ar cr $(LIB) $^
	
	@echo -e "$(WARN_COLOR)Adding KC loader to $(LIB)...$(NO_COLOR)"
	ar r $(LIB) kclib_obj/loader.o
	
	@echo -e "$(WARN_COLOR)Adding KC diskio to $(LIB)...$(NO_COLOR)"
	ar r $(LIB) kclib_obj/diskio.o

$(OBJ_DIR)/getc.o: getc.s ../type.h
	@echo -e "$(SC) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(SC) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/putc.o: putc.s ../type.h
	@echo -e "$(SC) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(SC) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/setds.o: setds.s ../type.h
	@echo -e "$(SC) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(SC) $< -o $@
	@echo -ne "$(NO_COLOR)"
	
$(OBJ_DIR)/math.o: math.c ../type.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/string.o: string.c ../type.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/io.o: io.c ../type.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/queue.o: queue.c ../type.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/list.o: list.c ../type.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/transfer.o: transfer.c ../type.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

################################################################
## Commands 
################################################################

clean:
	@rm -f $(LIB) $(OBJ_DIR)/*.o
	@echo -e "$(WARN_COLOR)Library Clean$(NO_COLOR)"
