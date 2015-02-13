/*
  xtcombobox.h
  Definition for XtComboBox

  Copyright (c) 1999-2000 Pavel Andreew

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

#ifndef _XTCOMBOBOX_H
#define _XTCOMBOBOX_H

#include <xobject.h>
#include <xtfield.h>
#include <xtlist.h>

/**
    @memo ComboBox control
*/
class XtComboBox : public XtInputStringField
{
  public:
    /** Constructs a new XtComboBox.
        @param __r field bounds
        @param __Items selectable list of items
        @param __Rows maximum visible rows at list
    */
    XtComboBox(XRect __r, LItem *__Items, short __Rows);

    /// Destructs the object's instance
    virtual ~XtComboBox();

    /// Add LItem at top of the list
    virtual void AddFirst(const char *__Text, long __Value, short __Flags);

    /// Add LItem at top of the list
    virtual void AddFirst(const char *__Text);

    /// Add LItem at end of the list
    virtual void AddLast(const char *__Text, long __Value, short __Flags);

    /// Add LItem at end of the list
    virtual void AddLast(const char *__Text);

    /** Deletes the specified item number
        @param __Item item number
    */
    virtual bool DeleteItem(unsigned long __Item);

    /// Draws the object
    virtual void	Draw();

    /// Insert the item as defined position
    virtual bool InsertItem(unsigned long __Position, LItem *__Item);

    /// Process keyboard events
    virtual bool	ProcessKeyboardEvent(XKeyboardEvent *__Event);

    /// Sets the current value of field corresponding with key from item list
    virtual void	SetKey(long __Key);

    /// Sets the current value of field
    virtual void	SetValue(char *__DefaultString);

    /// Sets a new list content
    virtual void	SetValue(LItem *__Items);

    /// Returns the key of current item
    virtual long	GetKey();

    /// Returns the list content
    virtual LItem	*GetList() { return DropList->GetValue(); }

  private:
    void	SetCurrentItem(char *__Template);
    /// Sets the current value of field corresponding with item number from list
    void	SetCurrentItem(unsigned long __Index);
    void	ClearTemplate();

  protected:
    XtList		*DropList;
    char		*Template;
    unsigned long	CurrentItem, ItemsNo;
};

extern char *xstrdup(const char *str);
extern int xstrcoll(const char *__s1, const char *__s2);

#endif
