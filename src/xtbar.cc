/*
  xtbar.cc

  Copyright (c) 1996-97 Adrian Teohar
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

#include <xtscreen.h>
#include <xtbar.h>

XtScrollBar::XtScrollBar(XRect __r, unsigned long __TotalNo, unsigned long __Flags) :
  XObject(__r)
{
  strcpy(ObjClass, "XtScrollBar");
  CurrentNo = 1;
  if(__Flags & SB_INDICATE_PERCENT)
    if(__Flags & SB_HORIZONTAL)
      MaxNo = (__TotalNo < Size.x) ? Size.x : __TotalNo;
     else
      MaxNo = (__TotalNo < Size.y) ? Size.y : __TotalNo;
    else
     MaxNo = __TotalNo;
  Flags = __Flags;
  SetPalette(P_BAR, P_BAR_LEN);
  XObject::SetState(SM_ACTIVE, true);
}

void XtScrollBar::Draw()
{
  unsigned i, pos, len;
  XSetColor(GetPalEntry(P_BAR_MARKERS), GetPalEntry(P_BAR_MARKERS + 1));
  if(Flags & SB_HORIZONTAL)
    {
    XPrintChar(Bounds.a, ACS_LARROW);
    XPrintChar(XPoint(Bounds.b.x, Bounds.a.y), ACS_RARROW);
    XSetColor(GetPalEntry(P_BAR_BAR), GetPalEntry(P_BAR_BAR + 1));
    for(i = 1; i < Size.x - 1; i++)
      XPrintChar(XPoint(Bounds.a.x + i, Bounds.a.y), ACS_BOARD);
    XSetColor(GetPalEntry(P_BAR_MARKERS), GetPalEntry(P_BAR_MARKERS + 1));
    if(MaxNo > 0)
      if(Flags & SB_INDICATE_PERCENT)
        {
        len = Size.x * (Size.x - 2) / MaxNo;
        pos = (Size.x - 2) * (CurrentNo - 1) / MaxNo + 1;
        for(i = 0; i < len; i++)
          XPrintChar(XPoint(Bounds.a.x + pos + i, Bounds.a.y), ACS_BLOCK);
	}
       else
        {
        pos = ((Size.x - 2) * CurrentNo) / MaxNo + 1;
        XPrintChar(XPoint(Bounds.a.x + pos, Bounds.a.y), ACS_BLOCK);
        }
    }
   else
    {
    XPrintChar(Bounds.a, ACS_UARROW);
    XPrintChar(XPoint(Bounds.a.x, Bounds.b.y), ACS_DARROW);
    XSetColor(GetPalEntry(P_BAR_BAR), GetPalEntry(P_BAR_BAR + 1));
    for(i = 1; i < Size.y - 1; i++)
      XPrintChar(XPoint(Bounds.a.x, Bounds.a.y + i), ACS_BOARD);
    XSetColor(GetPalEntry(P_BAR_MARKERS), GetPalEntry(P_BAR_MARKERS + 1));
    if(MaxNo > 0)
      if(Flags & SB_INDICATE_PERCENT)
        {
        len = Size.y * (Size.y - 2) / MaxNo;
        pos = (Size.y - 2) * (CurrentNo - 1) / MaxNo + 1;
        for(i = 0; i < len; i++)
          XPrintChar(XPoint(Bounds.a.x, Bounds.a.y + pos + i), ACS_BLOCK);
	}
       else
        {
        pos = ((Size.y - 2) * CurrentNo) / MaxNo + 1;
        XPrintChar(XPoint(Bounds.a.x, Bounds.a.y + pos), ACS_BLOCK);
	}
    }
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE, true);
}

unsigned long XtScrollBar::GetTotalNo()
{
  return MaxNo;
}

bool XtScrollBar::ProcessMouseEvent(XMouseEvent *__Event)
{
  XMessage *msg;
  switch(__Event->Type)
    {
    case EV_MOUSEDOWN:
      if(Bounds.Contains(__Event->Position))
	{
	if(__Event->Position.y == Bounds.a.y)
	  {
	  // `Up' button
	  msg = new XMessage;
	  msg->Type = MC_CLIENT_TO_SERVER;
	  msg->Message = SCROLLBAR_ITEMUP;
	  _PutEventInFront(msg);
	  return true;
	  }
	if(__Event->Position.y == Bounds.b.y)
	  {
	  // `Down' button
	  msg = new XMessage;
	  msg->Type = MC_CLIENT_TO_SERVER;
	  msg->Message = SCROLLBAR_ITEMDOWN;
	  _PutEventInFront(msg);
	  return true;
	  }
	}
      break;
    }
  return false;
}

void XtScrollBar::SetValue(unsigned long __ItemNo)
{
  CurrentNo = __ItemNo;
  if(GetState(SM_VISIBLE))
    Draw();
}

void XtScrollBar::SetTotalNo(unsigned long __ItemNo)
{
  if(Flags & SB_INDICATE_PERCENT)
    if(Flags & SB_HORIZONTAL)
      MaxNo = (__ItemNo < Size.x) ? Size.x + 1 : __ItemNo;
     else
      MaxNo = (__ItemNo < Size.y) ? Size.y + 1 : __ItemNo;
    else
     MaxNo = __ItemNo;
  if(GetState(SM_VISIBLE))
    Draw();
}

XtProgressBar::XtProgressBar(XRect __r, unsigned long __TotalNo, unsigned __Direction) :
  XObject(__r)
{
  strcpy(ObjClass, "XtProgressBar");
  MaxNo = __TotalNo;
  CurrentNo = 0;
  Percent = 0;
  if(__Direction <= PB_RIGHT_TO_LEFT)
    Len = Size.x;
   else
    Len = Size.y;
  Direction = __Direction;
  SetPalette(P_BAR, P_BAR_LEN);
}

void XtProgressBar::Draw()
{
  unsigned i, pos;
  if(CurrentNo >= 0 && CurrentNo <= MaxNo)
    {
    Percent = (CurrentNo * 100) / MaxNo;
    XSetColor(GetPalEntry(P_BAR_MARKERS), GetPalEntry(P_BAR_MARKERS + 1));
    pos = (Len * CurrentNo) / MaxNo;
      for(i = 0; i < pos; i++)
        switch(Direction) 
          {
          case PB_LEFT_TO_RIGHT:
            XPrintChar(XPoint(Bounds.a.x + i, Bounds.a.y), ACS_BLOCK);
            break;
          case PB_RIGHT_TO_LEFT:
            XPrintChar(XPoint(Bounds.b.x - i, Bounds.a.y), ACS_BLOCK);
            break;
          case PB_BOTTOM_TO_TOP:
            XPrintChar(XPoint(Bounds.a.x, Bounds.b.y - i), ACS_BLOCK);
            break;
          case PB_TOP_TO_BOTTOM:
            XPrintChar(XPoint(Bounds.a.x, Bounds.a.y + i), ACS_BLOCK);
            break;
          }
      XSetColor(GetPalEntry(P_BAR_BAR), GetPalEntry(P_BAR_BAR + 1));
      for(i = pos; i < Len - 1; i++)
        switch(Direction) 
          {
          case PB_LEFT_TO_RIGHT:
            XPrintChar(XPoint(Bounds.a.x + i, Bounds.a.y), ACS_BOARD);
            break;
          case PB_RIGHT_TO_LEFT:
            XPrintChar(XPoint(Bounds.b.x - i, Bounds.a.y), ACS_BOARD);
            break;
          case PB_BOTTOM_TO_TOP:
            XPrintChar(XPoint(Bounds.a.x, Bounds.b.y - i), ACS_BOARD);
            break;
          case PB_TOP_TO_BOTTOM:
            XPrintChar(XPoint(Bounds.a.x, Bounds.a.y + i), ACS_BOARD);
            break;
          }
    if(!GetState(SM_NO_REFRESH))
      XRefresh();
    }
  XObject::SetState(SM_VISIBLE, true);
}

float XtProgressBar::GetPercent()
{
  return Percent;
}

void XtProgressBar::SetValue(unsigned long __ItemNo)
{
  CurrentNo = __ItemNo;
  if(GetState(SM_VISIBLE))
    Draw();
}

unsigned long XtProgressBar::GetValue()
{
  return CurrentNo;
}

void XtProgressBar::SetTotalNo(unsigned long __ItemNo)
{
  MaxNo = __ItemNo;
  if(GetState(SM_VISIBLE))
    Draw();
}

unsigned long XtProgressBar::GetTotalNo()
{
  return MaxNo;
}
