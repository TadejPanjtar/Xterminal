/*
  xtother.h
  Definitions for other classes: XtTextViewer

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

#ifndef _XTOTHER_H
#define _XTOTHER_H

#include <xobject.h>
#include <xtbar.h>
#include <xtwindow.h>

struct TItem {
  char *Text;
  TItem *Next;
};

/**
    @memo Text file viewer
*/
class XtTextViewer : public XtWindow {
 public:

  /// Constructs a new XtTextViewer using the specified bounds, file name and title
  XtTextViewer(XRect __r, const char *__FileName, const char *__Title);

  /// Destructs the object's instance
  ~XtTextViewer();

  /// Draws the object
  virtual void Draw();

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process messages
  virtual bool ProcessMessage(XMessage *__Event);

  /** "Runs" the text viewer.
      Listens for events, process them, until the viewer window is closed
      (e.g. the <i>CM_CLOSE</i> command is received)
  */
  virtual int Run();

 protected:
  /// Initialize the associated scroll bars
  virtual void InitScrollBar();

  XtScrollBar *HScrollBar, *VScrollBar;
  char *FileName;
  unsigned long LinesNo, ColsNo, CP, CC;
  TItem *Items;
  int ReadFile();
  void SearchDialog(bool __cont);
};

#endif
