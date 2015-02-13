/*
  xtbutton.h
  Definitions for XtButton, XtCheckButton and XtRadioButton

  Copyright (c) 1996-97 Adrian Teohar
  Copyright (c) 1996-99 Dragos Acostachioaie
  Documentation Copyright (c) 1998-99 Dragos Acostachioaie

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

#ifndef _XTBUTTON_H
#define _XTBUTTON_H

#include <xtscreen.h>
#include <xobject.h>

/**
    @memo Push button
*/
class XtButton : public XObject {
 public:

  /** Constructs a new XtButton
      @param __r button bounds
      @param __Text text inside the button
      @param __State button flags, which can ORed:
          <p><b>BF_ARROW</b> - to mark the button with two arrows when selected
      @param __Command command that the button generates
  */
  XtButton(XRect __r, char *__Text, int __State, int __Command);

  /// Destructs the object's instance
  virtual ~XtButton();

  /// Computes the object's size and the real bounds
  virtual void CalculateBounds();

  /// Draws the object
  virtual void Draw();

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process messages
  virtual bool ProcessMessage(XMessage *__Event);

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /// Sets the object's state
  virtual void SetState(unsigned long __Mask, bool __BoolValue);

 protected:
  char Text[80], HotChar;
  chtype *Buffer;
  int Command;
  void Press();
  void WriteString(int __x, int __y, int __w, char *__astring);
};

// "Push" effect delay (in miliseconds)
extern unsigned BD_PUSH_EFFECT_DELAY;

// Flags
#define BF_NORMAL        0x0100
#define BF_DEFAULT       0x0200
#define BF_PRESSED       0x0400
#define BF_ARROW         0x0800

// Palette entries
#define P_BUTTON_LEN			16

#define P_BUTTON_TEXT			0x01
#define P_BUTTON_DEFAULT		0x03
#define P_BUTTON_SELECTED		0x05
#define P_BUTTON_DISABLED		0x07
#define P_BUTTON_SHORTCUT		0x09
#define P_BUTTON_SHORTCUT_DEFAULT	0x0B
#define P_BUTTON_SHORTCUT_SELECTED	0x0D
#define P_BUTTON_SHADOW			0x0F

// Default palette
#define P_BUTTON	"\x00\x06\x07\x06\x47\x06\x45\x06\x41\x06\x41\x06\x07\x06\x00\x04\x00\x06\x07\x06\x47\x06\x45\x06\x41\x06\x41\x06\x07\x06\x00\x04"

// Cluster Item definition
struct CItem {
  char Text[80];
  int Value;
  CItem *Next;
};

CItem *NewItem(char *, int, CItem *);

/**
    @memo Multiple items checking button
*/
class XtCheckButton : public XObject {
 public:

  /** Constructs a new XtCheckButton
      @param __r button bounds
      @param __Items pointer to clusters list
  */
  XtCheckButton(XRect __r, CItem *__Items);

  /// Destructs the object's instance
  virtual ~XtCheckButton();

  /// Draws the object
  virtual void Draw();

  /** Returns the value of the specified item number.
      @param __Item item number, 0 = current item, >0 = item's value
  */
  virtual int GetValue(unsigned __Item);

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process messages
  virtual bool ProcessMessage(XMessage *__Event);

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /// Sets the object's state
  virtual void SetState(unsigned long __Mask, bool __BoolValue);

 protected:
  CItem *CurrIndex;
  unsigned long CurrItem, ItemsNo;
  XPoint OldCursorPos;
  CItem *Items;
};

extern char* CB_ICON;
extern char CB_MARKER;

/**
    @memo Multiple items radio button
*/
class XtRadioButton : public XObject {
 public:

  /** Constructs a new XtRadioButton
      @param __r button bounds
      @param __Items pointer to clusters list
  */
  XtRadioButton(XRect __r, CItem *__Items);

  /// Destructs the object's instance
  virtual ~XtRadioButton();

  /// Draws the object
  virtual void Draw();

  /** Returns the value of the specified item number.
      @param __Item item number, 0 = current item, >0 = item's value
  */
  virtual int GetValue(unsigned __Item);

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process messages
  virtual bool ProcessMessage(XMessage *__Event);

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /// Sets the object's state
  virtual void SetState(unsigned long __Mask, bool __BoolValue);

 protected:
  CItem *CurrIndex;
  unsigned long CurrItem, ItemsNo;
  XPoint OldCursorPos;
  CItem *Items;
  int Value;
};

extern char *RB_ICON;
extern char RB_MARKER;

// Palette entries
#define P_CLUSTER_LEN			10

#define P_CLUSTER_TEXT			0x01
#define P_CLUSTER_DISABLED		0x03
#define P_CLUSTER_SELECTED		0x05
#define P_CLUSTER_SHORTCUT		0x07
#define P_CLUSTER_SHORTCUT_SELECTED	0x09

// Default palette
#define P_CLUSTER	"\x00\x06\x47\x06\x47\x06\x01\x06\x46\x06\x00\x06\x47\x06\x47\x06\x01\x06\x46\x06"

#endif
