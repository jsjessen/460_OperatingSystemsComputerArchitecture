################################################################
## Makefile for the KERNEL of James' MTX Operating System
################################################################
BOOTER = booter
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

OBJECTS = bs.o main.o

################################################################
## Booter Compilation 
################################################################

all: | $(OBJ_DIR) $(BOOTER) 
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
	@echo -e "$(LINK) $^ $(LFLAGS) -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(LINK) $^ $(LFLAGS) -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/bs.o: bs.s
	@echo -e "$(SC) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(SC) $< -o $@
	@echo -ne "$(NO_COLOR)"

$(OBJ_DIR)/main.o: main.c bs.s ext2.h
	@echo -e "$(CC) $(CFLAGS) $< -o $@"
	@echo -ne "$(ERROR_COLOR)"
	@$(CC) $(CFLAGS) $< -o $@
	@echo -ne "$(NO_COLOR)"

################################################################
## Commands 
################################################################

clean:
	@rm -f $(BOOTER) $(OBJ_DIR)/*.o
	@echo -e "$(WARN_COLOR)Boot Clean$(NO_COLOR)"
