/*
  xtbutton.cc

  Copyright (c) 1996-99 Dragos Acostachioaie
  Copyright (c) 1996-97 Adrian Teohar

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

#include <string.h>

#include <xtbutton.h>

unsigned BD_PUSH_EFFECT_DELAY = 200;

XtButton::XtButton(XRect __r, char *__Text, int __State, int __Command) :
  XObject(__r)
{
  strcpy(ObjClass, "XtButton");
  strcpy(Text, __Text);
  State = __State;
  Command = __Command;
  Buffer = new chtype[(Size.x + 1) * (Size.y + 1) * 2];
  SetPalette(P_BUTTON, P_BUTTON_LEN);
  XObject::SetState(SM_ACTIVE, true);
}

XtButton::~XtButton()
{
  delete Buffer;
}

void XtButton::CalculateBounds()
{
  if(Server)
    Bounds.a = Server->GetBounds().a + Origin;
   else
    Bounds.a = Origin;
  Bounds.b.x = Bounds.a.x + Size.x;
  Bounds.b.y = Bounds.a.y + Size.y - 1;
}

void XtButton::Draw()
{
  int width = strlen(Text);
  char temp[80];
  if(!GetState(SM_VISIBLE))
    XGetWindow(XRect(Bounds.a.x, Bounds.a.y, Bounds.b.x + 1, Bounds.b.y + 1), Buffer);
  if(GetState(BF_PRESSED))
    {
    XColorWindow(XRect(Bounds.a.x + 1, Bounds.a.y, Bounds.b.x, Bounds.b.y), GetPalEntry(P_BUTTON_TEXT), GetPalEntry(P_BUTTON_TEXT + 1));
    strcpy(temp, " ");
    strcat(temp, Text);
    strcat(temp, " ");
    if(GetState(BF_ARROW) && GetState(SM_FOCUSED))
      {
      XSetColor(GetPalEntry(P_BUTTON_SELECTED), GetPalEntry(P_BUTTON_SELECTED + 1));
      if(XGetState(TM_IS_PC_CONSOLE))
        {
        XPrintChar(XPoint(Bounds.a.x + 1, Bounds.a.y + Size.y / 2), ACS_RARROW);
        XPrintChar(XPoint(Bounds.b.x, Bounds.a.y + Size.y / 2), ACS_LARROW);
        }
       else
        {
        XPrintChar(XPoint(Bounds.a.x + 1, Bounds.a.y + Size.y / 2), ACS_LARROW);
        XPrintChar(XPoint(Bounds.b.x, Bounds.a.y + Size.y / 2), ACS_RARROW);
        }
      }
    WriteString(Bounds.a.x + 3, Bounds.a.y + Size.y / 2, width + 2, temp);
    }
  else
    {
    XColorWindow(XRect(Bounds.a.x, Bounds.a.y, Bounds.b.x - 1, Bounds.b.y), GetPalEntry(P_BUTTON_TEXT), GetPalEntry(P_BUTTON_TEXT + 1));
    if(XGetState(TM_IS_PC_CONSOLE))
      {
      XSetColor(GetPalEntry(P_BUTTON_SHADOW), GetPalEntry(P_BUTTON_SHADOW + 1));
      XFillWindow(XRect(Bounds.a.x + 1, Bounds.b.y + 1, Bounds.b.x, Bounds.b.y + 1), 223 | A_ALTCHARSET);
      XFillWindow(XRect(Bounds.b.x, Bounds.a.y, Bounds.b.x, Bounds.b.y), 220 | A_ALTCHARSET);
      }
    strcpy(temp, " ");
    strcat(temp, Text);
    strcat(temp, " ");
    if(GetState(BF_ARROW) && GetState(SM_FOCUSED))
      {
      XSetColor(GetPalEntry(P_BUTTON_SELECTED), GetPalEntry(P_BUTTON_SELECTED + 1));
      if(XGetState(TM_IS_PC_CONSOLE))
        {
        XPrintChar(XPoint(Bounds.a.x, Bounds.a.y + Size.y / 2), ACS_RARROW);
        XPrintChar(XPoint(Bounds.b.x - 1, Bounds.a.y + Size.y / 2), ACS_LARROW);
        }
       else
        {
        XPrintChar(XPoint(Bounds.a.x, Bounds.a.y + Size.y / 2), ACS_LARROW);
        XPrintChar(XPoint(Bounds.b.x - 1, Bounds.a.y + Size.y / 2), ACS_RARROW);
        }
      }
    WriteString(Bounds.a.x + 2, Bounds.a.y + Size.y / 2, width + 2, temp);
    }
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE, true);
}

void XtButton::Press()
{
  XObject::SetState(BF_PRESSED, !GetState(BF_PRESSED));
  if(GetState(SM_VISIBLE))
    {
    XPutWindow(XRect(Bounds.a.x, Bounds.a.y, Bounds.b.x + 1, Bounds.b.y + 1), Buffer);
    Draw();
    XRefresh();
    }
}

bool XtButton::ProcessMessage(XMessage *__Event)
{
  XMessage *msg;
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
       msg->Message = Command;
       _PutEventInFront(msg);
       return true;
    }
  return false;
}

bool XtButton::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  XMessage *msg;
  if((__Event->KeyCode == KB_ESC && Command == CM_CANCEL) ||
   (__Event->KeyCode == KB_ENTER && GetState(SM_FOCUSED)))
    {
    XObject::SetState(SM_FOCUSED, true);
    Press();
    napms(BD_PUSH_EFFECT_DELAY);
    Press();
    msg = new XMessage;
    msg->Class = EV_COMMAND;
    msg->Message = Command;
    _PutEventInFront(msg);
    return true;
    }
  return false;
}

bool XtButton::ProcessMouseEvent(XMouseEvent *__Event)
{
  XMessage *msg;
  switch(__Event->Type)
    {
    case EV_MOUSEDOWN:
      if(Bounds.Contains(__Event->Position))
        {
	msg = new XMessage;
        msg->Message = MT_RELEASE_FOCUS;
        msg->Type = MC_CLIENT_TO_SERVER;
	if(Server)
	  Server->ProcessMessage(msg);
	delete msg;
        SetState(SM_FOCUSED, true);
        Press();
	return true;
        }
      return false;
    case EV_MOUSEUP:
      if(GetState(SM_FOCUSED))
	{
        Press();
	// if the mouse was released outside button's bounds, drop "press" state
	if(Bounds.Contains(__Event->Position))
	  {
  	  msg = new XMessage;
	  msg->Class = EV_COMMAND;
          msg->Message = Command;
	  _PutEventInFront(msg);
	  }
        return true;
	}
      return false;
    }
  return false;
}

void XtButton::SetState(unsigned long __Mask, bool __BoolValue)
{
  XObject::SetState(__Mask, __BoolValue);
  if(GetState(SM_VISIBLE))
    Draw();
}

void XtButton::WriteString(int __x, int __y, int __w, char *__astring)
{
  int maxindex, index = 1, i = 0;
  int highlight = 0;
  maxindex = strlen(__astring);
  while(index < maxindex && __w > 0)
    {
    XSetColor(GetPalEntry(P_BUTTON_TEXT), GetPalEntry(P_BUTTON_TEXT + 1));
    if(__astring[index] == SHORTCUT_LETTER)
      highlight = !highlight;
     else
      {
      if(GetState(SM_FOCUSED))
        if(highlight)
          XSetColor(GetPalEntry(P_BUTTON_SHORTCUT), GetPalEntry(P_BUTTON_SHORTCUT + 1));
        else
     	  XSetColor(GetPalEntry(P_BUTTON_TEXT), GetPalEntry(P_BUTTON_TEXT + 1));
      if(GetState(BF_DEFAULT))
        if(highlight)
          XSetColor(GetPalEntry(P_BUTTON_SHORTCUT_DEFAULT), GetPalEntry(P_BUTTON_SHORTCUT_DEFAULT + 1));
        else
          XSetColor(GetPalEntry(P_BUTTON_DEFAULT), GetPalEntry(P_BUTTON_DEFAULT + 1));
      if(GetState(SM_DISABLED))
        XSetColor(GetPalEntry(P_BUTTON_DISABLED), GetPalEntry(P_BUTTON_DISABLED + 1));
      XPrintChar(XPoint(__x + i, __y), __astring[index]);
      i++;
      __w--;
      }
      index++;
    }
}

CItem *NewItem(char *__Text, int __checked, CItem *__anextitem)
{
  CItem *temp = new CItem;
  strcpy(temp->Text, __Text);
  temp->Value = __checked;
  temp->Next = __anextitem;
  return temp;
}

char *CB_ICON = " [ ] ";
char CB_MARKER = 'X';

XtCheckButton::XtCheckButton(XRect __r, CItem *__Items) :
  XObject(__r)
{
  strcpy(ObjClass, "XtCheckButton");
  CItem *index = __Items;
  Items = __Items;
  SetPalette(P_CLUSTER, P_CLUSTER_LEN);
  CurrItem = 1;
  ItemsNo = 0;
  while(index)
    {
    ItemsNo++;
    index = index->Next;
    }
  CurrIndex = Items;
  XObject::SetState(SM_ACTIVE, true);
}

XtCheckButton::~XtCheckButton()
{
  CItem *index = Items;
  while(Items)
    {
    index = index->Next;
    delete Items;
    Items = index;
    }
}

void XtCheckButton::Draw()
{
  CItem *index = Items;
  char *temp = new char[80];
  unsigned long i = 0;
  XColorWindow(Bounds, GetPalEntry(P_CLUSTER_TEXT), GetPalEntry(P_CLUSTER_TEXT + 1));
  while(index)
    {
    strcpy(temp, CB_ICON);
    strcat(temp, index->Text);
    if(GetState(SM_FOCUSED) && (i == CurrItem - 1))
      XSetColor(GetPalEntry(P_CLUSTER_SELECTED), GetPalEntry(P_CLUSTER_SELECTED + 1));
     else
      XSetColor(GetPalEntry(P_CLUSTER_TEXT), GetPalEntry(P_CLUSTER_TEXT + 1));
    XPrint(XPoint(Bounds.a.x, Bounds.a.y + i), temp);
    if(index->Value)
      XPrintChar(XPoint(Bounds.a.x + 2, Bounds.a.y + i), CB_MARKER);
    index = index->Next;
    i++;
    }
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE, 1);
  delete temp;
}

int XtCheckButton::GetValue(unsigned __Item)
{
  unsigned i = 1;
  CItem *temp = Items;
  if(!__Item)
    return CurrItem;
  while(temp)
    {
    if(__Item == i++)
      return temp->Value;
    temp = temp->Next;
    }
  return -1;
}

bool XtCheckButton::ProcessMessage(XMessage *__Event)
{
  XMessage *msg;
  CItem *temp;
  unsigned long i, result = 0;
  switch(__Event->Message)
    {
    case MT_RELEASE_FOCUS:
      SetState(SM_FOCUSED, false);
      return true;
    case MT_RESUME_FOCUS:
      SetState(SM_FOCUSED, true);
      return true;
    case MT_ANSWER_REQUEST:
      temp = Items;
      i = 1;
      while(temp)
	{
	if(temp->Value)
	  result |= i;
	temp = temp->Next;
	i *= 2;
	}
      msg = new XMessage;
      msg->Message = result;
      _PutEventInFront(msg);
      return true;
    }
  return false;
}

bool XtCheckButton::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  unsigned long i;
  switch(__Event->KeyCode)
    {
    case KB_RIGHT:
    case KB_DOWN:
      if(CurrItem == ItemsNo)
        {
        CurrItem = 1;
        CurrIndex = Items;
        }
       else
        {
        CurrItem++;
        CurrIndex = CurrIndex->Next;
        }
      Draw();
      return true;
    case KB_LEFT:
    case KB_UP:
      if(CurrItem == 1)
        {
        CurrItem = ItemsNo;
        while(CurrIndex->Next)
          CurrIndex = CurrIndex->Next;
        }
       else
        {
        CurrItem--;
        CurrIndex = Items;
        for(i = 1; i < CurrItem; i++)
          CurrIndex = CurrIndex->Next;
        }
      Draw();
      return true;
    case KB_SPACE:
      CurrIndex->Value = !CurrIndex->Value;
      Draw();
      return true;
    case KB_HOME:
      if(CurrItem > 1)
        {
        CurrIndex = Items;
        CurrItem = 1;
        Draw();
	return 1;
        }
      return false;
    case KB_END:
      if(CurrItem < ItemsNo)
        {
        CurrItem = ItemsNo;
        CurrIndex = Items;
        while(CurrIndex->Next)
          CurrIndex = CurrIndex->Next;
        Draw();
	return true;
        }
      return false;
    }
  return false;
}

bool XtCheckButton::ProcessMouseEvent(XMouseEvent *__Event)
{
  XMessage *msg;
  CItem *temp;
  unsigned long i;
  if(__Event->Type == EV_MOUSEDOWN)
    if(Bounds.Contains(__Event->Position))
      {
      i = 0;
      temp = Items;
      while(temp)
	{
	if(Bounds.a.y + i == __Event->Position.y)
	  {
	  temp->Value = !temp->Value;
	  CurrItem = i + 1;
	  CurrIndex = temp;
	  msg = new XMessage;
	  msg->Message = MT_RELEASE_FOCUS;
	  msg->Type = MC_CLIENT_TO_SERVER;
	  if(Server)
	    Server->ProcessMessage(msg);
	  SetState(SM_FOCUSED, true);
	  return true;
	  }
	i++;
	temp = temp->Next;
	}
      }
  return false;
}

void XtCheckButton::SetState(unsigned long __Mask, bool __BoolValue)
{
  XObject::SetState(__Mask, __BoolValue);
  if(GetState(SM_VISIBLE))
    Draw();
}

char* RB_ICON = " ( ) ";
char RB_MARKER = '*';

XtRadioButton::XtRadioButton(XRect __r, CItem *__Items) :
  XObject(__r)
{
  strcpy(ObjClass, "XtRadioButton");
  CItem *index = __Items;
  Items = __Items;
  CurrIndex = Items;
  SetPalette(P_CLUSTER, P_CLUSTER_LEN);
  ItemsNo = 0;
  Value = 0;
  while(index)
    {
    ItemsNo++;
    if(index->Value)
      if(!Value)
        {
        Value = ItemsNo;
	CurrIndex = index;
	}
       else
        index->Value = 0;
    index = index->Next;
    }
  if(!Value)
    Value = 1;
  CurrItem = Value;
  XObject::SetState(SM_ACTIVE, true);
}

XtRadioButton::~XtRadioButton()
{
  CItem *index = Items;
  while(Items)
    {
    index = index->Next;
    delete Items;
    Items = index;
    }
}

void XtRadioButton::Draw()
{
  CItem *index = Items;
  char *temp = new char[80];
  unsigned long i = 0;
  XColorWindow(Bounds, GetPalEntry(P_CLUSTER_TEXT), GetPalEntry(P_CLUSTER_TEXT + 1));
  while(index)
    {
    strcpy(temp, RB_ICON);
    strcat(temp, index->Text);
    if(GetState(SM_FOCUSED) && (i == CurrItem - 1))
      XSetColor(GetPalEntry(P_CLUSTER_SELECTED), GetPalEntry(P_CLUSTER_SELECTED + 1));
     else
      XSetColor(GetPalEntry(P_CLUSTER_TEXT), GetPalEntry(P_CLUSTER_TEXT + 1));
    XPrint(XPoint(Bounds.a.x, Bounds.a.y + i), temp);
    if(index->Value)
      XPrintChar(XPoint(Bounds.a.x + 2, Bounds.a.y + i), RB_MARKER);
    index = index->Next;
    i++;
    }
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE, true);
  delete temp;
}

int XtRadioButton::GetValue(unsigned __Item)
{
  if(!__Item)
    return CurrItem;
  if(__Item == CurrItem)
    return 1;
  return 0;
}

bool XtRadioButton::ProcessMessage(XMessage *__Event)
{
  XMessage *msg;
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
      msg->Message = Value;
      _PutEventInFront(msg);
      return true;
    }
  return false;
}

bool XtRadioButton::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  unsigned long i;
  switch(__Event->KeyCode)
    {
    case KB_RIGHT:
    case KB_DOWN:
      CurrIndex->Value = 0;
      if(CurrItem == ItemsNo)
        {
        CurrItem = 1;
        CurrIndex = Items;
        }
       else
        {
        CurrItem++;
        CurrIndex = CurrIndex->Next;
        }
      CurrIndex->Value = 1;
      Draw();
      return true;
    case KB_LEFT:
    case KB_UP:
      CurrIndex->Value = 0;
      if(CurrItem == 1)
        {
        CurrItem = ItemsNo;
        while(CurrIndex->Next)
          CurrIndex = CurrIndex->Next;
        }
       else
        {
        CurrItem--;
        CurrIndex = Items;
        for(i = 1; i < CurrItem; i++)
          CurrIndex = CurrIndex->Next;
        }
      CurrIndex->Value = 1;
      Draw();
      return true;
    case KB_HOME:
      if(CurrItem > 1)
        {
        CurrIndex->Value = 0;
        CurrIndex = Items;
        CurrItem = 1;
        CurrIndex->Value = 1;
        Draw();
	return true;
        }
      return false;
    case KB_END:
      if(CurrItem < ItemsNo)
        {
        CurrIndex->Value = 0;
        CurrItem = ItemsNo;
        CurrIndex = Items;
        while(CurrIndex->Next)
          CurrIndex = CurrIndex->Next;
        CurrIndex->Value = 1;
        Draw();
	return true;
        }
      return false;
    }
  return false;
}

bool XtRadioButton::ProcessMouseEvent(XMouseEvent *__Event)
{
  XMessage *msg;
  CItem *temp;
  unsigned long i = 0;
  if(__Event->Type == EV_MOUSEDOWN)
    if(Bounds.Contains(__Event->Position))
      {
      temp = Items;
      while(temp)
        {
        if(Bounds.a.y + i == __Event->Position.y)
          {
          CurrIndex->Value = 0;
	  temp->Value = 1;
          CurrItem = i + 1;
          CurrIndex = temp;
	  msg = new XMessage;
          msg->Message = MT_RELEASE_FOCUS;
	  msg->Type = MC_CLIENT_TO_SERVER;
	  if(Server)
            Server->ProcessMessage(msg);
          SetState(SM_FOCUSED, true);
          return true;
          }
        i++;
        temp = temp->Next;
        }
      }
  return false;
}

void XtRadioButton::SetState(unsigned long __Mask, bool __BoolValue)
{
  XObject::SetState(__Mask, __BoolValue);
  if(GetState(SM_VISIBLE))
    Draw();
}
