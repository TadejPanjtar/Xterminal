/*
  xtpassive.h
  Definitions for passive objects: XtStaticText, XtContextualHelp

  Copyright (c) 1996-99 Dragos Acostachioaie

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

#ifndef _XTPASSIVE_H
#define _XTPASSIVE_H

#ifdef __GNUG__
#pragma interface
#endif

#include <string>
#include <multimap.h>

#include <xobject.h>
#include <xtwindow.h>

/** Instantiations of this class are "passive" objects.

    @memo Static text class
*/
class XtStaticText : public XObject {
 public:

  /** Constructs a new XtStaticText
      @param __r object's bounds
      @param __Alignment align style, which can be:
        <p><b>AS_NO_ALIGN</b> - no align is made
        <p><b>AS_LEFT</b> - aligns to the left
        <p><b>AS_CENTER</b> - centers the text horizontally
        <p><b>AS_RIGHT</b> - aligns to the right
        <p><b>AS_TOP</b> - aligns to the top
        <p><b>AS_BOTTOM</b> - aligns to the bottom
        <p><b>AS_MIDDLE</b> - centers the text vertically
      @param __Text a default text
  */
  XtStaticText(XRect __r, unsigned __Alignment, const char *__Text);

  /// Destructs the object instance
  virtual ~XtStaticText();

  /// Draws the text
  virtual void Draw();

  /// Returns the current text
  virtual const char *GetValue();

  /// Sets the text
  virtual void SetValue(const char *__Text);

 protected:

  /// Text
  string *Text;

  /// Alignment style
  unsigned Alignment;
};

// Alignment styles
#define AS_NO_ALIGN	0x00
#define AS_LEFT		0x01
#define AS_CENTER	0x02
#define AS_RIGHT	0x03
#define AS_TOP		0x04
#define AS_BOTTOM	0x05
#define AS_MIDDLE	0x06

// Palette entries
#define P_STATICTEXT_LEN	2

#define P_STATICTEXT_TEXT	0x01

// Default palette
#define P_STATICTEXT		"\x07\x04"

// Alternate palette
#define P_STATICTEXT_ALT	"\x00\x07"

/** Instantiations of this class are "passive" objects.

    @memo Contextual help class
*/
class XtContextualHelp : public XtStaticText {
 public:

  /** Constructs a new XtContextualHelp with the specified bounds.
      @param __r help bounds
      @param __Alignment align style, which can be:
        <p><b>AS_NO_ALIGN</b> - no align is made
        <p><b>AS_LEFT</b> - aligns to the left
        <p><b>AS_CENTER</b> - centers the text horizontally
        <p><b>AS_RIGHT</b> - aligns to the right
        <p><b>AS_TOP</b> - aligns to the top
        <p><b>AS_BOTTOM</b> - aligns to the bottom
        <p><b>AS_MIDDLE</b> - centers the text vertically
      @param __Flags options, which can be:
        <p><b>DISPLAY_MARGINS</b> - show window margins
  */
  XtContextualHelp(XRect __r, unsigned __Alignment, unsigned long __Flags);

  /// Destructs the object instance
  virtual ~XtContextualHelp();

  /// Displays the help content
  virtual void Draw();

  /// Returns the current context's value
  virtual unsigned long GetContext();

  /// Returns the text associated to the specified context's value
  virtual const char *GetValue(unsigned long __Context);

  /// Sets the current context's value
  virtual void SetContext(unsigned long __Context);

  /// Associate the specified text to the specified context's value
  virtual void SetValue(unsigned long __Context, const char *__Text);

 protected:
  unsigned long Flags, Context;
  XtWindow *Wnd;
  typedef multimap < unsigned long, string *, less < const unsigned long > > ContextsMap;
  typedef pair < const unsigned long, string * > ContextsKeyValPair;
  ContextsMap ContextMap;
};

// XtContextualHelp options
#define DISPLAY_MARGINS	0x01

#endif
