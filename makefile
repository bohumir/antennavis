###############################################################################
###############################################################################
##                                                                           ##
##  Antenna Visualization Toolkit                                            ##
##                                                                           ##
##  Copyright (C) 1998 Adrian Agogino, Ken Harker                            ##
##  Copyright (C) 2005 Joop Stakenborg                                       ##
##                                                                           ##
###############################################################################
###############################################################################
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU Library General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
##


# The C compiler
CC = cc

# Compiler options
COPTS = -c -g -ansi -Wall

# Shared library linker command
SHLINK = cc

# Name for shared libs
#SH_LIB = libtogl.so.1.3
SH_LIB = 

# Where to find tcl.h, tk.h, OpenGL/Mesa headers, etc
INCLUDES = -I/usr/include/tcl8.4 -I/usr/include/tk8.4

# Where to find libtcl.a, libtk.a, OpenGL/Mesa libraries:
LIBDIRS = -L/usr/lib

# Libraries to link with
LIBS = -ltk8.4 -ltcl8.4 -lGLU -lGL -L/usr/X11R6/lib -lX11 -lXmu -lXext -lm

TK_FLAGS =

CFLAGS = $(COPTS) $(INCLUDES) $(TK_FLAGS)

LFLAGS = $(LIBDIRS)

HEADERS = TkAntenna.h ParseArgs.h ant.h pcard.h VisField.h togl.h
OBJS    = TkAntenna.o AntennaWidget.o ParseArgs.o togl.o ant.o pcard.o \
	VisField.o VisWires.o

default: TkAnt

# TkAnt
TkAnt: TkAntenna.o AntennaWidget.o ParseArgs.o ant.o pcard.o \
	VisField.o VisWires.o togl.o $(HEADERS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -f core AntennaWidget.o ParseArgs.o TkAntenna.o ant.o pcard.o \
	VisField.o VisWires.o togl.o TkAnt *~ *-stamp

install:
	install -s TkAnt $(DESTDIR)/usr/bin
	install antenna.tcl $(DESTDIR)/usr/bin

###############################################################################
###############################################################################
##                                                                           ##
##                            End of makefile                                ##
##                                                                           ##
###############################################################################
###############################################################################




