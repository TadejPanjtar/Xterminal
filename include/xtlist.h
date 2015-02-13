/*
  xtlist.h
  Definition for XtList

  Copyright (c) 1998-2001 Dragos Acostachioaie
  Copyright (c) 1999-2000 Udo Kreckel

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

#ifndef _XTLIST_H
#define _XTLIST_H

#include <xobject.h>
#include <xtbar.h>

// List Item definition
struct LItem {
    char *Text;
    short Flags;
    long Value;
    LItem *Next;
};

LItem *NewLItem(char *__Text, long __Value, short __Flags, LItem *__ANextItem);
LItem *NewList(char **__Text, unsigned __Count);

/**
    @memo Selectable list of items
*/
class XtList : public XObject {
 public:

  /** Constructs a new XtList.
      @param __r list bounds
      @param __Flags options, which can be:
          <p><b>SELECTABLE_ITEMS</b> - to allow selection (enable/disable) of items
	  <p><b>CIRCULAR_SELECTION</b> - make the list a circular one
	  <p><b>HIDE_DISABLED_LIST_ITEMS</b> - hide the items that are disabled
	  <p><b>LIST_V_SCROLLBAR</b> - to provide a vertical scroll bar
	  <p><b>LIST_H_SCROLLBAR</b> - to provide a horizontal scroll bar
	  <p><b>MAX_SELECTED</b> - do not allow more than maximum selectable items
      @param __Items linked list of items:
          <p>struct LItem {<br>
               char *Text;<br>
	       short Flags;<br>
               long Value;<br>
               LItem *Next;<br>
             };<br>
          <p>This list can be constructed using the following 2 functions:
	  <p>LItem *NewLItem(char *__Text, long __Value, short __Flags, LItem *__ANextItem)
	     which builds the entire list, and:
	  <p>LItem *NewList(char **__Text, unsigned __Count)
	     which builds from a list of char*, containing __Count items,
	     a linked list of items
  */
  XtList(XRect __r, unsigned long __Flags, LItem *__Items = 0);

  /// Destructs the object's instance
  ~XtList();

  /// Add LItem at top of the list
  virtual void AddFirst(const char *__Text, long __Value, short __Flags);

  /// Add LItem at top of the list
  virtual void AddFirst(const char *__Text);

  /// Add footer at bottom of the list
  virtual void AddFooter(const char *__Text);

  /// Add header at top of the list
  virtual void AddHeader(const char *__Text);

  /// Add LItem at end of the list
  virtual void AddLast(const char *__Text, long __Value, short __Flags);

  /// Add LItem at end of the list
  virtual void AddLast(const char *__Text);

  /// Mark all the selected items as not selected
  virtual void ClearSelected();

  /// Mark the specified item as not selected
  virtual void DeSelect(LItem *__Item);

  /// Decrement the number of selected items
  virtual void DecNumberSelected();

  /** Deletes the specified item number
      @param __Item item number
  */
  virtual bool DeleteItem(unsigned long __Item);

  /// Draws the object
  virtual void Draw();

  /// Returns the columns number
  virtual unsigned long GetColsNumber();

  /// Returns the item number of the current cursor position
  virtual unsigned long GetCurrentItem();

  /** Returns the flags value of the specified item number
      @param __Item item number
  */
  virtual short GetFlags(unsigned long __Item);

  /// Returns the items number
  virtual unsigned long GetItemsNumber();

  /// Returns the last keycode
  virtual unsigned int GetKeyCode();

  /** Returns the item number of the selected items or zero if end of list
      @param __Item item number, 0 = start first item, >0 = next item
  */
  virtual unsigned long GetSelected(unsigned long __Item);

  /** Returns the text of the specified item number
      @param __Item item number
  */
  virtual char *GetText(unsigned long __Item);

  /** Returns the value of the specified item number
      @param __Item item number
  */
  virtual long GetValue(unsigned long __Item);

  /// Returns the list content
  virtual LItem* GetValue();

  /// Increments the number of selected items
  virtual void IncNumberSelected();

  /// Insert the item as defined position
  virtual bool InsertItem(unsigned long __Position, LItem *__Item);

  /// Returns the maximum number of selectable items
  virtual unsigned long MaxSelected();

  /// Returns the number of selected items
  virtual unsigned long NumberSelected();

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process message events
  virtual bool ProcessMessage(XMessage *__Event);

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /** "Runs" the list: waits for events until an item is choosed or the
      dialog is closed
  */
  virtual int Run();

  /// Mark the specified item as selected
  virtual void Select(LItem *__Item);

  /// Sets the current item number
  virtual void SetCurrentItem(LItem *__Item);

  /// Sets the current item number
  virtual void SetCurrentItem(unsigned long __Item);

  /** Sets the flags of the specified item number.
      @param __Item item number
      @param __Flags flags
  */
  virtual void SetFlags(unsigned long __Item, short __Flags);

  /// Changes the maximum number of selectable items
  virtual void SetMaxSelectable(unsigned long __max);

  /// Sets the object's state
  virtual void SetState(unsigned long __Mask, bool __BoolValue);

  /** Sets the text of the specified item number.
      @param __Item item number
      @param __Text text
  */
  virtual bool SetText(unsigned long __Item, const char *__Text);

  /// Sets a new list content
  virtual void SetValue(LItem *__Items);

  /** Sets the value of the specified item number.
      @param __Item item number
      @param __Value value
  */
  virtual void SetValue(unsigned long __Item, long __Value);

 protected:
  /// Counts the selected items
  virtual void CountSelected();

  /// Initialize the associated scroll bar
  virtual void InitScrollBar();

  void SearchDialog(bool __cont);

  /// Sets the associated scroll bar values
  virtual void SetScrollBar();

  XtScrollBar *VScrollBar, *HScrollBar;
  unsigned long Flags;
  LItem *CurrIndex;
  unsigned long CurrItem, ItemsNo, ColsNo, CP, CC, CurY;
  XPoint OldCursorPos;
  unsigned long max_selected, number_selected;
  unsigned int KeyCode;
  LItem *Items;
  char *HeaderLine, *FooterLine;
  unsigned short AddLine;
};

#define SELECTABLE_ITEMS		0x0001
#define CIRCULAR_SELECTION		0x0002
#define SKIP_DISABLED_LIST_ITEMS	0x0004
#define HIDE_DISABLED_LIST_ITEMS	0x0008
#define MAX_SELECTED			0x0010
#define LIST_H_SCROLLBAR		0x0020
#define LIST_V_SCROLLBAR		0x0040
#define HEADER_LINE              	0x0080
#define FOOTER_LINE              	0x0100

#define ITEM_DISABLED	0x0001
#define ITEM_SELECTED	0x0002
#define ITEM_INVISIBLE	0x0004

// Palette entries
#define P_LIST_LEN			8

#define P_LIST_SURFACE			0x01
#define P_LIST_DISABLED			0x03
#define P_LIST_SELECTED			0x05
#define P_LIST_SELECTED_DISABLED	0x07

// Default palette
#define P_LIST 		"\x00\x07\x40\x07\x47\x06\x40\x06\x00\x07\x80\x07\x10\x07\x50\x07"

#endif
