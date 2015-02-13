/*
  xtwindow.cc

  Copyright (c) 1996-2000 Dragos Acostachioaie

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

#include <xtwindow.h>

XtWindow::XtWindow(XRect __r, const char *__Title) :
  XObject(__r)
{
  strcpy(ObjClass, "XtWindow");
  Title = new char[strlen(__Title) + 1];
  strcpy(Title, __Title);
  SetPalette(P_WINDOW, P_WINDOW_LEN);
  if(XGetState(TM_IS_PC_CONSOLE))
    strcpy(Frame, FS_DOUBLEFRAME);
// by default, Xterminal will choose the best chars for the current terminal
   else
    strcpy(Frame, FS_DEFAULT);
  Buffer = 0;
  XObject::SetState(SM_ACTIVE, true);
}

XtWindow::~XtWindow()
{
  Close();
  delete Title;
}

void XtWindow::Close()
{
  if(GetState(SM_VISIBLE))
    {
    if(GetState(WM_HAVE_SHADOW))
      XPutWindow(XRect(Bounds.a, Bounds.b + XPoint(2, 1)), Buffer);
     else
      XPutWindow(Bounds, Buffer);
    delete Buffer;
    if(!GetState(SM_NO_REFRESH))
      XRefresh();
    XObject::SetState(SM_VISIBLE, false);
    }
}

void XtWindow::Draw()
{
  if(!GetState(SM_VISIBLE))
    {
    XObject::SetState(SM_FOCUSED, true);
    if(GetState(WM_HAVE_SHADOW))
      {
      Buffer = new chtype[(Size.y + 1) * (Size.x + 2)];
      XGetWindow(XRect(Bounds.a, Bounds.b + XPoint(2, 1)), Buffer);
      }
     else
      {
      Buffer = new chtype[Size.y * Size.x];
      XGetWindow(Bounds, Buffer);
      }
    }
  FrameUpdate();
  XColorWindow(XRect(Bounds.a.x + 1, Bounds.a.y + 1, Bounds.b.x - 1, Bounds.b.y - 1), GetPalEntry(P_WINDOW_SURFACE), GetPalEntry(P_WINDOW_SURFACE + 1));
  if(GetState(WM_HAVE_SHADOW))
    {
    XColorWindowContent(XRect(Bounds.a.x + 2, Bounds.b.y + 1, Bounds.b.x + 1, Bounds.b.y + 1), GetPalEntry(P_WINDOW_SHADOW), GetPalEntry(P_WINDOW_SHADOW + 1));
    XColorWindowContent(XRect(Bounds.b.x + 1, Bounds.a.y + 1, Bounds.b.x + 2, Bounds.b.y + 1), GetPalEntry(P_WINDOW_SHADOW), GetPalEntry(P_WINDOW_SHADOW + 1));
    }
  XObject::Draw();
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE, true);
}

void XtWindow::FrameUpdate()
{
  div_t x;
  unsigned xi;
  if(GetState(SM_FOCUSED))
    if(GetState(SM_DRAGGING))
      XSetColor(GetPalEntry(P_WINDOW_MARKERS), GetPalEntry(P_WINDOW_MARKERS + 1));
     else
      XSetColor(GetPalEntry(P_WINDOW_FRAME), GetPalEntry(P_WINDOW_FRAME + 1));
   else
    XSetColor(GetPalEntry(P_WINDOW_UNFOCUSED_FRAME), GetPalEntry(P_WINDOW_UNFOCUSED_FRAME + 1));
  if(GetState(SM_DRAGGING))
    XDrawBorder(Bounds, FS_DEFAULT);
   else
    XDrawBorder(Bounds, Frame);
  if(GetState(WM_CAN_CLOSE))
    {
    XPrintChar(XPoint(Bounds.a.x + 2, Bounds.a.y), '[');
    XPrintChar(XPoint(Bounds.a.x + 4, Bounds.a.y), ']');
    XSetColor(GetPalEntry(P_WINDOW_MARKERS), GetPalEntry(P_WINDOW_MARKERS + 1));
    XPrintChar(XPoint(Bounds.a.x + 3, Bounds.a.y), WINDOW_CLOSE_MARKER);
    }
  if(GetState(SM_FOCUSED))
    XSetColor(GetPalEntry(P_WINDOW_FRAME), GetPalEntry(P_WINDOW_FRAME));
   else
    XSetColor(GetPalEntry(P_WINDOW_UNFOCUSED_FRAME), GetPalEntry(P_WINDOW_UNFOCUSED_FRAME + 1));
  if(GetState(WM_CAN_ZOOM) && !GetState(SM_DRAGGING))
    {
    XPrintChar(XPoint(Bounds.b.x - 2, Bounds.a.y), ']');
    XPrintChar(XPoint(Bounds.b.x - 4, Bounds.a.y), '[');
    XSetColor(GetPalEntry(P_WINDOW_MARKERS), GetPalEntry(P_WINDOW_MARKERS + 1));
    XPrintChar(XPoint(Bounds.b.x - 3, Bounds.a.y), WINDOW_ZOOM_MARKER);
    }
  if(GetState(WM_CAN_RESIZE) && !GetState(SM_DRAGGING))
    {
    XSetColor(GetPalEntry(P_WINDOW_MARKERS), GetPalEntry(P_WINDOW_MARKERS + 1));
    XPrintChar(Bounds.b, ACS_LRCORNER);
    }
  if(Title[0])
    {
    if(GetState(SM_FOCUSED))
      if(GetState(SM_DRAGGING))
        XSetColor(GetPalEntry(P_WINDOW_MARKERS), GetPalEntry(P_WINDOW_MARKERS + 1));
       else
        XSetColor(GetPalEntry(P_WINDOW_TITLE), GetPalEntry(P_WINDOW_TITLE + 1));
     else
      XSetColor(GetPalEntry(P_WINDOW_UNFOCUSED_TITLE), GetPalEntry(P_WINDOW_UNFOCUSED_TITLE + 1));
    x = div(Size.x - strlen(Title), 2);
    xi = x.quot + Bounds.a.x;
    XPrintChar(XPoint(xi - 1, Bounds.a.y), ' ');
    XPrint(XPoint(xi, Bounds.a.y), Title);
    XPrintChar(XPoint(xi + strlen(Title), Bounds.a.y), ' ');
    }
}

void XtWindow::MoveTo(XPoint __p)
{
  unsigned long oldstate = State;
  if(GetState(SM_VISIBLE))
    {
    XObject::SetState(SM_NO_REFRESH, true);
    Close();
    }
  XObject::MoveTo(__p);
  XObject::SetState(SM_NO_REFRESH, oldstate & SM_NO_REFRESH);
  if(oldstate & SM_VISIBLE)
    Draw();
}

bool XtWindow::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  XMessage *msg;
  if(GetState(SM_DRAGGING) && Adjust.y == 0)
    return false;
  switch(__Event->KeyCode)
    {
    case WINDOW_CLOSE_KEY:
      if(GetState(WM_CAN_CLOSE))
	{
	if(GetState(SM_DRAGGING))
	  {
	  XKeyboardEvent *ev = new XKeyboardEvent;
	  ev->KeyCode = KB_ENTER;
	  ProcessKeyboardEvent(ev);
	  delete ev;
	  }
	msg = new XMessage;
	msg->Class = EV_COMMAND;
        msg->Message = CM_CLOSE;
	_PutEventInFront(msg);
	Close();
	return true;
	}
      return false;
    case WINDOW_MOVE_KEY:
      if(GetState(WM_CAN_MOVE) && !GetState(SM_DRAGGING))
	{
        SetState(SM_DRAGGING, true);
	Adjust.Assign(0, 1);
	return true;
	}
      return false;
    case KB_ENTER:
      if(GetState(SM_DRAGGING))
	{
        SetState(SM_DRAGGING, false);
	return true;
	}
      return false;
    case KB_LEFT:
      if(GetState(SM_DRAGGING))
        {
	if(Bounds.a.x > 1)
	  MoveTo(Origin - XPoint(1, 0));
        return true;
	}
      return false;
    case KB_RIGHT:
      if(GetState(SM_DRAGGING))
        {
	if(Bounds.b.x + (GetState(WM_HAVE_SHADOW) ? 2 : 0) < XGetSize().x)
	  MoveTo(Origin + XPoint(1, 0));
        return true;
	}
      return false;
    case KB_DOWN:
      if(GetState(SM_DRAGGING))
        {
	if(Bounds.b.y + (GetState(WM_HAVE_SHADOW) ? 1 : 0) < XGetSize().y)
	  MoveTo(Origin + XPoint(0, 1));
        return true;
	}
      return false;
    case KB_UP:
      if(GetState(SM_DRAGGING))
        {
	if(Bounds.a.y > 1)
	  MoveTo(Origin - XPoint(0, 1));
        return true;
	}
      return false;
    }
  return false;
}

bool XtWindow::ProcessMessage(XMessage *__Event)
{
  switch(__Event->Message)
    {
    case MT_RELEASE_FOCUS:
      SetState(SM_FOCUSED, false);
      return true;
    case MT_RESUME_FOCUS:
      SetState(SM_FOCUSED, true);
      return true;
    case WM_CLOSE:
      Close();
      return true;
    case WM_OPEN:
      Draw();
      return true;
    }
  return false;
}

bool XtWindow::ProcessMouseEvent(XMouseEvent *__Event)
{
  XMessage *msg;
  if(GetState(SM_DRAGGING) && Adjust.y == 1)
    return false;
  switch(__Event->Type)
    {
    case EV_MOUSEDOWN:
      if(__Event->Position.y == Bounds.a.y)
        {
        if(GetState(WM_CAN_CLOSE) && __Event->Position.x >= Bounds.a.x + 2
	  && __Event->Position.x <= Bounds.a.x + 4)
          // "Close"
          {
	  msg = new XMessage;
	  msg->Class = EV_COMMAND;
          msg->Message = CM_CLOSE;
	  _PutEventInFront(msg);
	  Close();
	  return true;
          }
        if(GetState(WM_CAN_MOVE) && __Event->Position.x >= Bounds.a.x
	  && __Event->Position.x <= Bounds.b.x)
	  // "Move"
          {
          SetState(SM_DRAGGING, true);
          Adjust.Assign(__Event->Position.x - Bounds.a.x, 0);
	  return true;
          }
        }
      return false;
    case EV_MOUSEUP:
      if(GetState(SM_DRAGGING))
        {
        SetState(SM_DRAGGING, false);
	return true;
        }
      return false;
    case EV_MOUSEMOVE:
      if(GetState(SM_DRAGGING))
	{
        if(__Event->Position.x >= Adjust.x + 1
	  && __Event->Position.x + Size.x + (GetState(WM_HAVE_SHADOW) ? 2 : 0) - Adjust.x - 1 <= XGetSize().x
	  && __Event->Position.y >= 1
	  && __Event->Position.y + Size.y + (GetState(WM_HAVE_SHADOW) ? 1 : 0) - 1 <= XGetSize().y)
	  if(Server)
            MoveTo(XPoint(__Event->Position.x - Adjust.x - Server->GetBounds().a.x, __Event->Position.y - Server->GetBounds().a.y));
	   else
            MoveTo(XPoint(__Event->Position.x - Adjust.x, __Event->Position.y));
        return true;
	}
      return false;
    }
  return false;
}

void XtWindow::Resize(XPoint __delta)
{
  unsigned long oldstate = State;
  if(GetState(SM_VISIBLE))
    {
    XObject::SetState(SM_NO_REFRESH, true);
    Close();
    }
  XObject::Resize(__delta);
  if(oldstate & SM_VISIBLE)
    Draw();
  XObject::SetState(SM_NO_REFRESH, oldstate & SM_NO_REFRESH);
}

void XtWindow::SetFrame(char *__Frame)
{
  strcpy(Frame, __Frame);
  if(GetState(SM_VISIBLE))
    {
    FrameUpdate();
    if(!GetState(SM_NO_REFRESH))
      XRefresh();
    }
}

void XtWindow::SetTitle(const char *__Title)
{
  delete Title;
  Title = new char[strlen(__Title) + 1];
  strcpy(Title, __Title);
}

void XtWindow::SetState(unsigned long __Mask, bool __BoolValue)
{
  XObject::SetState(__Mask, __BoolValue);
  if(GetState(SM_VISIBLE))
    Draw();
}
