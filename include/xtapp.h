/*
  xtapp.h
  Definition for XtApplication

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

#ifndef _XTAPP_H
#define _XTAPP_H

#include <xtmenu.h>
#include <xobject.h>
#include <xtdesktop.h>

/**
    @memo Full-featured application
*/
class XtApplication : public XtDesktop {
 public:

  /// Constructs a new XtApplication, using the entire screen
  XtApplication();

  /// Destructs the object's instance
  ~XtApplication();

  /// Draws the object
  virtual void Draw();

  /** Handles the object passed as argument, by calling
      \Ref{XObject::ProcessEvent} for all registered clients
  */
  virtual void HandleEvent(XEvent *__Event);

  /// Function that gets called when no events ocurrs
  virtual void Idle();

  /// Initializes the application's menu bar
  virtual void InitMenuBar();

  /// Initializes the application's status line
  virtual void InitStatusLine();

  /// Process commands
  virtual bool ProcessCommand(XMessage *__Event);

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Registers the specified XObject as client
  virtual void RegisterClient(XObject *__Client);

  /// Waits for events, process them, until the <i>CM_QUIT</i> command is received
  virtual void Run();

  /** Puts the specified client on top of the desktop; waits for events,
      provides them to the client, until the <i>CM_CLOSE</i> command is received
  */
  virtual void RunClient(XObject *__Client);

 protected:

  /// Application's associated pull-down menu
  XtMenu *Menu;

  /// Application's associated status line
  XtStatusLine *StatusLine;

  /// The client that currently own the focus
  XObject *CurrentFocusedClient;
};

#endif
