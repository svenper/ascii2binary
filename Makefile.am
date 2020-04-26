bin_PROGRAMS = binary2ascii ascii2binary
dist_man_MANS = binary2ascii.1 ascii2binary.1
binary2ascii_SOURCES = binary2ascii.c binfmt.c MachineInfo.c endian.c exitcodes.c
ascii2binary_SOURCES = ascii2binary.c GetWord.c MachineInfo.c endian.c exitcodes.c
noinst_HEADERS = exitcodes.h a2b_endian.h
LOCALEDIR = /usr/local/share/ascii2binary/locale
AM_CFLAGS=-std=gnu99
ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = dist-zip dist-bzip2
#cp -r locale $(LOCALEDIR)


