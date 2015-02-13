/*
  xttab.h
  Definitions for XtTab and XtTabDialog

  Copyright (c) 2001 Dragos Acostachioaie

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

#ifndef _XTTAB_H
#define _XTTAB_H

#include <xbclass.h>
#include <xobject.h>
#include <xtdialog.h>
#include <xtwindow.h>

/**
    @memo Tab containing multiple objects
*/
class XtTab : public XtDialogBox {
 public:

  /// Constructs a new XtTab with the specified bounds
  XtTab(XRect __r, const char *__Title);

  /// Destructs the object's instance
  virtual ~XtTab();
};

class XtTabDialog : public XtDialog {
 public:

  /// Constructs a new XtTabDialog with the specified bounds
  XtTabDialog(XRect __r);

  /// Destructs the object's instance
  virtual ~XtTabDialog();

  /// Draws the dialog window and all the registered clients
  virtual void Draw();

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /** "Runs" the dialog.
      Listens for events, process them, until the dialog is closed
      (e.g. the <i>CM_CLOSE</i> or <i>CM_OK</i> command is received)
  */
  virtual int Run();
};

#endif
