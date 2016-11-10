SHELL		:= bash

export BINNAME := fdshooter

LIBDIR	:= MYSDLlib
GAMEDIR := .
MKFILE	:= Makefile


ifeq ($(OS),Windows_NT)
	MKFILE := Makefile.win
else
	MKFILE := Makefile.osx
endif


.PHONY: all clean

all: library game

library:
	@echo "====================================================="
	@echo "Building dependencies"
	@echo "====================================================="
	$(MAKE) -C $(LIBDIR)/source -f $(MKFILE)


game:
	@echo "====================================================="
	@echo "Building Game"
	@echo "====================================================="
	$(MAKE) -C $(GAMEDIR)/source -f $(MKFILE)



clean:
	@echo "====================================================="
	@echo "Cleaning Dependencies"
	@echo "====================================================="
	$(MAKE) -C $(LIBDIR)/source -f $(MKFILE) clean

	@echo "====================================================="
	@echo "Cleaning Game"
	@echo "====================================================="
	$(MAKE) -C $(GAMEDIR)/source -f $(MKFILE) clean

