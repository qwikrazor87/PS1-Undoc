TARGET = ps1undoc
OBJS =  src/main.o src/lib.o src/exports.o src/imports.o

LIBDIR =
LIBS =

CFLAGS = -Wall -Wextra -O2 -G0
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION=660

all:
	rm -rf EBOOT.PBP PARAM.SFO release
	mkdir release
	mkdir "release/PS1 Undoc"
	mksfo "PS1 Undoc" PARAM.SFO
	pack-pbp EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL ps1undoc.prx NULL
	cp EBOOT.PBP "release/PS1 Undoc/EBOOT.PBP"
	rm -rf EBOOT.PBP PARAM.SFO


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
