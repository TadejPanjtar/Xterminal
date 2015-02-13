/*
  xtdialog.h
  Definitions for the XtDialog and XtDialogBox

  Copyright (c) 1996-2000 Dragos Acostachioaie
  Copyright (c) 2000 Udo Kreckel

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

#ifndef _XTDIALOG_H
#define _XTDIALOG_H

#include <xbclass.h>
#include <xobject.h>
#include <xtwindow.h>

/**
    @memo Dialog containing multiple objects
*/
class XtDialog : public XObject {
 public:

  /// Constructs a new XtDialog with the specified bounds
  XtDialog(XRect __r);

  /** Handles the object passed as argument, by calling <i>ProcessEvent()</i>
      of all the registered clients
  */
  virtual void HandleEvent(XEvent *__Event);

  /// Process messages
  virtual bool ProcessMessage(XMessage *__Event);

 protected:

  /// The client that currently own the focus
  XObject *CurrentFocusedClient;

  /// Set the focus to the next client
  virtual void FocusNextClient();

  /// Set the focus to the previos client
  virtual void FocusPrevClient();
};

/**
    @memo Dialog window containing multiple objects
*/
class XtDialogBox : public XtDialog {
 public:

  /// Constructs a new dialog window with the specified bounds and title
  XtDialogBox(XRect __r, const char *__Title);

  /// Destructs the object's instance
  virtual ~XtDialogBox();

  virtual void CalculateBounds();

  /// Draws the dialog window and all the registered clients
  virtual void Draw();

  /// Move the dialog's origin to the specified point
  virtual void MoveTo(XPoint __p);

  /** "Runs" the dialog.
      Listens for events, process them, until the dialog is closed
      (e.g. the <i>CM_CLOSE</i> or <i>CM_OK</i> command is received)
  */
  virtual int Run();

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /// Sets the focus to the current object
  virtual bool SetFocus(XObject *__Object);

  /// Sets the object's state
  virtual void SetState(unsigned long __Mask, bool __BoolValue);

  /// Set the dialog title
  virtual void SetTitle(const char *__Title);

 protected:

  /// Associated window
  XtWindow *Wnd;
};

#endif
