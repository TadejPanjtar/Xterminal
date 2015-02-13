/*
  xtmenu.h
  Definitions for XtMenu and XtStatusLine

  Copyright (c) 1998-99 Dragos Acostachioaie
  Copyright (c) 2000    Udo Kreckel

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

#ifndef _XTMENU_H
#define _XTMENU_H

#include <xtpassive.h>
#include <xobject.h>
#include <xtwindow.h>

// Menu Item definition
struct MenuItem {
  char *Text;
  char *KeyText;
  int Key, Command;
  long Id;
  unsigned short Value;
  XtWindow *ChildWindow;
  MenuItem *Child, *Parent;
  MenuItem *Next;
};

#define MENU_ITEM_DISABLED	0x0001
#define MENU_ITEM_SELECTED	0x0002
#define MENU_ITEM_CHILD_MENU	0x0004
#define MENU_ITEM_CHILD_OPEN	0x0008
#define MENU_ITEM_SEPARATOR	0x0010

MenuItem *NewMenuItem(char *__Text, char *__KeyText, int __Key, int __Command, MenuItem *__Child, MenuItem *__ANextItem);
MenuItem *NewLine(MenuItem *__ANextItem);
void DisableCommand(MenuItem *__Menu, int __Command);
void EnableCommand(MenuItem *__Menu, int __Command);
// void SetContextualHelp(XtMenu *__Menu, int __Command, const char *__Text);
MenuItem *_FindFirstChild(MenuItem *__start);
MenuItem *_LookForKeycode(MenuItem *__start, int __Key);
MenuItem *_LookForId(MenuItem *__start, long __Id);

static long LastId;

/**
    @memo Pull-down menu
*/
class XtMenu : public XObject {
 friend class XtApplication;
 public:

  /** Constructs a new XtMenu.
      @param __r menu bounds
      @param __Flags options, which can be:
          <p><b>VERTICAL_MENU</b> - to position the menu vertically
	  <p><b>HIDE_DISABLED_ITEMS</b> - to hide the disabled items
      @param __Items linked list of menu items:
          <p>struct MenuItem {<br>
               char *Text;<br>
               char *KeyText;<br>
               int Key, Command;<br>
               unsigned Value;<br>
               XtWindow *ChildWindow;<br>
               MenuItem *Child, *Parent;<br>
               MenuItem *Next;<br>
             };<br>
	  <p>This list can be constructed using the following 2 functions:
	  <p>MenuItem *NewMenuItem(char *__Text, char *__KeyText, int __Key, int __Command, MenuItem *__Child, MenuItem *__ANextItem)
	     which builds the entire list, and:
	  <p>MenuItem *NewLine(MenuItem *__ANextItem) which builds a line separator
  */
  XtMenu(XRect __r, unsigned long __Flags, MenuItem *__Items = 0);

  /// Destructs the object's instance
  ~XtMenu();

  /// Draws the object
  virtual void Draw();

  /// Returns the current contextual help
  virtual XtContextualHelp *GetHelp();

  /// Returns the menu content
  virtual MenuItem *GetValue();

  /// add new menu item
  virtual long AddMenuItem(const char *__Text, const char *__KeyText, int __Key, int __Command);

  /// add new sub menu item to given menu item
  virtual long AddMenuSubItem(const char *__Text, const char *__KeyText, int __Key, int __Command, long __Id);

  /// add line separator to menu item
  virtual long AddNewLine(long __Id);

  /// Deletes all menu items
  virtual void DeleteAllItem(MenuItem *__Menu);

  /// Deletes menu item upon given id and all subitems, if available
  virtual void DeleteMenuItem(long __Id);

  /// Process incoming events by calling XObjectEventListener's ProcessEvent()
  virtual bool ProcessEvent(XEvent *__Event);

  /// Process commands
  virtual bool ProcessCommand(XMessage *__Event);

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process messages
  virtual bool ProcessMessage(XMessage *__Event);

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /// Setup a contextual help for menu items
  virtual void SetHelp(XRect __r, unsigned __Alignment, unsigned long __Flags);

  /// Change the menu content
  virtual void SetValue(MenuItem *__Items);

 protected:
  unsigned long Flags;
  MenuItem *Items, *CurrentItem;
  chtype *Buffer;
  XtContextualHelp *Help;
  virtual void DeleteItem(MenuItem *__Menu);
  virtual void DrawChildMenu(XPoint __p, MenuItem *__Item);
  virtual unsigned DrawItem(XPoint __p, unsigned __width, MenuItem *__Item);
};

#define VERTICAL_MENU		0x0001
#define HIDE_DISABLED_ITEMS	0x0002

extern unsigned MENU_ENTRIES_HORIZONTAL_SPACING;
extern unsigned MENU_ENTRIES_VERTICAL_SPACING;

void SetContextualHelp(XtMenu *__Menu, int __Command, const char *__Text);

// Palette entries
#define P_MENU_LEN			10

#define P_MENU_SURFACE			0x01
#define P_MENU_DISABLED			0x03
#define P_MENU_SELECTED			0x05
#define P_MENU_SHORTCUT			0x07
#define P_MENU_SHORTCUT_SELECTED	0x09

// Default palette
#define P_MENU		"\x00\x07\x40\x07\x00\x06\x01\x07\x01\x06\x10\x07\x90\x07\x00\x07\x08\x07\x40\x07"
#define P_WINDOW_MENU	"\x00\x07\x00\x07\x00\x07\x00\x02\x00\x02\x07\x00\x02\x00\x17\x00\x17\x00\x17\x00\x00\x00\x00\x07\x07\x00\x07\x00"

struct StatusItem {
  char *Text;
  char *KeyText;
  int Key, Command;
  unsigned Value;
  StatusItem *Next;
};

StatusItem *NewStatusItem(char *__Text, int __Key, int __Command, StatusItem *__ANextItem);
void DisableCommand(StatusItem *__StatusLine, int __Command);
void EnableCommand(StatusItem *__StatusLine, int __Command);

#define STATUS_ITEM_DISABLED	0x0001

/**
    @memo Status line
*/
class XtStatusLine : public XObject {
 friend class XtApplication;
 public:

  /** Constructs a new XtStatusLine.
      @param __r status line bounds
      @param __Flags options, which can be:
          <p><b>HIDE_DISABLED_ITEMS</b> - to hide the items that are disabled
      @param __Items linked list of items:
          <p>struct StatusItem {<br>
               char *Text;<br>
               char *KeyText;<br>
               int Key, Command;<br>
               unsigned Value;<br>
               StatusItem *Next;<br>
             };<br>
	  <p>This list can be constructed using the following function:
	  <p>StatusItem *NewStatusItem(char *__Text, int __Key, int __Command, StatusItem *__ANextItem)
	     which builds the entire list
  */
  XtStatusLine(XRect __r, unsigned long __Flags, StatusItem *__Items);

  /// Destructs the object's instance
  ~XtStatusLine();

  /// Draws the object
  virtual void Draw();

  /// Returns the status line content
  virtual StatusItem* GetValue();

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /// Sets a new status line content
  virtual void SetValue(StatusItem *__Items);

 protected:
  unsigned long Flags;
  StatusItem *Items;
};

extern unsigned STATUS_ENTRIES_SPACING;

// Palette entries
#define P_STATUSLINE_LEN		6

#define P_STATUSLINE_SURFACE		0x01
#define P_STATUSLINE_DISABLED		0x03
#define P_STATUSLINE_SHORTCUT		0x05

// Default palette
#define P_STATUSLINE "\x00\x07\x81\x07\x01\x07\x10\x07\x90\x07\x08\x07"

#endif
