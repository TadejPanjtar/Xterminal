/*
  xtwindow.h
  Definition for XtWindow

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

#ifndef _XTWINDOW_H
#define _XTWINDOW_H

#include <xtscreen.h>
#include <xobject.h>

/**
    @memo Window class
*/
class XtWindow : public XObject {
 friend class XtDialogBox;
 public:

  /// Constructs a new XtWindow with the specified bounds and title
  XtWindow(XRect __r, const char *__Title);

  /// Destructs the XtWindow instance
  virtual ~XtWindow();

  /// Close the window
  virtual void Close();

  /// Draws the window
  virtual void Draw();

  /// Move the upper-left window corner to the specified coordinate
  virtual void MoveTo(XPoint __p);

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /** Process message events. __Event->Message can be:
      <p><b>WM_CLOSE</b> - request to close the window
      <p><b>WM_OPEN</b> - request to open the window (e.g. to draw it)
      <p><b>WM_MOVE</b> - request to move the window
  */
  virtual bool ProcessMessage(XMessage *__Event);

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /** Adjusts the object's size with the specified coordinates
      @param __delta translation vector
  */
  virtual void Resize(XPoint __delta);

  /// Set the frame style
  virtual void SetFrame(char *__Frame);

  /// Set the window title
  virtual void SetTitle(const char *__Title);

  /// Sets the window state
  virtual void SetState(unsigned long __Mask, bool __BoolValue);

 protected:

  /// Buffer to save the old screen area
  chtype *Buffer;

  /// Window title
  char *Title;

  /// Window frame
  char Frame[10];

  /// Updates window margins
  void FrameUpdate();

  XPoint Adjust;
};

// Messages
#define WM_CLOSE     0x0100
#define WM_OPEN      0x0101
#define WM_MOVE      0x0102

// State Masks
#define WM_HAVE_SHADOW	0x0800
#define WM_CAN_CLOSE	0x1000
#define WM_CAN_MOVE	0x2000
#define WM_CAN_ZOOM	0x4000
#define WM_CAN_RESIZE	0x8000

// Action keys
#define WINDOW_MOVE_KEY		KB_META('m')
#define WINDOW_CLOSE_KEY	KB_META('c')

// Markers
#define WINDOW_CLOSE_MARKER	ACS_BULLET
#define WINDOW_ZOOM_MARKER	ACS_UARROW

// Palette entries
#define P_WINDOW_LEN			14

#define P_WINDOW_SURFACE		0x01
#define P_WINDOW_FRAME			0x03
#define P_WINDOW_TITLE			0x05
#define P_WINDOW_UNFOCUSED_FRAME	0x07
#define P_WINDOW_UNFOCUSED_TITLE	0x09
#define P_WINDOW_SHADOW			0x0B
#define P_WINDOW_MARKERS		0x0D

// Default palette
#define P_WINDOW	"\x07\x04\x07\x04\x07\x04\x00\x04\x00\x04\x07\x00\x02\x04\x17\x00\x17\x00\x17\x00\x00\x00\x00\x07\x07\x00\x07\x00"

// Alternate palette
#define P_WINDOW_ALT	"\x00\x07\x00\x07\x00\x07\x00\x02\x00\x02\x07\x00\x02\x00\x17\x00\x17\x00\x17\x00\x00\x00\x00\x07\x07\x00\x07\x00"

#endif
