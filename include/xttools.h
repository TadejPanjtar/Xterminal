/*
  xttools.h
  Definition for XtMessageBox

  Copyright (c) 1998-99 Dragos Acostachioaie

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

#ifndef _XTTOOLS_H
#define _XTTOOLS_H

#include <xtpassive.h>
#include <xtbutton.h>
#include <xtdialog.h>

/**
    @memo A message box
*/
class XtMessageBox : public XtDialogBox {
 public:

  /** Constructs a new XtMessageBox
      @param __r box bounds
      @param __Title box title
      @param __Message text to print
      @param __ButtonFlags buttons options, which can be:
          <p><b>YES_NO_BUTTON</b> - "Yes"/"No" buttons
	  <p><b>OK_CANCEL_BUTTON</b> - "Ok"/"Cancel" buttons
	  <p><b>OK_BUTTON</b> - "Ok" button
	  <p><b>CANCEL_BUTTON</b> - "Cancel" button
  */
  XtMessageBox(XRect __r, const char *__Title, const char *__Message,
               unsigned long __ButtonFlags);

  /** "Runs" the dialog.
      Listens for events, process them, until the dialog is closed
  */
  virtual int Run();

 protected:
  XtStaticText *Message;
  XtButton *Button1, *Button2;
};

extern char *YES_BUTTON_MESSAGE;
extern char *NO_BUTTON_MESSAGE;
extern char *OK_BUTTON_MESSAGE;
extern char *CANCEL_BUTTON_MESSAGE;

#define YES_NO_BUTTON		0x01
#define OK_CANCEL_BUTTON	0x02
#define OK_BUTTON		0x03
#define CANCEL_BUTTON		0x04

#endif
