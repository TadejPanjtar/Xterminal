/*
  xtlib.h
  Initializing and exiting Xterminal library; internal definitions

  Copyright (c) 1997-98 Dragos Acostachioaie

  This file is part of Xterminal library.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the license, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _XTLIB_H
#define _XTLIB_H

/** Initialize the Xterminal library. Must be called before doing anything else.
*/
void XInit();

/** End the Xterminal session. Must be called before program exits.
*/
void XDone();

/** Produce debug information to Xterminal's log file. To produce something,
    the library should be configured with '--enable-debug' option.
*/
void XDebug(char *__fmt, ...);

#endif
