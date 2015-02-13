/*
  xtlist.cc

  Copyright (c) 1998-2002 Dragos Acostachioaie
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

#include <ctype.h>
#include <string.h>

#include <xtscreen.h>
#include <xtbutton.h>
#include <xtdialog.h>
#include <xtfield.h>
#include <xtlist.h>
#include <xttools.h>

char XtList_LastSearchText[80] = "";
unsigned long XtList_LastSearchLine;
bool XtList_SearchBackwards = false, XtList_IgnoreCase = true;

LItem *NewLItem(char *__Text, long __Value, short __Flags, LItem *__ANextItem)
{
    LItem *temp = new LItem;

    temp->Text = new char[strlen(__Text) + 1];
    strcpy(temp->Text, __Text);
    temp->Value = __Value;
    temp->Flags = __Flags;
    temp->Next = __ANextItem;
    return temp;
}

LItem *NewList(char **__Text, unsigned __Count)
{
    unsigned long i;
    LItem *temp, *prev = 0, *head = 0;

    for(i = 0; i < __Count; i++)
	{
	temp = new LItem;
	temp->Text = new char[strlen(__Text[i]) + 1];
	strcpy(temp->Text, __Text[i]);
	temp->Value = 0;
	temp->Flags = 0;
	temp->Next = 0;
	if(!head)
	    head = temp;
	if(prev)
	    prev->Next = temp;
	prev = temp;
	temp = temp->Next;
	}
    return head;
}

XtList::XtList(XRect __r, unsigned long __Flags, LItem *__Items) :
    XObject(__r)
{
    Items = __Items;
    ItemsNo = GetItemsNumber();
    ColsNo = GetColsNumber();
    CurrIndex = Items;
    CurrItem = 1;
    CP = CC = CurY = 1;
    Flags = __Flags;
    max_selected = 1;
    CountSelected();
    strcpy(ObjClass, "XtList");
    SetPalette(P_LIST, P_LIST_LEN);
    InitScrollBar();
    XObject::SetState(SM_ACTIVE, true);
    HeaderLine = 0;
    FooterLine = 0;
}

XtList::~XtList()
{
    LItem *index = Items;

    while(Items)
	{
	index = index->Next;
	delete Items->Text;
	delete Items;
	Items = index;
	}
    if(VScrollBar)
	delete VScrollBar;
    if(HScrollBar)
	delete HScrollBar;
    if(HeaderLine)
	delete HeaderLine;
    if(FooterLine)
	delete FooterLine;
}

void XtList::AddFirst(const char *__Text, long __Value, short __Flags)
{
    LItem *temp = new LItem;

    temp->Text = new char[strlen(__Text) + 1];
    strcpy(temp->Text, __Text);
    temp->Value = __Value;
    temp->Flags = __Flags;
    temp->Next = Items;
    Items = temp;
    ItemsNo++;
    CurrIndex = Items;
    if(strlen(__Text) > ColsNo)
	ColsNo = strlen(__Text);
    SetScrollBar();
}

void XtList::AddFirst(const char *__Text)
{
    AddFirst(__Text, 0, 0);
}

void XtList::AddFooter(const char *__Text)
{
    FooterLine = new char[strlen(__Text) + 1];
    strcpy(FooterLine, __Text);
    Flags |= FOOTER_LINE;
}

void XtList::AddHeader(const char *__Text)
{
    HeaderLine = new char[strlen(__Text) + 1];
    strcpy(HeaderLine, __Text);
    Flags |= HEADER_LINE;
}

void XtList::AddLast(const char *__Text, long __Value, short __Flags)
{
    LItem *index = Items;

    LItem *temp = new LItem;
    temp->Text = new char[strlen(__Text) + 1];
    strcpy(temp->Text, __Text);
    temp->Value = __Value;
    temp->Flags = __Flags;
    temp->Next = 0;

    if(!index)
	{
        Items = temp;
	ColsNo = strlen(__Text);
        ItemsNo++;
        CurrIndex = Items;
	}
    else
        while(index)
            if(index->Next)
                index = index->Next;
            else
        	{
                index->Next = temp;
		if(strlen(__Text) > ColsNo)
		    ColsNo = strlen(__Text);
                ItemsNo++;
                break;
        	}
    SetScrollBar();
}

void XtList::AddLast(const char *__Text)
{
    AddLast(__Text, 0, 0);
}

void XtList::ClearSelected()
{
  LItem *index = Items;
  while(index)
    {
    DeSelect(index);
    index = index->Next;
    }
}

void XtList::CountSelected()
{
    LItem *index = Items;

    number_selected = 0;
    if(Flags & MAX_SELECTED)
	{
        index = Items;
        while(index)
    	    {
            if(index->Flags & ITEM_SELECTED)
                if(NumberSelected() < MaxSelected())
                    IncNumberSelected();
                else
                    index->Flags &= ~ITEM_SELECTED;
            index = index->Next;
    	    }
	}
}

void XtList::DeSelect(LItem *__Item)
{
  if(__Item->Flags & ITEM_SELECTED)
    {
    __Item->Flags &= ~ITEM_SELECTED;
    DecNumberSelected();
    }
}

void XtList::DecNumberSelected()
{
  number_selected--;
}

bool XtList::DeleteItem(unsigned long __Item)
{
    unsigned long i = 1;
    LItem *index = Items;
    LItem *temp = Items;

    if(!__Item)
        return false;
    while(temp)
	{
        if(__Item == i)
    	    {
            delete temp->Text;
            ItemsNo--;
            index->Next = temp->Next;
            ColsNo = GetColsNumber();
            if(CurrItem > ItemsNo)
                CurrItem = ItemsNo;
            SetScrollBar();
            if (__Item == 1)
            {
                Items = temp->Next;
                CurrIndex = Items;
            }
	    if(index->Next == 0)
		CurY--;
	    if(CurY == 0)
		{
		CurY++;
		CP--;
		}
            delete temp;
            return true;
    	    }
	i++;
        index = temp;
        temp = temp->Next;
	}

    return false;
}

void XtList::Draw()
{
  unsigned long i, count;
  LItem *temp = Items;
  char *s;

  if(VScrollBar && !VScrollBar->GetState(SM_VISIBLE))
    VScrollBar->Draw();
  if(HScrollBar && !HScrollBar->GetState(SM_VISIBLE))
    HScrollBar->Draw();
  XColorWindow(XRect(Bounds.a.x, Bounds.a.y, Bounds.b.x - 1, Bounds.b.y - 1), GetPalEntry(P_LIST_SURFACE), GetPalEntry(P_LIST_SURFACE + 1));
  AddLine = 0;
  if (Flags & HEADER_LINE)
  {
      s = new char[strlen(HeaderLine) + Size.x - 2];
      if(strlen(HeaderLine) < CC - 1)
          strcpy(s, " ");
      else
          strcpy(s, HeaderLine + CC - 1);
      for(unsigned c = strlen(s); c <= Size.x - 2; c++)
          s[c] = ' ';
      s[Size.x - 2] = '\0';
      for(unsigned c = 0; c < Size.x - 2; c++)
          XPrintChar(XPoint(Bounds.a.x + c, Bounds.a.y), s[c]);
      delete s;
      AddLine = 1;
  }
  if(GetState(SM_FOCUSED) && ItemsNo > 0)
    if(CurrIndex->Flags & ITEM_DISABLED)
      XColorWindow(XRect(Bounds.a.x, Bounds.a.y + CurY - 1 + AddLine, Bounds.b.x - 1, Bounds.a.y + CurY - 1 + AddLine), GetPalEntry(P_LIST_SELECTED_DISABLED), GetPalEntry(P_LIST_SELECTED_DISABLED + 1));
    else
      XColorWindow(XRect(Bounds.a.x, Bounds.a.y + CurY - 1 + AddLine, Bounds.b.x - 1, Bounds.a.y + CurY - 1 + AddLine), GetPalEntry(P_LIST_SELECTED), GetPalEntry(P_LIST_SELECTED + 1));
  for(i = 1; i < CP; i++)
    temp = temp->Next;
  i = 1;
  count = Size.y - 1;
  if (Flags & HEADER_LINE)
      count --;
  if (Flags & FOOTER_LINE)
      count --;

  while(temp && i <= count)
    {
    if(!(temp->Flags & ITEM_INVISIBLE))
	{
	if(i == CurrItem - CP + 1 && GetState(SM_FOCUSED))
	    if(temp->Flags & ITEM_DISABLED)
    		XSetColor(GetPalEntry(P_LIST_SELECTED_DISABLED), GetPalEntry(P_LIST_SELECTED_DISABLED + 1));
	    else
    		XSetColor(GetPalEntry(P_LIST_SELECTED), GetPalEntry(P_LIST_SELECTED + 1));
        else
	    if(temp->Flags & ITEM_DISABLED)
    		XSetColor(GetPalEntry(P_LIST_DISABLED), GetPalEntry(P_LIST_DISABLED + 1));
	    else
		XSetColor(GetPalEntry(P_LIST_SURFACE), GetPalEntry(P_LIST_SURFACE + 1));
	s = new char[strlen(temp->Text) + Size.x - 2];
	if(strlen(temp->Text) < CC - 1)
	    strcpy(s, " ");
	else
	    strcpy(s, temp->Text + CC - 1);
	for(unsigned c = strlen(s); c <= Size.x - 2; c++)
	    s[c] = ' ';
	s[Size.x - 2] = '\0';
	for(unsigned c = 0; c < Size.x - 2; c++)
	    XPrintChar(XPoint(Bounds.a.x + c + 1, Bounds.a.y + i - 1 + AddLine), s[c]);
	delete s;
	if((Flags & (MAX_SELECTED | SELECTABLE_ITEMS)) && (temp->Flags & ITEM_SELECTED))
	    XPrintChar(XPoint(Bounds.a.x, Bounds.a.y + i - 1 + AddLine), ACS_DIAMOND);
	i++;
	}
    temp = temp->Next;
    }

  if (Flags & FOOTER_LINE)
  {
      XSetColor(GetPalEntry(P_LIST_SURFACE), GetPalEntry(P_LIST_SURFACE + 1));
      s = new char[strlen(FooterLine) + Size.x - 2];
      if(strlen(FooterLine) < CC - 1)
          strcpy(s, " ");
      else
          strcpy(s, FooterLine + CC - 1);
      for(unsigned c = strlen(s); c <= Size.x - 2; c++)
          s[c] = ' ';
      s[Size.x - 2] = '\0';
      for(unsigned c = 0; c < Size.x - 2; c++)
          XPrintChar(XPoint(Bounds.a.x + c, Bounds.b.y - 1), s[c]);
      delete s;
  }
  if(VScrollBar)
    VScrollBar->SetValue(CurrItem - 1);
  if(HScrollBar)
    HScrollBar->SetValue(CC);
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE, true);
}

unsigned long XtList::GetColsNumber()
{
  LItem *index = Items;
  unsigned cols = 0;
  while(index)
    {
    if(strlen(index->Text) > cols)
      cols = strlen(index->Text);
    index = index->Next;
    }
  return cols;
}

unsigned long XtList::GetCurrentItem()
{
    return CurrItem;
}

short XtList::GetFlags(unsigned long __Item)
{
    unsigned long i = 1;
    LItem *temp = Items;

    if(__Item == 0)
       {
        while(temp)
            {
            if(CurrItem == i++)
                return temp->Flags;
            temp = temp->Next;
            }
        return -1;
       }

    while(temp)
	{
	if(__Item == i++)
	    return temp->Flags;
	temp = temp->Next;
	}
  return -1;
}

unsigned long XtList::GetItemsNumber()
{
  LItem *index = Items;
  unsigned long n = 0;
  while(index)
    {
    n++;
    index = index->Next;
    }
  return n;
}

unsigned int XtList::GetKeyCode()
{
    return KeyCode;
}

unsigned long XtList::GetSelected(unsigned long __Item)
{
    LItem *index = Items;
    unsigned long i;

    if(__Item <= ItemsNo)
	{
	if(__Item == 0)
    	    i = 1;
	else
    	    i = __Item;

	for(unsigned j = i; j > 1; j--)
	    index = index->Next;

	for(; i <= ItemsNo; i++)
    	    if(index->Flags & ITEM_SELECTED)
        	return i;
    	    else
    		index = index->Next;
	}

    return 0;
}

char *XtList::GetText(unsigned long __Item)
{
    unsigned long i = 1;
    LItem *temp = Items;

    while(temp)
	{
	if(__Item == i++)
	    return temp->Text;
	temp = temp->Next;
	}
  return 0;
}

long XtList::GetValue(unsigned long __Item)
{
    unsigned long i = 1;
    LItem *temp = Items;

    while(temp)
	{
	if(__Item == i++)
	    return temp->Value;
	temp = temp->Next;
	}
  return -1;
}

LItem* XtList::GetValue()
{
  return Items;
}

void XtList::IncNumberSelected()
{
  number_selected++;
}

void XtList::InitScrollBar()
{
  VScrollBar = HScrollBar = 0;
  if(Flags & LIST_V_SCROLLBAR)
    {
    VScrollBar = new XtScrollBar(
      XRect(Size.x - 1, 0, Size.x - 1, Size.y - 2),
      GetItemsNumber(),
      0);
    RegisterClient(VScrollBar);
    AddListener(VScrollBar->GetId(), EV_MOUSE);
    }
  if(Flags & LIST_H_SCROLLBAR)
    {
    HScrollBar = new XtScrollBar(
      XRect(0, Size.y - 1, Size.x - 2, Size.y - 1),
      GetColsNumber(),
      SB_HORIZONTAL | SB_INDICATE_PERCENT);
    RegisterClient(HScrollBar);
    }
}

bool XtList::InsertItem(unsigned long __Position, LItem *__Item)
{
    unsigned long i = 1;
    LItem *temp = Items;
    LItem *index;

    if(!__Position)
        return false;
    while(temp)
	{
        if(__Position == i++)
    	    {
            index = new LItem;
            index->Text = new char[strlen(__Item->Text) + 1];
            strcpy(index->Text, __Item->Text);
            index->Value = __Item->Value;
            index->Flags = __Item->Flags;
            index->Next = temp->Next;
            if(Flags & MAX_SELECTED && index->Flags & ITEM_SELECTED)
                if(NumberSelected() < MaxSelected())
                    IncNumberSelected();
                else
                    index->Flags &= ~ITEM_SELECTED;
            temp->Next = index;
            ItemsNo++;
            ColsNo = GetColsNumber();
            SetScrollBar();
            delete temp;
            return true;
    	    }
        temp = temp->Next;
	}
    return false;
}

unsigned long XtList::MaxSelected()
{
  return max_selected;
}

unsigned long XtList::NumberSelected()
{
  return number_selected;
}

bool XtList::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  XMessage *msg;
  unsigned long i, count;
  LItem *index;

  count = Size.y;
  if (Flags & HEADER_LINE)
      count--;
  if (Flags & FOOTER_LINE)
      count--;
  KeyCode = __Event->KeyCode;
  switch(__Event->KeyCode)
    {
    case KB_DOWN:
      if(CurrItem < ItemsNo)
	{
	CurrItem++;
	CurrIndex = CurrIndex->Next;
        if(CurY < count - 1)
	  CurY++;
	 else
	  CP++;
	Draw();
	return true;
	}
       else
	if((Flags & CIRCULAR_SELECTION) && ItemsNo < count)
	  {
	  CurrItem = 1;
	  CurrIndex = Items;
	  CP = CurY = 1;
	  Draw();
	  return true;
	  }
      break;
    case KB_UP:
      if(ItemsNo == 0)
        break;
      if(CurrItem > 1)
	{
	CurrItem--;
	CurrIndex = Items;
	for(i = 1; i < CurrItem; i++)
	  CurrIndex = CurrIndex->Next;
	if(CurY > 1)
	  CurY--;
	 else
	  CP--;
	Draw();
	return true;
	}
       else
	if((Flags & CIRCULAR_SELECTION) && ItemsNo < count)
	  {
	  CurrItem = ItemsNo;
	  CurrIndex = Items;
	  while(CurrIndex->Next)
	    CurrIndex = CurrIndex->Next;
	  CP = 1;
	  CurY = ItemsNo;
	  Draw();
	  return true;
	  }
      break;
    case KB_RIGHT:
      if((CC < ColsNo - (Size.x - 3)) && ColsNo > Size.x - 3)
        {
	CC++;
	Draw();
        return true;
	}
      break;
    case KB_LEFT:
      if(CC > 1)
        {
	CC--;
	Draw();
	return true;
	}
      break;
    case KB_SPACE:
      if((Flags & (MAX_SELECTED | SELECTABLE_ITEMS)) && !(CurrIndex->Flags & ITEM_DISABLED))
	{
	if(CurrIndex->Flags & ITEM_SELECTED)
	  DeSelect(CurrIndex);
	 else
	  Select(CurrIndex);
	Draw();
	return true;
	}
       else
        {
        XBeep();
	XRefresh();
	}
      break;
    case KB_ENTER:
      if(ItemsNo > 0 && !(CurrIndex->Flags & ITEM_DISABLED))
	{
	msg = new XMessage;
	msg->Class = EV_COMMAND;
	msg->Type = MC_CLIENT_TO_SERVER;
	msg->Message = CM_OK;
	_PutEventInFront(msg);
	return true;
	}
      break;
    case KB_ESC:
      msg = new XMessage;
      msg->Class = EV_COMMAND;
      msg->Type = MC_CLIENT_TO_SERVER;
      msg->Message = CM_CLOSE;
      _PutEventInFront(msg);
      return true;
    case KB_PPAGE:
      if(CurrItem > 1)
	{
        if(CurrItem > count)
	  {
	  CurrIndex = Items;
	  if(CurY == 1)
	    {
	    for(i = 0; i < CurrItem - count; i++)
	      CurrIndex = CurrIndex->Next;
            CurrItem -= count - 2;
            CP -= count - 2;
	    }
	   else
	    {
	    for(i = 0; i < CurrItem - CurY; i++)
	      CurrIndex = CurrIndex->Next;
	    CurrItem -= CurY - 1;
	    }
	  CurY = 1;
	  }
	 else
	  {
	  CurrItem = 1;
	  CurrIndex = Items;
	  CP = CurY = 1;
	  }
	Draw();
	return true;
	}
      break;
    case KB_NPAGE:
      if(CurrItem < ItemsNo)
	{
        if(CurrItem + count - 1 < ItemsNo)
	  {
          for(i = 0; i < count - 2; i++)
	    CurrIndex = CurrIndex->Next;
          CurrItem += count - 2;
	  if(CurY > 1)
	    CP += CurY - 1;
          CurY = count - 1;
	  }
	 else
	  {
	  CurrItem = ItemsNo;
	  CurrIndex = Items;
	  while(CurrIndex->Next)
	    CurrIndex = CurrIndex->Next;
          if(ItemsNo > count - 1)
	    {
            CP = ItemsNo - count + 2;
            CurY = count - 1;
	    }
	   else
	    {
	    CP = 1;
	    CurY = ItemsNo;
	    }
	  }
	Draw();
	return true;
	}
      break;
    case KB_HOME:
      if(CurrItem > 1)
	{
	CurrItem = 1;
	CurrIndex = Items;
	  CP = CurY = 1;
	Draw();
	return true;
	}
      break;
    case KB_END:
      if(CurrItem < ItemsNo)
	{
	CurrItem = ItemsNo;
	CurrIndex = Items;
	while(CurrIndex->Next)
	  CurrIndex = CurrIndex->Next;
        if(ItemsNo > count - 1)
	  {
          CP = ItemsNo - count + 2;
          CurY = count - 1;
	  }
	 else
	  {
	  CP = 1;
	  CurY = ItemsNo;
	  }
	Draw();
	return true;
	}
      break;
    case KB_META('a'):
      if((Flags & SELECTABLE_ITEMS))
	{
        index = Items;
	while(index)
	  {
	  if(!(index->Flags & (ITEM_DISABLED | ITEM_SELECTED | ITEM_INVISIBLE)))
	    Select(index);
	  index = index->Next;
	  }
	Draw();
	return true;
	}
      break;
    case KB_META('r'):
      if((Flags & SELECTABLE_ITEMS))
	{
        index = Items;
	while(index)
	  {
	  if(!(index->Flags & ITEM_DISABLED))
  	    if(index->Flags & ITEM_SELECTED)
	      DeSelect(index);
	     else
	      Select(index);
	  index = index->Next;
	  }
	Draw();
	return true;
	}
      break;
    case KB_META('s'):
	if(ItemsNo > 0)
	    SearchDialog(false);
	return true;
    case KB_META('n'):
	if(ItemsNo > 0)
	    SearchDialog(true);
	return true;
    default:
      if(__Event->KeyCode >= 0x20 && __Event->KeyCode <= 0xff)
        {
	index = Items;
	i = 0;
	while(index)
	  {
	  i++;
	  if(tolower(index->Text[0]) == tolower(__Event->KeyCode))
	    {
	    CurrIndex = index;
	    CurrItem = i;
            if(CurrItem > count - 1)
              {
	      CP = i;
              CurY = 1;
	      }
             else
              {
	      CP = 1;
              CurY = i;
	      }
	    Draw();
	    return true;
	    }
          index = index->Next;
	  }
	}
      break;
    }
  return false;
}

bool XtList::ProcessMessage(XMessage *__Event)
{
  XMessage *msg;
  XKeyboardEvent *ev;
  switch(__Event->Message)
    {
    case MT_RELEASE_FOCUS:
      SetState(SM_FOCUSED, false);
      return true;
    case MT_RESUME_FOCUS:
      SetState(SM_FOCUSED, true);
      return true;
    case MT_ANSWER_REQUEST:
      msg = new XMessage;
      msg->Type = MC_CLIENT_TO_SERVER;
      msg->Message = CurrItem;
      _PutEventInFront(msg);
      return true;
    case SCROLLBAR_ITEMUP:
      ev = new XKeyboardEvent;
      ev->KeyCode = KB_UP;
      ProcessKeyboardEvent(ev);
      delete ev;
      return true;
    case SCROLLBAR_ITEMDOWN:
      ev = new XKeyboardEvent;
      ev->KeyCode = KB_DOWN;
      ProcessKeyboardEvent(ev);
      delete ev;
      return true;
    }
  return false;
}

bool XtList::ProcessMouseEvent(XMouseEvent *__Event)
{
  XMessage *msg;
  unsigned long i;
  if(__Event->Type == EV_MOUSEDOWN)
    if(Bounds.Contains(__Event->Position))
      if(CP + __Event->Position.y - Bounds.a.y + AddLine <= ItemsNo)
	{
	if(__Event->Position.y == Bounds.a.y + CurY - 1 + AddLine && GetState(SM_FOCUSED) && (Flags & SELECTABLE_ITEMS))
	  {
	  if(!(CurrIndex->Flags & ITEM_DISABLED))
	    {
	    if(__Event->Clicks > 1)
	      {
	      msg = new XMessage;
	      msg->Type = MC_CLIENT_TO_SERVER;
	      msg->Message = CM_OK;
	      _PutEventInFront(msg);
	      return true;
	      }
	    if(CurrIndex->Flags & ITEM_SELECTED)
	      DeSelect(CurrIndex);
	     else
	      Select(CurrIndex);
	    Draw();
	    return true;
	    }
	  }
	 else
	  {
	  msg = new XMessage;
	  msg->Type = MC_CLIENT_TO_SERVER;
	  msg->Message = MT_RESUME_FOCUS;
	  if(Server)
	    Server->HandleEvent(msg);
          if (Flags & FOOTER_LINE)
              if(__Event->Position.y >= Bounds.a.y + Size.y - 1)
                  return false;
	  if(__Event->Position.y > Bounds.a.y + CurY - 1 + AddLine)
	    {
	    for(i = 0; i < __Event->Position.y - Bounds.a.y - CurY + 1 + AddLine; i++)
	      CurrIndex = CurrIndex->Next;
	    CurrItem = CurrItem + __Event->Position.y - Bounds.a.y - CurY + 1 + AddLine;
	    }
	   else
	    {
	    CurrIndex = Items;
	    for(i = 0; i < CurrItem - (Bounds.a.y + CurY - __Event->Position.y + AddLine); i++)
	      CurrIndex = CurrIndex->Next;
	    CurrItem = CurrItem + __Event->Position.y - Bounds.a.y - CurY + 1 + AddLine;
	    }
	  CurY = __Event->Position.y - Bounds.a.y + 1 + AddLine;
	  SetState(SM_FOCUSED, 1);
	  }
	return true;
	}
  return false;
}

int XtList::Run()
{
  int result;
  XEvent *ev;
  Draw();
  ev = new XMessage;
  ((XMessage *)ev)->Message = MT_RESUME_FOCUS;
  HandleEvent(ev);
  delete ev;
  for(;;)
    {
    ev = FetchEvent();
    HandleEvent(ev);
    if(ev && ev->Class == EV_COMMAND && (((XMessage *)ev)->Message == CM_CLOSE || ((XMessage *)ev)->Message == CM_OK))
      break;
    if(ev)
      delete ev;
    }
  result = ((XMessage *)ev)->Message;
  delete ev;
  ev = new XMessage;
  ((XMessage *)ev)->Message = MT_RELEASE_FOCUS;
  HandleEvent(ev);
  delete ev;
  return result;
}

void XtList::SearchDialog(bool __cont)
{
    XtDialogBox *dialog;
    XtInputStringField *data;
    XtCheckButton *options;
    XtButton *butt1, *butt2;
    XtMessageBox *box;
    LItem *index = Items;
    char text[80], *s = new char[ColsNo + 1];
    unsigned long i, j, k, found = 0;

    if(!__cont)
	{
	dialog = new XtDialogBox(XRect(10, 8, 70, 18), "Search");
	data = new XtInputStringField(XRect(2, 2, 50, 2), XtList_LastSearchText, 80, 0);
	dialog->RegisterClient(data);
	options = new XtCheckButton(XRect(2, 4, 35, 5),
	    NewItem("Search backwards", XtList_SearchBackwards,
	    NewItem("Ignore case", XtList_IgnoreCase,
	    0)));
	dialog->RegisterClient(options);
	butt1 = new XtButton(XRect(21, 8, 26, 8), "~O~k", BF_ARROW, CM_OK);
	dialog->RegisterClient(butt1);
	butt2 = new XtButton(XRect(31, 8, 41, 8), "~C~ancel", BF_ARROW, CM_CLOSE);
	dialog->RegisterClient(butt2);
	if(dialog->Run() == CM_CLOSE)
	    {
	    delete dialog;
	    return;
	    }
	strcpy(text, data->GetValue());
	strcpy(XtList_LastSearchText, text);
	XtList_SearchBackwards = options->GetValue(1);
	XtList_IgnoreCase = options->GetValue(2);
	delete dialog;
	XtList_LastSearchLine = CurrItem;
	}
    else
	strcpy(text, XtList_LastSearchText);
    i = XtList_LastSearchLine;
    for(j = 0; j < i; j++)
	index = index->Next;
    while(!found && i < ItemsNo)
	{
	strcpy(s, index->Text);
	for(k = 0; k < strlen(s); k++)
	    s[k] = toupper(s[k]);
	for(k = 0; k < strlen(text); k++)
	    text[k] = toupper(text[k]);
	if(strstr(s, text))
	    {
	    found = i + 1;
	    break;
	    }
	if(XtList_SearchBackwards)
	    {
	    i--;
	    index = Items;
	    for(j = 0; j < i; j++)
		index = index->Next;
	    }
	else
	    {
	    i++;
	    index = index->Next;
	    }
	}
    if(found)
	{
	XtList_LastSearchLine = CurrItem = found;
	CurrIndex = index;
	if(CurrItem < Size.y - 2)
	    {
	    CP = 1;
	    CurY = CurrItem;
	    }
	else
	    if(CurrItem + Size.y - 2 > ItemsNo)
		{
		CP = ItemsNo - (Size.y - 2);
		CurY = ItemsNo - CP - 2;
		}
	    else
		{
		CP = CurrItem;
		CurY = 1;
		}
	    Draw();
	}
    else
	{
	sprintf(s, "\"%s\"\nnot found.", text);
	box = new XtMessageBox(XRect(20, 8, 60, 16), "Search", s, OK_BUTTON);
	box->Run();
	delete box;
	}
}

void XtList::Select(LItem *__Item)
{
  if(Flags & SELECTABLE_ITEMS) {
    if( (Flags & MAX_SELECTED) && (NumberSelected() >= MaxSelected()) )
      return;
    else
      if( !(__Item->Flags & ITEM_SELECTED) )
	{
        __Item->Flags |= ITEM_SELECTED;
	IncNumberSelected();
	}
  }
}

void XtList::SetCurrentItem(LItem *__Item)
{
  LItem *index = Items;
  unsigned long i;
  if(__Item)
    {
    for(i = 1; index != __Item && index->Next; i++)
      index = index->Next;
    if(!index)
      return;
    CurrIndex = __Item;
    CurrItem = i;
    if(CurrItem > Size.y - 1)
      {
      CP = i;
      CurY = 1;
      }
    else
      {
      CP = 1;
      CurY = i;
      }
    Draw();
    }
}

void XtList::SetCurrentItem(unsigned long __Item)
{
  unsigned long i;
  CurrIndex = Items;
  if(__Item > 0 && __Item <= ItemsNo)
    {
    for(i = 1; i < __Item; i++)
      CurrIndex = CurrIndex->Next;
    CurrItem = __Item;
    if(CurrItem > Size.y - 1)
      {
      CP = i;
      CurY = 1;
      }
    else
      {
      CP = 1;
      CurY = i;
      }
    Draw();
    }
}

void XtList::SetFlags(unsigned long __Item, short __Flags)
{
    LItem *index = Items;
    unsigned long i;

    if(__Item > 0 && __Item <= ItemsNo)
	{
	for(i = 1; i < __Item; i++)
	    index = index->Next;
	index->Flags = __Flags;
	if(Flags & MAX_SELECTED && index->Flags & ITEM_SELECTED)
    	    if(NumberSelected() < MaxSelected())
        	IncNumberSelected();
    	    else
        	index->Flags &= ~ITEM_SELECTED;
	}
}

void XtList::SetMaxSelectable(unsigned long __max)
{
  max_selected = __max;
}

void XtList::SetScrollBar()
{
    if(Flags & LIST_V_SCROLLBAR)
	VScrollBar->SetTotalNo(GetItemsNumber());

    if(Flags & LIST_H_SCROLLBAR)
	HScrollBar->SetTotalNo(GetColsNumber());
}

void XtList::SetState(unsigned long __Mask, bool __BoolValue)
{
  XObject::SetState(__Mask, __BoolValue);
  if(VScrollBar)
    {
    if(__Mask & SM_VISIBLE)
      VScrollBar->SetState(SM_VISIBLE, __BoolValue);
    if(__Mask & SM_DISABLED)
      VScrollBar->SetState(SM_DISABLED, __BoolValue);
    }
  if(HScrollBar)
    {
    if(__Mask & SM_VISIBLE)
      HScrollBar->SetState(SM_VISIBLE, __BoolValue);
    if(__Mask & SM_DISABLED)
      HScrollBar->SetState(SM_DISABLED, __BoolValue);
    }

  if(GetState(SM_VISIBLE))
    Draw();
}

bool XtList::SetText(unsigned long __Item, const char *__Text)
{
    unsigned long i = 1;
    LItem *temp = Items;

    if(!__Item)
        return false;
    while(temp)
	{
        if(__Item == i++)
    	    {
            delete temp->Text;
            temp->Text = new char[strlen(__Text) + 1];
            strcpy(temp->Text, __Text);
            ColsNo = GetColsNumber();
            SetScrollBar();
            return true;
    	    }
        temp = temp->Next;
	}
    return false;
}

void XtList::SetValue(LItem *__Items)
{
  LItem *index = Items;
  while(Items)
    {
    index = index->Next;
    delete Items->Text;
    delete Items;
    Items = index;
    }
  Items = __Items;
  ItemsNo = GetItemsNumber();
  ColsNo = GetColsNumber();
  CurrIndex = Items;
  CurrItem = 1;
  CountSelected();
  CP = CC = CurY = 1;
  if(VScrollBar)
    VScrollBar->SetTotalNo(ItemsNo);
  if(HScrollBar)
    HScrollBar->SetTotalNo(ColsNo);
  if(GetState(SM_VISIBLE))
    Draw();
}

void XtList::SetValue(unsigned long __Item, long __Value)
{
  LItem *index = Items;
  unsigned long i;
  if(__Item > 0 && __Item <= ItemsNo)
    {
    for(i = 1; i < __Item; i++)
      index = index->Next;
    index->Value = __Value;
    }
}
