/*
  xtdesktop.h
  Definitions for XtBackground and XtDesktop

  Copyright (c) 1996-2000 Dragos Acostachioaie

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

#ifndef _XTDESKTOP_H
#define _XTDESKTOP_H

#include <xtscreen.h>
#include <xobject.h>

/**
    @memo Screen surface background
*/
class XtBackground : public XObject {
 public:

  /// Constructs a new XtBackground, using the specified area and fill character
  XtBackground(XRect __r, chtype __ch);

  /// Draws the object
  virtual void Draw();

  /// Returns the current fill character
  chtype GetPattern();

  /// Sets a new fill character
  virtual void SetPattern(chtype __Pattern);

 protected:
  chtype Pattern;
};

// Palette entries
#define P_BACKGROUND_LEN	2

#define P_BACKGROUND_SURFACE	0x01

// Default palette
#define P_BACKGROUND		"\x07\x04\x07\x04"

/**
    @memo Desktop
*/
class XtDesktop : public XtBackground {
 public:

  /// Constructs a new desktop using the specified bounds
  XtDesktop(XRect __r);

  /// Draws the object
  virtual void Draw();
};

#endif
