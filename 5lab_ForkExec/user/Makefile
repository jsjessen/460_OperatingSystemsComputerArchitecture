################################################################
## Makefile for the KERNEL of James' MTX Operating System
################################################################
USER = u1 
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
LFLAGS = /usr/lib/bcc/libc.a 
#Note: including the -d flag makes transition to Umode fail

LIB = ../lib/mylib.a

OBJ_DIR  = obj

OBJECTS = u.o user.o u1.o  

################################################################
## User Compilation 
################################################################

all: | $(OBJ_DIR) $(USER) 

$(OBJ_DIR):
	@mkdir -p -- $@

# Temporarily using KC's loader.o & diskio.o until I can create my own
$(USER): $(addprefix $(OBJ_DIR)/, $(OBJECTS)) $(LIB)
	@echo -e "$(LINK) $^ $(LFLAGS) -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(LINK) $^ $(LFLAGS) -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/u.o: u.s
	@echo -e "$(SC) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(SC) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/user.o: user.c user.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/u1.o: u1.c user.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

################################################################
## Commands 
################################################################

clean:
	@rm -f $(USER) $(OBJ_DIR)/*.o
	@echo -e "$(WARN_COLOR)User Clean$(NO_COLOR)"
