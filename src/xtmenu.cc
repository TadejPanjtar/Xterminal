/*
  xtmenu.cc

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

#include <ctype.h>
#include <string>

#include <xtscreen.h>
#include <xtmenu.h>

unsigned MENU_ENTRIES_HORIZONTAL_SPACING = 0;
unsigned MENU_ENTRIES_VERTICAL_SPACING = 2;

MenuItem *NewMenuItem(char *__Text, char *__KeyText, int __Key, int __Command, MenuItem *__Child, MenuItem *__ANextItem)
{
  MenuItem *temp = new MenuItem;
  if(__Text)
    {
    temp->Text = new char[strlen(__Text) + 1];
    strcpy(temp->Text, __Text);
    }
   else
    temp->Text = 0;
  if(__KeyText)
    {
    temp->KeyText = new char[strlen(__KeyText) + 1];
    strcpy(temp->KeyText, __KeyText);
    }
   else
    temp->KeyText = 0;
  temp->Key = __Key;
  temp->Command = __Command;
  temp->Value = 0;
  temp->Parent = 0;
  temp->Child = 0;
  temp->ChildWindow = 0;
  temp->Next = __ANextItem;
  temp->Id = LastId++;
  if(__Child)
    {
    temp->Value = MENU_ITEM_CHILD_MENU;
    __Child->Parent = temp;
    temp->Child = __Child;
    }
  return temp;
}

MenuItem *NewLine(MenuItem *__ANextItem)
{
  MenuItem *temp = new MenuItem;
  temp->Text = new char[2];
  strcpy(temp->Text, "-");
  temp->KeyText = 0;
  temp->Key = 0;
  temp->Command = 0;
  temp->Value = MENU_ITEM_SEPARATOR;
  temp->Parent = 0;
  temp->Child = 0;
  temp->ChildWindow = 0;
  temp->Next = __ANextItem;
  temp->Id = LastId++;
  return temp;
}

void DisableCommand(MenuItem *__Menu, int __Command)
{
  MenuItem *index = __Menu;
  while(index)
    {
    if(index->Child)
      DisableCommand(index->Child, __Command);
    if(index->Command == __Command && !index->Child)
      index->Value |= MENU_ITEM_DISABLED;
    index = index->Next;
    }
}

void EnableCommand(MenuItem *__Menu, int __Command)
{
  MenuItem *index = __Menu;
  while(index)
    {
    if(index->Child)
      EnableCommand(index->Child, __Command);
    if(index->Command == __Command && (index->Value & MENU_ITEM_DISABLED))
      index->Value &= ~MENU_ITEM_DISABLED;
    index = index->Next;
    }
}

void SetContextualHelp(XtMenu *__Menu, int __Command, const char *__Text)
{
  XtContextualHelp *help;
  help = __Menu->GetHelp();
  help->SetValue(__Command, __Text);
}

MenuItem *_FindFirstChild(MenuItem *__start)
{
  MenuItem *index = __start;
  while(index && !(index->Value & MENU_ITEM_CHILD_OPEN))
    index = index->Next;
  if(index)
    return _FindFirstChild(index->Child);
  return __start;
}

MenuItem *_LookForKeycode(MenuItem *__start, int __Key)
{
  MenuItem *index = __start, *temp;
  while(index && index->Key != __Key)
    {
    if(index->Child)
      {
      if((temp = _LookForKeycode(index->Child, __Key)))
        return temp;
      }
    index = index->Next;
    }
  return index;
}

MenuItem *_LookForId(MenuItem *__start, long __Id)
{
  MenuItem *index = __start, *temp;
  while(index && index->Id != __Id)
    {
    if(index->Child)
      {
      if((temp = _LookForId(index->Child, __Id)))
        return temp;
      }
    index = index->Next;
    }
  return index;
}

MenuItem *_LookForItem(MenuItem *__start, MenuItem *__Search)
{
  MenuItem *index = __start, *temp;
  while(index && index->Next != __Search)
  {
      if(index->Child)
      {
          if (index->Child == __Search)
              return index;
          if((temp = _LookForItem(index->Child, __Search)))
              return temp;
      }
      index = index->Next;
  }
  return index;
}


XtMenu::XtMenu(XRect __r, unsigned long __Flags, MenuItem *__Items) :
  XObject(__r)
{
  strcpy(ObjClass, "XtMenu");
  SetPalette(P_MENU, P_MENU_LEN);
  Flags = __Flags;
  Items = CurrentItem = __Items;
  Help = 0;
  LastId = 1;
  XObject::SetState(SM_ACTIVE, true);
}

XtMenu::~XtMenu()
{
  DeleteAllItem(Items);
  if(Help)
    delete Help;
}

long XtMenu::AddMenuItem(const char *__Text, const char *__KeyText, int __Key, int __Command)
{
  MenuItem *index = Items;

  MenuItem *temp = new MenuItem;
  if(__Text)
    {
    temp->Text = new char[strlen(__Text) + 1];
    strcpy(temp->Text, __Text);
    }
   else
    temp->Text = 0;
  if(__KeyText)
    {
    temp->KeyText = new char[strlen(__KeyText) + 1];
    strcpy(temp->KeyText, __KeyText);
    }
   else
    temp->KeyText = 0;
  temp->Key = __Key;
  temp->Command = __Command;
  temp->Value = 0;
  temp->Parent = 0;
  temp->Child = 0;
  temp->ChildWindow = 0;
  temp->Next = 0;
  temp->Id = LastId++;
  if(!index)
  {
      Items = temp;
  }
  else
      while(index)
          if(index->Next)
              index = index->Next;
          else
          {
              index->Next = temp;
              break;
          }
  return temp->Id;
}

long XtMenu::AddMenuSubItem(const char *__Text, const char *__KeyText, int __Key, int __Command, long __Id)
{
  MenuItem *index = Items;

  index = _LookForId(Items, __Id);
  if (index == 0)
      return 0;

  MenuItem *temp = new MenuItem;
  if(__Text)
    {
    temp->Text = new char[strlen(__Text) + 1];
    strcpy(temp->Text, __Text);
    }
   else
    temp->Text = 0;
  if(__KeyText)
    {
    temp->KeyText = new char[strlen(__KeyText) + 1];
    strcpy(temp->KeyText, __KeyText);
    }
   else
    temp->KeyText = 0;
  temp->Key = __Key;
  temp->Command = __Command;
  temp->Value = 0;
  temp->Parent = 0;
  temp->Child = 0;
  temp->ChildWindow = 0;
  temp->Next = 0;
  temp->Id = LastId++;
  if(!index->Child)
  {
      index->Value = MENU_ITEM_CHILD_MENU;
      temp->Parent = index;
      index->Child = temp;
  }
  else
  {
      index = index->Child;
      while(index)
          if(index->Next)
              index = index->Next;
          else
          {
              index->Next = temp;
              break;
          }
  }

  return temp->Id;
}

long XtMenu::AddNewLine(long __Id)
{
  MenuItem *index = Items;

  index = _LookForId(Items, __Id);
  if (index == 0)
      return 0;

  MenuItem *temp = new MenuItem;
  temp->Text = new char[2];
  strcpy(temp->Text, "-");
  temp->KeyText = 0;
  temp->Key = 0;
  temp->Command = 0;
  temp->Value = MENU_ITEM_SEPARATOR;
  temp->Parent = 0;
  temp->Child = 0;
  temp->ChildWindow = 0;
  temp->Next = 0;
  temp->Id = LastId++;
  if(!index->Child)
  {
      index->Value = MENU_ITEM_CHILD_MENU;
      temp->Parent = index;
      index->Child = temp;
  }
  else
  {
      index = index->Child;
      while(index)
          if(index->Next)
              index = index->Next;
          else
          {
              index->Next = temp;
              break;
          }
  }

  return temp->Id;
}

void XtMenu::DeleteAllItem(MenuItem *__Menu)
{
  MenuItem *index = __Menu, *temp;
  while(index)
    {
    if(index->Child)
      DeleteAllItem(index->Child);
    temp = index;
    index = index->Next;
    if(temp->Text)
      delete temp->Text;
    if(temp->KeyText)
      delete temp->KeyText;
    delete temp;
    }
  Items = 0;
}

void XtMenu::DeleteItem(MenuItem *__Menu)
{
  MenuItem *index = __Menu, *temp;
  while(index)
    {
    if(index->Child)
      DeleteItem(index->Child);
    temp = index;
    index = index->Next;
    if(temp->Text)
      delete temp->Text;
    if(temp->KeyText)
      delete temp->KeyText;
    delete temp;
    }
}

void XtMenu::DeleteMenuItem(long __Id)
{
  MenuItem *index = Items, *temp, *doit;

  index = _LookForId(Items, __Id);
  if (index == 0)
      return;

  temp = _LookForItem(Items, index);

  if (index->Parent)
  {
      doit = index->Next;
      if (doit)
      {
          doit->Parent = index->Parent;
          doit = index->Parent;
          doit->Child = index->Next;
      }
      else
      {
          doit = index->Parent;
          doit->Value &= ~MENU_ITEM_CHILD_MENU;
          doit->Child = 0;
      }
      
  }
  else
      if(temp)
          temp->Next = index->Next;

  if (Items == index)
      Items = index->Next;

  index->Next = 0;
  DeleteItem(index);
}

void XtMenu::Draw()
{
  MenuItem *index = Items, *child = 0;
  unsigned i = 0, j = 0;
  XPoint child_p;
  std::string s;
  XSetColor(GetPalEntry(P_MENU_SURFACE), GetPalEntry(P_MENU_SURFACE + 1));
  XFillWindow(Bounds, ' ');
  for(; index; index = index->Next)
    if(!(index->Value & MENU_ITEM_DISABLED) || !(Flags & HIDE_DISABLED_ITEMS))
      {
      s = index->Text;
      if(index->Value & MENU_ITEM_CHILD_OPEN)
        {
        child = index;
        child_p = XPoint(Bounds.a.x + i, Bounds.a.y + j);
        }
      if(Flags & VERTICAL_MENU)
        {
        DrawItem(XPoint(Bounds.a.x + i, Bounds.a.y + j), s.length() - (s.find(SHORTCUT_LETTER) != -1) * 2, index);
        j += MENU_ENTRIES_VERTICAL_SPACING;
        }
       else
        i += DrawItem(XPoint(Bounds.a.x + i, Bounds.a.y + j), s.length() - (s.find(SHORTCUT_LETTER) != -1) * 2, index) + MENU_ENTRIES_HORIZONTAL_SPACING + 1;
      }
  if(child)
    DrawChildMenu(child_p, child);
  if(Help)
    {
    if(GetState(SM_FOCUSED) && (CurrentItem->Value & MENU_ITEM_SELECTED))
      Help->SetContext(CurrentItem->Command);
     else
      Help->SetContext(0);
    Help->Draw();
    }
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE, true);
}

void XtMenu::DrawChildMenu(XPoint __p, MenuItem *__Item)
{
  MenuItem *index = __Item->Child, *child = 0;
  unsigned i = 0, j = 1, max_tlen = 0, max_klen = 0, max_len;
  XPoint child_p;
  std::string s;
  while(index)
    {
    if(!(index->Value & MENU_ITEM_DISABLED) || !(Flags & HIDE_DISABLED_ITEMS))
      {
      s = index->Text;
      if(!(index->Value & MENU_ITEM_SEPARATOR) && s.length() - (s.find(SHORTCUT_LETTER) != -1) * 2 > max_tlen)
        max_tlen = s.length() - (s.find(SHORTCUT_LETTER) != -1) * 2;
      if(!(index->Value & MENU_ITEM_SEPARATOR) && strlen(index->KeyText) > max_klen)
        max_klen = strlen(index->KeyText);
      i++;
      }
    index = index->Next;
    }
  max_len = max_tlen + max_klen + 2;
  if(__p.x + max_len + 5 > Bounds.b.x)
    __p.x = Bounds.b.x - max_len - 5;
  if(__Item->ChildWindow)
    __Item->ChildWindow->Draw();
   else
    {
    __Item->ChildWindow = new XtWindow(XRect(__p.x, __p.y + 1, __p.x + max_len + 3, __p.y + i + 2), "\0");
    __Item->ChildWindow->SetState(SM_NO_REFRESH | WM_HAVE_SHADOW, true);
    __Item->ChildWindow->SetPalette(P_WINDOW_MENU, P_WINDOW_LEN);
    __Item->ChildWindow->SetFrame(FS_DEFAULT);
    __Item->ChildWindow->Draw();
    }
  for(index = __Item->Child; index; index = index->Next)
    if(!(index->Value & MENU_ITEM_DISABLED) || !(Flags & HIDE_DISABLED_ITEMS))
      {
      if(index->Value & MENU_ITEM_SEPARATOR)
        {
        XSetColor(GetPalEntry(P_MENU_SURFACE), GetPalEntry(P_MENU_SURFACE + 1));
        XPrintChar(XPoint(__p.x, __p.y + 1 + j), ACS_LTEE);
        for(i = __p.x + 1; i <= __p.x + max_len + 3 - 1; i++)
          XPrintChar(XPoint(i, __p.y + 1 + j), ACS_HLINE);
        XPrintChar(XPoint(__p.x + max_len + 3, __p.y + 1 + j), ACS_RTEE);
        }
       else
        {
        if(index->Value & MENU_ITEM_CHILD_OPEN)
          {
          child = index;
          child_p = XPoint(__p.x + 1, __p.y + 1 + j);
          }
        DrawItem(XPoint(__p.x, __p.y + 1 + j), max_len, index);
        }
      j++;
      }
  if(child)
    DrawChildMenu(child_p, child);
}

unsigned XtMenu::DrawItem(XPoint __p, unsigned __width, MenuItem *__Item)
{
  unsigned i, j = 1;
  if(GetState(SM_FOCUSED) && (__Item->Value & MENU_ITEM_SELECTED))
    if(Flags & VERTICAL_MENU)
      XColorWindow(XRect(Bounds.a.x + 1, __p.y, Bounds.b.x - 1, __p.y), GetPalEntry(P_MENU_SELECTED), GetPalEntry(P_MENU_SELECTED + 1));
     else
      XColorWindow(XRect(__p.x + 1, __p.y, __p.x + __width + 2, __p.y), GetPalEntry(P_MENU_SELECTED), GetPalEntry(P_MENU_SELECTED + 1));
  for(i = 0; i < strlen(__Item->Text); i++)
    {
    if(__Item->Text[i] == SHORTCUT_LETTER)
      {
      if(__Item->Value & MENU_ITEM_DISABLED)
        XSetColor(GetPalEntry(P_MENU_DISABLED), GetPalEntry(P_MENU_DISABLED + 1));
       else
        {
        if(GetState(SM_FOCUSED) && (__Item->Value & MENU_ITEM_SELECTED))
          XSetColor(GetPalEntry(P_MENU_SHORTCUT_SELECTED), GetPalEntry(P_MENU_SHORTCUT_SELECTED + 1));
         else
          XSetColor(GetPalEntry(P_MENU_SHORTCUT), GetPalEntry(P_MENU_SHORTCUT + 1));
	}
      XPrintChar(XPoint(__p.x + ++j, __p.y), __Item->Text[++i]);
      i++;
      }
     else
      {
      if(GetState(SM_FOCUSED) && (__Item->Value & MENU_ITEM_SELECTED))
	XSetColor(GetPalEntry(P_MENU_SELECTED), GetPalEntry(P_MENU_SELECTED + 1));
       else
        if(__Item->Value & MENU_ITEM_DISABLED)
	  XSetColor(GetPalEntry(P_MENU_DISABLED), GetPalEntry(P_MENU_DISABLED + 1));
	 else
	  XSetColor(GetPalEntry(P_MENU_SURFACE), GetPalEntry(P_MENU_SURFACE + 1));
      XPrintChar(XPoint(__p.x + ++j, __p.y), __Item->Text[i]);
      }
    }
  if(GetState(SM_FOCUSED) && (__Item->Value & MENU_ITEM_SELECTED))
    {
    XSetColor(GetPalEntry(P_MENU_SELECTED), GetPalEntry(P_MENU_SELECTED + 1));
    XPrintChar(XPoint(__p.x + 1, __p.y), ' ');
    XPrintChar(XPoint(__p.x + j + 1, __p.y), ' ');
    }
  if(__Item->KeyText)
    if(__Item->Value & MENU_ITEM_CHILD_MENU)
      XPrintChar(XPoint(__p.x + __width + 1, __p.y), ACS_RARROW);
     else
      XPrint(XPoint(__p.x + __width - strlen(__Item->KeyText) + 2, __p.y), __Item->KeyText);
  return j;
}

XtContextualHelp *XtMenu::GetHelp()
{
  return Help;
}

MenuItem *XtMenu::GetValue()
{
  return Items;
}

bool XtMenu::ProcessCommand(XMessage *__Event)
{
  switch(__Event->Message)
    {
    case CM_MENU:
      __Event->Class = EV_MESSAGE;
      __Event->Message = MT_RESUME_FOCUS;
      ProcessMessage(__Event);
      return 1;
    }
  return 0;
}

bool XtMenu::ProcessEvent(XEvent *__Event)
{
  return XObjectEventListener::ProcessEvent(__Event);
}

bool XtMenu::ProcessMessage(XMessage *__Event)
{
  MenuItem *temp;
  if(!Items)
    return false;
  switch(__Event->Message)
    {
    case MT_RELEASE_FOCUS:
      SetState(SM_FOCUSED, false);
      Draw();
      return true;
    case MT_RESUME_FOCUS:
      CurrentItem = Items;
      for(temp = _FindFirstChild(Items); temp && !(temp->Value & MENU_ITEM_SELECTED); temp = temp->Next);
      if(temp)
	CurrentItem = temp;
       else
	CurrentItem->Value |= MENU_ITEM_SELECTED;
      SetState(SM_FOCUSED, true);
      Draw();
      return true;
    }
  return false;
}

bool XtMenu::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  XMessage *msg;
  XKeyboardEvent *event;
  MenuItem *temp, *old;
  bool value;
  std::string s;
  if(!Items)
    return false;
  switch(__Event->KeyCode)
    {
    case KB_RIGHT:
      if(!GetState(SM_FOCUSED))
        return false;
      for(CurrentItem = Items; CurrentItem && !(CurrentItem->Value & MENU_ITEM_SELECTED); CurrentItem = CurrentItem->Next);
      CurrentItem->Value &= ~MENU_ITEM_SELECTED;
      value = false;
      if(CurrentItem->Value & MENU_ITEM_CHILD_OPEN)
        {
        value = true;
        while((temp = _FindFirstChild(Items)) != Items)
	  {
	  delete temp->Parent->ChildWindow;
	  XRefresh();
	  temp->Parent->ChildWindow = 0;
          temp->Parent->Value &= ~MENU_ITEM_CHILD_OPEN;
	  }
	}
      CurrentItem = CurrentItem->Next;
      for(; CurrentItem && (CurrentItem->Value & MENU_ITEM_DISABLED); CurrentItem = CurrentItem->Next);
      if(!CurrentItem)
        CurrentItem = Items;
      for(temp = CurrentItem; temp && (temp->Value & MENU_ITEM_DISABLED); temp = temp->Next);
      if(!temp)
	return true;
      CurrentItem = temp;
      CurrentItem->Value |= MENU_ITEM_SELECTED;
      if(value && (CurrentItem->Value & MENU_ITEM_CHILD_MENU))
        {
	event = new XKeyboardEvent;
	event->KeyCode = KB_ENTER;
	ProcessKeyboardEvent(event);
	delete event;
	}
      Draw();
      return true;
    case KB_LEFT:
      if(!GetState(SM_FOCUSED))
        return false;
      for(temp = Items; temp && !(temp->Value & MENU_ITEM_SELECTED); temp = temp->Next);
      CurrentItem = temp;
      CurrentItem->Value &= ~MENU_ITEM_SELECTED;
      value = false;
      if(CurrentItem->Value & MENU_ITEM_CHILD_OPEN)
        {
        value = true;
        while((temp = _FindFirstChild(Items)) != Items)
	  {
	  delete temp->Parent->ChildWindow;
	  XRefresh();
	  temp->Parent->ChildWindow = 0;
          temp->Parent->Value &= ~MENU_ITEM_CHILD_OPEN;
	  }
	}
      temp = Items;
      for(; temp && (temp->Value & MENU_ITEM_DISABLED); temp = temp->Next);
      if(!temp)
        return true;
      if(CurrentItem != temp)
        {
        old = CurrentItem;
	for(CurrentItem = Items; CurrentItem->Next != old; CurrentItem = CurrentItem->Next);
	}
       else
        for(CurrentItem = Items; CurrentItem->Next; CurrentItem = CurrentItem->Next);
      while(CurrentItem->Value & MENU_ITEM_DISABLED)
        {
	old = CurrentItem;
	for(CurrentItem = temp; CurrentItem->Next != old; CurrentItem = CurrentItem->Next);
	}
      CurrentItem->Value |= MENU_ITEM_SELECTED;
      if(value && (CurrentItem->Value & MENU_ITEM_CHILD_MENU))
        {
	event = new XKeyboardEvent;
	event->KeyCode = KB_ENTER;
	ProcessKeyboardEvent(event);
	delete event;
	}
      Draw();
      return true;
    case KB_DOWN:
      if(!GetState(SM_FOCUSED))
        return false;
      CurrentItem->Value &= ~MENU_ITEM_SELECTED;
      CurrentItem = CurrentItem->Next;
      for(; CurrentItem && ((CurrentItem->Value & MENU_ITEM_SEPARATOR) || (CurrentItem->Value & MENU_ITEM_DISABLED)); CurrentItem = CurrentItem->Next);
      if(!CurrentItem)
        CurrentItem = _FindFirstChild(Items);
      for(temp = CurrentItem; temp && ((temp->Value & MENU_ITEM_SEPARATOR) || (temp->Value & MENU_ITEM_DISABLED)); temp = temp->Next);
      if(!temp)
	return true;
      CurrentItem = temp;
      CurrentItem->Value |= MENU_ITEM_SELECTED;
      Draw();
      return true;
    case KB_UP:
      if(!GetState(SM_FOCUSED))
        return false;
      CurrentItem->Value &= ~MENU_ITEM_SELECTED;
      temp = _FindFirstChild(Items);
      for(; temp && ((temp->Value & MENU_ITEM_SEPARATOR) || (temp->Value & MENU_ITEM_DISABLED)); temp = temp->Next);
      if(!temp)
        return true;
      if(CurrentItem != temp)
        {
	old = CurrentItem;
	for(CurrentItem = temp; CurrentItem->Next != old; CurrentItem = CurrentItem->Next);
	}
       else
        for(CurrentItem = temp; CurrentItem->Next; CurrentItem = CurrentItem->Next);
      while((CurrentItem->Value & MENU_ITEM_SEPARATOR) || (CurrentItem->Value & MENU_ITEM_DISABLED))
        {
	old = CurrentItem;
	for(CurrentItem = temp; CurrentItem->Next != old; CurrentItem = CurrentItem->Next);
	}
      CurrentItem->Value |= MENU_ITEM_SELECTED;
      Draw();
      return true;
    case KB_PPAGE:
    case KB_HOME:
      if(!GetState(SM_FOCUSED))
        return false;
      CurrentItem->Value &= ~MENU_ITEM_SELECTED;
      temp = _FindFirstChild(Items);
      for(; temp && ((temp->Value & MENU_ITEM_SEPARATOR) || (temp->Value & MENU_ITEM_DISABLED)); temp = temp->Next);
      if(!temp)
        return true;
      CurrentItem = temp;
      CurrentItem->Value |= MENU_ITEM_SELECTED;
      Draw();
      return true;
    case KB_NPAGE:
    case KB_END:
      if(!GetState(SM_FOCUSED))
        return true;
      CurrentItem->Value &= ~MENU_ITEM_SELECTED;
      for(; CurrentItem->Next; CurrentItem = CurrentItem->Next);
      if(((CurrentItem->Value & MENU_ITEM_SEPARATOR) || (CurrentItem->Value & MENU_ITEM_DISABLED)))
	for(CurrentItem = _FindFirstChild(Items); CurrentItem && ((CurrentItem->Value & MENU_ITEM_SEPARATOR) || (CurrentItem->Value & MENU_ITEM_DISABLED)); CurrentItem = CurrentItem->Next);
      CurrentItem->Value |= MENU_ITEM_SELECTED;
      Draw();
      return true;
    case KB_ENTER:
      if(!GetState(SM_FOCUSED))
        return false;
      if(CurrentItem->Value & MENU_ITEM_CHILD_MENU)
	{
	CurrentItem->Value |= MENU_ITEM_CHILD_OPEN;
	CurrentItem = CurrentItem->Child;
	for(temp = _FindFirstChild(Items); temp && !(temp->Value & MENU_ITEM_SELECTED); temp = temp->Next);
	if(!temp)
	  {
	  for(temp = CurrentItem; temp && (temp->Value & MENU_ITEM_DISABLED); temp = temp->Next);
	  if(!temp)
	    {
	    CurrentItem = CurrentItem->Parent;
	    CurrentItem->Value &= ~MENU_ITEM_CHILD_OPEN;
	    return true;
	    }
	  }
        CurrentItem = temp;
        CurrentItem->Value |= MENU_ITEM_SELECTED;
	Draw();
	}
       else
	{
	msg = new XMessage;
	msg->Class = EV_COMMAND;
	msg->Message = CurrentItem->Command;
	_PutEventInFront(msg);
	while((temp = _FindFirstChild(Items)) != Items)
	  {
	  delete temp->Parent->ChildWindow;
	  XRefresh();
	  temp->Parent->ChildWindow = 0;
          temp->Parent->Value &= ~MENU_ITEM_CHILD_OPEN;
	  }
	SetState(SM_FOCUSED, false);
        Draw();
	}
      return true;
    case KB_ESC:
      if(!GetState(SM_FOCUSED))
        return false;
      temp = _FindFirstChild(Items);
      if(temp->Parent)
        {
        CurrentItem = temp->Parent;
        if(temp->Parent->ChildWindow)
          {
          delete CurrentItem->ChildWindow;
          CurrentItem->ChildWindow = 0;
          CurrentItem->Value &= ~MENU_ITEM_CHILD_OPEN;
          }
	}
       else
        SetState(SM_FOCUSED, false);
      Draw();
      return true;
    default:
      for(temp = _FindFirstChild(Items); temp; temp = temp->Next)
        {
	s = temp->Text;
	for(int i = 0; i < s.length(); i++)
	  s[i] = tolower(s[i]);
	if(s.find(SHORTCUT_LETTER) != -1 &&
	  s[s.find(SHORTCUT_LETTER) + 1] == __Event->KeyCode &&
	  !(temp->Value & MENU_ITEM_DISABLED))
	  {
          SetState(SM_FOCUSED, true);
	  CurrentItem->Value &= ~MENU_ITEM_SELECTED;
	  CurrentItem = temp;
	  CurrentItem->Value |= MENU_ITEM_SELECTED;
	  event = new XKeyboardEvent;
	  event->KeyCode = KB_ENTER;
	  ProcessKeyboardEvent(event);
	  delete event;
	  return true;
	  }
	}
      if((temp = _LookForKeycode(Items, __Event->KeyCode)) && !(temp->Value & MENU_ITEM_DISABLED))
        {
	while((CurrentItem = _FindFirstChild(Items)) != Items)
	  {
	  delete CurrentItem->Parent->ChildWindow;
	  XRefresh();
	  CurrentItem->Parent->ChildWindow = 0;
          CurrentItem->Parent->Value &= ~MENU_ITEM_CHILD_OPEN;
	  }
	for(; CurrentItem && !(CurrentItem->Value & MENU_ITEM_SELECTED); CurrentItem = CurrentItem->Next);
	if(CurrentItem && (CurrentItem->Value & MENU_ITEM_SELECTED))
	  CurrentItem->Value &= ~MENU_ITEM_SELECTED;
	CurrentItem = temp;
	if(CurrentItem->Value & MENU_ITEM_CHILD_MENU)
	  {
          CurrentItem->Value |= MENU_ITEM_SELECTED;
	  SetState(SM_FOCUSED, true);
	  event = new XKeyboardEvent;
	  event->KeyCode = KB_ENTER;
	  ProcessKeyboardEvent(event);
	  delete event;
	  }
	 else
	  {
	  msg = new XMessage;
	  msg->Class = EV_COMMAND;
	  msg->Message = CurrentItem->Command;
	  _PutEventInFront(msg);
	  CurrentItem = Items;
	  Draw();
	  }
	return true;
	}
    }
  return false;
}

bool XtMenu::ProcessMouseEvent(XMouseEvent *__Event)
{
  XKeyboardEvent *event;
  XRect r;
  MenuItem *index, *temp;
  std::string s;
  unsigned i;
  if(!Items)
    return false;
  switch(__Event->Type)
    {
    case EV_MOUSEDOWN:
      if(Flags & VERTICAL_MENU)
        {
	i = Bounds.a.y;
        for(index = Items; index; index = index->Next)
	  {
          if(!(index->Value & MENU_ITEM_DISABLED) || !(Flags & HIDE_DISABLED_ITEMS))
	    if(__Event->Position.y == i && __Event->Position.x >= Bounds.a.x && __Event->Position.x <= Bounds.b.x)
	      {
	      for(CurrentItem = Items; CurrentItem && !(CurrentItem->Value & MENU_ITEM_SELECTED); CurrentItem = CurrentItem->Next);
	        if(CurrentItem)
	          {
	          CurrentItem->Value &= ~MENU_ITEM_SELECTED;
                  if(CurrentItem->Value & MENU_ITEM_CHILD_OPEN)
                    while((temp = _FindFirstChild(Items)) != Items)
	              {
	              delete temp->Parent->ChildWindow;
		      XRefresh();
	              temp->Parent->ChildWindow = 0;
                      temp->Parent->Value &= ~MENU_ITEM_CHILD_OPEN;
	              }
		  }
	      index->Value |= MENU_ITEM_SELECTED;
	      CurrentItem = index;
	      SetState(SM_FOCUSED, true);
	      event = new XKeyboardEvent;
	      event->KeyCode = KB_ENTER;
	      ProcessKeyboardEvent(event);
	      delete event;
	      return true;
	      }
	  i += MENU_ENTRIES_VERTICAL_SPACING;
	  }
        }
       else
        if(__Event->Position.y == Bounds.a.y)
          {
	  i = Bounds.a.x + 1;
          for(index = Items; index; index = index->Next)
            if(!(index->Value & MENU_ITEM_DISABLED) || !(Flags & HIDE_DISABLED_ITEMS))
              {
	      s = index->Text;
	      if(__Event->Position.x >= i && __Event->Position.x <= i + s.length() - (s.find(SHORTCUT_LETTER) != -1) * 2 + 1)
	        {
	        if(!(index->Value & MENU_ITEM_SELECTED) || !(index->Value & MENU_ITEM_CHILD_OPEN))
	          {
	          for(CurrentItem = Items; CurrentItem && !(CurrentItem->Value & MENU_ITEM_SELECTED); CurrentItem = CurrentItem->Next);
	          if(CurrentItem)
	            {
	            CurrentItem->Value &= ~MENU_ITEM_SELECTED;
                    if(CurrentItem->Value & MENU_ITEM_CHILD_OPEN)
                      while((temp = _FindFirstChild(Items)) != Items)
	                {
	                delete temp->Parent->ChildWindow;
		        XRefresh();
	                temp->Parent->ChildWindow = 0;
                        temp->Parent->Value &= ~MENU_ITEM_CHILD_OPEN;
	                }
		    }
	          index->Value |= MENU_ITEM_SELECTED;
	          CurrentItem = index;
	          SetState(SM_FOCUSED, true);
	          event = new XKeyboardEvent;
	          event->KeyCode = KB_ENTER;
	          ProcessKeyboardEvent(event);
	          delete event;
	          return true;
	          }
	        }
	      i += s.length() - (s.find(SHORTCUT_LETTER) != -1) * 2 + MENU_ENTRIES_HORIZONTAL_SPACING + 2;
	      }
          }
         else
          {
          temp = _FindFirstChild(Items);
	  if(!temp->Parent)
	    return false;
	  r = temp->Parent->ChildWindow->GetBounds();
	  r.a += XPoint(1, 1);
	  r.b -= XPoint(1, 1);
          if((temp->Parent->Value & MENU_ITEM_CHILD_OPEN)
	    && r.Contains(__Event->Position))
            {
	    for(i = 0, index = temp; i < __Event->Position.y - temp->Parent->ChildWindow->GetBounds().a.y - 1; i++, index = index->Next);
	    if((index->Value & MENU_ITEM_DISABLED) || (index->Value & MENU_ITEM_SEPARATOR))
	      return true;
	    CurrentItem->Value &= ~MENU_ITEM_SELECTED;
	    index->Value |= MENU_ITEM_SELECTED;
	    CurrentItem = index;
	    event = new XKeyboardEvent;
	    event->KeyCode = KB_ENTER;
	    ProcessKeyboardEvent(event);
	    delete event;
	    return true;
	    }
          }
      return false;
    }
  return false;
}

void XtMenu::SetHelp(XRect __r, unsigned __Alignment, unsigned long __Flags)
{
  Help = new XtContextualHelp(__r, __Alignment, __Flags);
  Help->SetPalette(P_MENU, P_MENU_LEN);
  Help->SetState(SM_NO_REFRESH, true);
  SetContextualHelp(this, 0, "");
}

void XtMenu::SetValue(MenuItem *__Items)
{
  Items = __Items;
}

unsigned STATUS_ENTRIES_SPACING	= 0;

StatusItem *NewStatusItem(char *__Text, int __Key, int __Command, StatusItem *__ANextItem)
{
  StatusItem *temp = new StatusItem;
  temp->Text = __Text;
  temp->Key = __Key;
  temp->Command = __Command;
  temp->Value = 0;
  temp->Next = __ANextItem;
  return temp;
}

void DisableCommand(StatusItem *__StatusLine, int __Command)
{
  StatusItem *index = __StatusLine;
  while(index)
    {
    if(index->Command == __Command)
      index->Value |= STATUS_ITEM_DISABLED;
    index = index->Next;
    }
}

void EnableCommand(StatusItem *__StatusLine, int __Command)
{
  StatusItem *index = __StatusLine;
  while(index)
    {
    if(index->Command == __Command && (index->Value & STATUS_ITEM_DISABLED))
      index->Value &= ~STATUS_ITEM_DISABLED;
    index = index->Next;
    }
}

XtStatusLine::XtStatusLine(XRect __r, unsigned long __Flags, StatusItem *__Items) :
  XObject(__r)
{
  strcpy(ObjClass, "XtStatusLine");
  SetPalette(P_STATUSLINE, P_STATUSLINE_LEN);
  Flags = __Flags;
  Items = __Items;
  XObject::SetState(SM_ACTIVE, true);
}

XtStatusLine::~XtStatusLine()
{
}

void XtStatusLine::Draw()
{
  StatusItem *index = Items;
  unsigned i = 0, k = 0, shortcut = 0;
  std::string s;
  XSetColor(GetPalEntry(P_STATUSLINE_SURFACE), GetPalEntry(P_STATUSLINE_SURFACE + 1));
  XFillWindow(Bounds, ' ');
  for(; index; index = index->Next)
    if(!(index->Value & STATUS_ITEM_DISABLED) || !(Flags & HIDE_DISABLED_ITEMS))
      {
      s = index->Text;
      for(k = 0; k < strlen(index->Text); k++)
        if(index->Text[k] == SHORTCUT_LETTER)
	  {
	  if(shortcut)
	    {
	    if(index->Value & STATUS_ITEM_DISABLED)
	      XSetColor(GetPalEntry(P_STATUSLINE_DISABLED), GetPalEntry(P_STATUSLINE_DISABLED + 1));
	     else
	      XSetColor(GetPalEntry(P_STATUSLINE_SURFACE), GetPalEntry(P_STATUSLINE_SURFACE + 1));
	    shortcut = 0;
	    }
	   else
	    {
            XSetColor(GetPalEntry(P_STATUSLINE_SHORTCUT), GetPalEntry(P_STATUSLINE_SHORTCUT + 1));
	    shortcut = 1;
	    }
	  }
	 else
          XPrintChar(XPoint(Bounds.a.x + ++i, Bounds.a.y), index->Text[k]);
      i += STATUS_ENTRIES_SPACING + 2;
      }
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE, true);
}

StatusItem* XtStatusLine::GetValue()
{
  return Items;
}

bool XtStatusLine::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  XMessage *msg;
  StatusItem *index;
  for(index = Items; index && index->Key != __Event->KeyCode; index = index->Next);
  if(index)
    {
    msg = new XMessage;
    msg->Class = EV_COMMAND;
    msg->Message = index->Command;
    _PutEventInFront(msg);
    return true;
    }
  return false;
}

bool XtStatusLine::ProcessMouseEvent(XMouseEvent *__Event)
{
  XMessage *msg;
  StatusItem *index;
  unsigned i = Bounds.a.x;
  std::string s;
  if(__Event->Type == EV_MOUSEDOWN && Bounds.Contains(__Event->Position))
    {
    for(index = Items; index; index = index->Next)
      {
      s = index->Text;
      if(__Event->Position.x >= i && __Event->Position.x <= i + s.length() - (s.find(SHORTCUT_LETTER) != -1) * 2 + 1)
        {
        msg = new XMessage;
        msg->Class = EV_COMMAND;
        msg->Message = index->Command;
        _PutEventInFront(msg);
        return true;
	}
      i += s.length() - (s.find(SHORTCUT_LETTER) != -1) * 2 + STATUS_ENTRIES_SPACING + 2;
      }
    return true;
    }
  return false;
}

void XtStatusLine::SetValue(StatusItem *__Items)
{
  Items = __Items;
}
