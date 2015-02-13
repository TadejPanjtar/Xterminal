/*
  xthlist.h
  Definition for XtHList

  Written by Ric Klaren based on XtList by Dragos Acostachioaie

  Copyright (c) 1998-99 Dragos Acostachioaie
  Copyright (c) 1999 Ric Klaren

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

#ifndef _XTHLIST_H
#define _XTHLIST_H

#include <xobject.h>
#include <xtbar.h>
#include <vector>

enum {
	HLI_TYPE_GENERIC,
	HLI_TYPE_NODE,
	HLI_TYPE_LEAF
};

enum {
	HLI_NONE = 0,
	HLI_HIDDEN = 1,
	HLI_SELECTED = HLI_HIDDEN << 1,		// not implemented
	HLI_OPEN = HLI_SELECTED << 1,
	HLI_DISABLED = HLI_OPEN << 1		// not implemented
};

/** Base item class.

    Provides basic functionality for the node and leaf classes
*/
class XtHLItem {
 public:
  XtHLItem(char *__text, int __state = 0, XtHLItem *__next = 0)
    {
    text = new char[strlen(__text)+1];
    state = __state;
    next = __next;
    strcpy(text, __text);
    }

  virtual ~XtHLItem()
    {
    if(text != 0)
      delete [] text;
    }

  virtual char FirstLetter() const
    {
    if(text != 0)
      return text[0];
     else
      return '\0';
    }

  virtual char *Text() const
    {
    return text;
    }

  virtual int Type() const
    {
    return HLI_TYPE_GENERIC;
    }

  virtual bool Selected() const
    {
    return ((state & HLI_SELECTED) != 0);
    }

  virtual void Select()
    {
    state |= HLI_SELECTED;
    }

  virtual void Deselect()
    {
    state &= ~HLI_SELECTED;
    }

  virtual bool ToggleSelect()
    {
    state ^= HLI_SELECTED;
    return ((state & HLI_SELECTED) != 0);
    }

  virtual void Disable()
    {
    state |= HLI_DISABLED;
    }

  virtual void Enable()
    {
    state &= ~HLI_DISABLED;
    }

  virtual bool Disabled() const
    {
    return ((state & HLI_DISABLED) != 0);
    }

  virtual XtHLItem *Next() const
    {
    return next;
    }

  virtual XtHLItem *SetNext(XtHLItem *__next)
    {
    next = __next;
    return next;
    }

 protected:
  char *text;					// text shown in hlist
  int state;					// mask of HLI_OPEN etc...
  XtHLItem *next;				// next item
};

/// class for leaves, the selectable elements of the list
class XtHLItemLeaf : public XtHLItem {
 public:
  XtHLItemLeaf(char *__text, unsigned long __key, int __state = HLI_OPEN,
    XtHLItem *__next = 0) : XtHLItem(__text, __state, __next)
    {
    key = __key;
    }

  virtual ~XtHLItemLeaf()
    {
    }

  virtual int Type() const
    {
    return HLI_TYPE_LEAF;
    }

  virtual unsigned long Key() const
    {
    return key;
    }

 protected:
  unsigned long key;
};

class XtHLItemNode : public XtHLItem {
 public:
  XtHLItemNode(char *__text, int __state = HLI_NONE, XtHLItem *__next = 0, XtHLItemNode *__parent = 0 ) :
    XtHLItem(__text, __state, __next)
    {
    last_child = 0;
    parent = __parent;
    indicator_open = '-';
    indicator_close = '+';
    }

  ~XtHLItemNode(void)
    {
    }

  virtual int Type() const
    {
    return HLI_TYPE_NODE;
    }

  virtual void Open()
    {
    state |= HLI_OPEN;
    }

  virtual void Close()
    {
    state &= ~HLI_OPEN;
    }

  virtual void ToggleOpen()
    {
    state ^= HLI_OPEN;
    }

  virtual bool Opened() const
    {
    return ((state & HLI_OPEN) != 0);
    }

  virtual char Indicator() const
    {
    if((state & HLI_OPEN) != 0)
      return indicator_open;
     else
      return indicator_close;
    }

  virtual void FixParents( XtHLItemNode *parent, XtHLItem *child, XtHLItem *new_child )
	{
	 if( parent )
		{
		// fix this one...
		parent->last_child = new_child;
		// and his parent..
		FixParents( parent->parent, child, new_child );
		}
	}

  virtual void AddFixLinks(XtHLItem *item)
    {
    FixParents( this->parent, this, item );
    if(last_child == 0)	// easy case...
      {
      last_child = item;
      item->SetNext(next);
      next = item;
      }
     else
      {
      item->SetNext(last_child->Next());
      last_child->SetNext(item);
      last_child = item;
      }
    }

  virtual void AddNode(char *text, int state)
    {
    XtHLItem *item = new XtHLItemNode(text, state, 0, this );
    AddFixLinks(item);
    }

  virtual void Add(char *text, unsigned long value, int state)
    {
    XtHLItem *item = new XtHLItemLeaf(text, value, state);
    AddFixLinks(item);
    }

  virtual XtHLItem *LastChild() const
    {
    return last_child;
    }

// TODO: methods for indicator setting
 private:
  XtHLItem *last_child;
  XtHLItemNode *parent;
  char indicator_open;
  char indicator_close;
};

// Palette entries
#define P_HLIST_LEN			8

#define P_HLIST_SURFACE			0x01
#define P_HLIST_DISABLED		0x03
#define P_HLIST_SELECTED		0x05
#define P_HLIST_SELECTED_DISABLED	0x07

// Default palette
#define P_HLIST 		"\x00\x07\x40\x07\x47\x06\x40\x06\x00\x07\x80\x07\x10\x07\x50\x07"

enum {
	HLIST_SELECTABLE_ITEMS = 1,
	HLIST_CURSOR_WRAP = HLIST_SELECTABLE_ITEMS << 1,
	HLIST_SELECT_LIMIT = HLIST_CURSOR_WRAP << 1
};

/**
    @memo Selectable hierchical list of items
*/
class XtHList : public XObject {
 public:
  /** Constructs a new XtHList.

      HList is short for Hierarchical List. It consists of a number of
      'branches' with 'leaves'. When a branch is closed only one line is
      displayed with a '+' in front of it. When a branch is open a '-' is shown
      in front of it.
      @param __r list bounds
      @param __Flags options, which can be:
	 <p><b>HLIST_SELECTABLE_ITEMS</b> - to allow selection (enable/disable) of items
	 <p><b>HLIST_CURSOR_WRAP</b> - make the list a circular one
	 <p><b>HLIST_SELECT_LIMIT</b> - hide the items that are disabled
  */
  XtHList(XRect __r, unsigned long __Flags, unsigned int __max_selectable = 1);

  /// Destructs the object's instance
  ~XtHList();

  /// Draws the object
  virtual void Draw();

  /// Returns the number of items in the list
  virtual unsigned GetItemCount() const;

  /// Clears the selected items in the list
  void ClearSelected ();

  /// Returns the number of selected items in the list
  unsigned int NumberSelected() const;

  /// Return the key values associated with the selected items
  void SelectedKeys(vector<unsigned long>& values) const;

  /// Return the texts of the selected items
  void SelectedStrings(vector<char *>& values) const;

  /** Add a node to the hierarchical list
      @param __Text text to be displayed
      @param state, Various things that can be set regarding the item.
	   <p><b>HLI_NONE</b> - Nothing special is the default.
	   <p><b>HLI_HIDDEN</b> - Item is hidden (not implemented yet).
	   <p><b>HLI_SELECTED</b> - Item is selected.
	   <p><b>HLI_OPEN</b> - Only for nodes/branches, branch is open.
	   <p><b>HLI_DISABLED</b> - Item is disabled (not completely implemented).
  */
  virtual int AddNode( char *__Text, int state = HLI_NONE );

  /// Add a node to node to_node in the hierarchical list
  virtual bool AddNode(const char *to_node, char *text, int state = HLI_NONE);

  /// Add a item to node to_node in the hierarchical list
  virtual bool Add(const char *to_node, char *text, unsigned long key, int state = HLI_NONE);

  /** "Runs" the list: waits for events until an item is chosen or the
      dialog is closed
  */
  virtual int Run();

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process message events
  virtual bool ProcessMessage(XMessage *__Event);

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /// Sets the object's state
  virtual void SetState(unsigned long __Mask, bool __BoolValue);

#ifdef DEBUG
  virtual void Dump(void);
#endif
 protected:
  void Select(XtHLItem *item);
  void Deselect(XtHLItem *item);
  void DeselectFirst();

  XtHLItem *NextItem(XtHLItem *item) const;
  XtHLItem *LastItem() const;
  XtHLItem *LastVisibleItem() const;
  XtHLItem *GetItemNo(unsigned int index) const;

  void DrawItem(const XtHLItem *item, const XtHLItem *last);
  XtHLItem *DrawNode(XtHLItemNode *snode);

  unsigned int UpdateLineCount();
  unsigned int GetItemLine(XtHLItem *target) const;
  XtHLItem *GetItemOnLine(unsigned int line) const;

  XtHLItemNode *FindNode(const char *text) const;
  bool OpenBranches(XtHLItem *target);
  bool OpenBranchesNode(XtHLItem *target, XtHLItemNode *node);
  void ExposeItem( XtHLItem *target );
  bool SearchLetter(XtHLItem *item, int KeyCode);

  /// Initialize the associated scroll bar
  virtual void InitScrollBar();

  XtScrollBar *ScrollBar;
  unsigned int Flags;
  unsigned int number_selected;
  unsigned int max_selectable;			// maximum number selectable

  unsigned int CurrIndex;			// Index of current item
  XtHLItem *CurrItem;				// the item itself
  unsigned int ItemCount;			// Number of items in the list
  unsigned int LineCount;			// number of lines in the hlist (ex. closed branches)
  unsigned int LineOffset, CurY;
  unsigned int draw_index, indent, ndrawn;	// Used during redraws...

  XtHLItem *root;				// root of the hierarchical list
};

#endif
