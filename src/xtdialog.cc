/*
  xtdialog.cc

  Copyright (c) 1996-2001 Dragos Acostachioaie
  Copyright (c) 2000 Udo Kreckel

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
#include <xtdialog.h>
#include <xtlib.h>

XtDialog::XtDialog(XRect __r) :
  XObject(__r)
{
  strcpy(ObjClass, "XtDialog");
  CurrentFocusedClient = 0;
}

void XtDialog::HandleEvent(XEvent *__Event)
{
  ClientList::iterator index = Clients.begin(), end = Clients.end();
  if(!__Event || __Event->Class == EV_NOTHING)
    return;
  if(ProcessEvent(__Event))
    return;
  for(; index != end; index++)
    if((*index)->GetState(SM_FOCUSED) || __Event->Class == EV_MOUSE)
      (*index)->HandleEvent(__Event);
  for(index = Clients.begin(); index != end; index++)
    if((*index)->GetState(SM_FOCUSED))
      {
      CurrentFocusedClient = *index;
      break;
      }
  // allow client's Draw() to have effect
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
}

bool XtDialog::ProcessMessage(XMessage *__Event)
{
  XMessage *msg;
  ClientList::iterator index = Clients.begin(), end = Clients.end();
  switch(__Event->Message)
    {
    case MT_RELEASE_FOCUS:
      if(CurrentFocusedClient)
        {
        msg = new XMessage;
        msg->Message = MT_RELEASE_FOCUS;
        CurrentFocusedClient->ProcessMessage(msg);
        delete msg;
	}
      if(__Event->Type != MC_CLIENT_TO_SERVER)
        SetState(SM_FOCUSED, false);
      if(!GetState(SM_NO_REFRESH))
        XRefresh();
      return true;
    case MT_RESUME_FOCUS:
      SetState(SM_FOCUSED, true);
      if(CurrentFocusedClient)
        {
        msg = new XMessage;
        msg->Message = MT_RESUME_FOCUS;
        CurrentFocusedClient->ProcessMessage(msg);
        delete msg;
	}
       else
        // find the first active object and give him the focus
        for(; index != end; index++)
          if((*index)->GetState(SM_ACTIVE))
            {
            CurrentFocusedClient = *index;
            msg = new XMessage;
            msg->Message = MT_RESUME_FOCUS;
            (*index)->ProcessMessage(msg);
            delete msg;
            break;
            }
      if(!GetState(SM_NO_REFRESH))
        XRefresh();
      return true;
    case MT_FORWARD_FOCUS:
        FocusNextClient();
        return true;
    case MT_BACKWARD_FOCUS:
        FocusPrevClient();
	return true;
    }
  return false;
}

XtDialogBox::XtDialogBox(XRect __r, const char *__Title) :
  XtDialog(__r)
{
  strcpy(ObjClass, "XtDialogBox");
  Wnd = new XtWindow(__r, __Title);
  Wnd->SetState(SM_NO_REFRESH | WM_HAVE_SHADOW | WM_CAN_CLOSE | WM_CAN_MOVE, true);
  XObject::SetState(SM_ACTIVE, true);
}

XtDialogBox::~XtDialogBox()
{
  delete Wnd;
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
}

void XtDialogBox::CalculateBounds()
{
  XObject::CalculateBounds();
  Wnd->MoveTo(Bounds.a);
  Wnd->CalculateBounds();
}

void XtDialogBox::Draw()
{
  Wnd->Draw();
  XObject::Draw();
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE, true);
}

void XtDialog::FocusNextClient()
{
    XMessage *msg;
    ClientList::iterator index = Clients.begin(), end = Clients.end();
    bool found = false;
    
    if(CurrentFocusedClient)
    {
        msg = new XMessage;
        msg->Message = MT_RELEASE_FOCUS;
        CurrentFocusedClient->ProcessMessage(msg);
        delete msg;
        found = false;
        for(; *index != CurrentFocusedClient; index++);
        for(++index; index != end; index++)
            if((*index)->GetState(SM_ACTIVE))
            {
                found = true;
                CurrentFocusedClient = *index;
                msg = new XMessage;
                msg->Message = MT_RESUME_FOCUS;
                (*index)->ProcessMessage(msg);
                delete msg;
                break;
            }
        if(!found)
            for(index = Clients.begin(); index != end; index++)
                if((*index)->GetState(SM_ACTIVE))
                {
                    CurrentFocusedClient = *index;
                    msg = new XMessage;
                    msg->Message = MT_RESUME_FOCUS;
                    (*index)->ProcessMessage(msg);
                    delete msg;
                    break;
                }
          if(!GetState(SM_NO_REFRESH))
              XRefresh();
    }
}

void XtDialog::FocusPrevClient()
{
    XMessage *msg;
    XObject *obj;
    ClientList::iterator index = Clients.begin(), begin = Clients.begin(),
	end = Clients.end();
    bool found = false;

    if(CurrentFocusedClient)
	{
        msg = new XMessage;
        msg->Message = MT_RELEASE_FOCUS;
        CurrentFocusedClient->ProcessMessage(msg);
        delete msg;
        found = false;
	for(; *index != CurrentFocusedClient; index++)
	    if((*index)->GetState(SM_ACTIVE) && *index != CurrentFocusedClient)
		{
		found = true;
		obj = *index;
		}
	if(!found)
	    for(++index; index != end; index++)
		if((*index)->GetState(SM_ACTIVE))
		    {
		    found = true;
		    obj = *index;
		    }

	if(found)
	    {
	    CurrentFocusedClient = obj;
            msg = new XMessage;
            msg->Message = MT_RESUME_FOCUS;
            CurrentFocusedClient->ProcessMessage(msg);
            delete msg;
	    }
	if(!GetState(SM_NO_REFRESH))
	    XRefresh();
    }
}

void XtDialogBox::MoveTo(XPoint __p)
{
  unsigned long oldstate = Wnd->GetState();
  XObject::MoveTo(__p);
  Wnd->SetState(SM_VISIBLE, false);
  Wnd->MoveTo(Bounds.a);
  Wnd->State = oldstate;
  Draw();
}

bool XtDialogBox::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  XMessage *msg;
  XEvent *ev;
  XObject *obj;
  ClientList::iterator index = Clients.begin(), end = Clients.end();
  bool found = false;

  if(Wnd->GetState(SM_DRAGGING) && Wnd->Adjust.y == 0)
    return false;
  switch(__Event->KeyCode)
    {
    case WINDOW_CLOSE_KEY:
      if(Wnd->GetState(WM_CAN_CLOSE))
	{
	if(GetState(SM_DRAGGING))
	  {
	  XKeyboardEvent *ev = new XKeyboardEvent;
	  ev->KeyCode = KB_ENTER;
	  ProcessKeyboardEvent(ev);
	  delete ev;
	  }
        ev = new XMessage;
        ((XMessage *)ev)->Message = MT_RELEASE_FOCUS;
        HandleEvent(ev);
        delete ev;
        if(!GetState(SM_NO_REFRESH))
          XRefresh();
	msg = new XMessage;
	msg->Class = EV_COMMAND;
        msg->Message = CM_CLOSE;
	_PutEventInFront(msg);
	return true;
	}
      return false;
    case WINDOW_MOVE_KEY:
      if(Wnd->GetState(WM_CAN_MOVE) && !GetState(SM_DRAGGING))
	{
	Wnd->SetState(SM_DRAGGING, true);
	Wnd->Adjust.Assign(0, 1);
	SetState(SM_DRAGGING, true);
	if(CurrentFocusedClient)
	  {
	  msg = new XMessage;
          msg->Message = MT_RELEASE_FOCUS;
          CurrentFocusedClient->ProcessMessage(msg);
	  delete msg;
	  }
        if(!GetState(SM_NO_REFRESH))
          XRefresh();
	return true;
	}
      return false;
    case KB_TAB:
      if(GetState(SM_DRAGGING))
          return false;
      FocusNextClient();
      return true;
    case KB_META('p'):
      if(GetState(SM_DRAGGING))
	return false;
      FocusPrevClient();
      return true;
    case KB_ENTER:
      if(GetState(SM_DRAGGING))
	{
	Wnd->SetState(SM_DRAGGING, false);
	SetState(SM_DRAGGING, false);
	if(CurrentFocusedClient)
	  {
          msg = new XMessage;
          msg->Message = MT_RESUME_FOCUS;
          CurrentFocusedClient->ProcessMessage(msg);
	  delete msg;
          if(!GetState(SM_NO_REFRESH))
            XRefresh();
	  }
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
	if(Bounds.b.x + (Wnd->GetState(WM_HAVE_SHADOW) ? 2 : 0) < XGetSize().x)
	  MoveTo(Origin + XPoint(1, 0));
        return true;
	}
      return false;
    case KB_DOWN:
      if(GetState(SM_DRAGGING))
        {
	if(Bounds.b.y + (Wnd->GetState(WM_HAVE_SHADOW) ? 1 : 0) < XGetSize().y)
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
  return Wnd->ProcessKeyboardEvent(__Event);
}

bool XtDialogBox::ProcessMouseEvent(XMouseEvent *__Event)
{
  XMessage *msg;
  XEvent *ev;

  if(GetState(SM_DRAGGING) && Wnd->Adjust.y == 1)
    return false;
  switch(__Event->Type)
    {
    case EV_MOUSEDOWN:
      if(__Event->Position.y == Bounds.a.y)
        {
        if(Wnd->GetState(WM_CAN_CLOSE) && __Event->Position.x >= Bounds.a.x + 2
	  && __Event->Position.x <= Bounds.a.x + 4)
          // "Close"
          {
          ev = new XMessage;
          ((XMessage *)ev)->Message = MT_RELEASE_FOCUS;
          HandleEvent(ev);
          delete ev;
          if(!GetState(SM_NO_REFRESH))
            XRefresh();
	  msg = new XMessage;
	  msg->Class = EV_COMMAND;
          msg->Message = CM_CLOSE;
	  _PutEventInFront(msg);
	  return true;
          }
        if(Wnd->GetState(WM_CAN_MOVE) && __Event->Position.x >= Bounds.a.x
	  && __Event->Position.x <= Bounds.b.x)
	  // "Move"
          {
	  Wnd->SetState(SM_DRAGGING, true);
          Wnd->Adjust.Assign(__Event->Position.x - Bounds.a.x, 0);
	  SetState(SM_DRAGGING, true);
	  if(CurrentFocusedClient)
	    {
	    msg = new XMessage;
            msg->Message = MT_RELEASE_FOCUS;
	    CurrentFocusedClient->ProcessMessage(msg);
	    delete msg;
	    }
          if(!GetState(SM_NO_REFRESH))
            XRefresh();
	  return true;
          }          
        }
      return false;
    case EV_MOUSEUP:
      if(GetState(SM_DRAGGING))
        {
	Wnd->SetState(SM_DRAGGING, false);
        SetState(SM_DRAGGING, false);
	if(CurrentFocusedClient)
	  {
	  msg = new XMessage;
          msg->Message = MT_RESUME_FOCUS;
          CurrentFocusedClient->ProcessMessage(msg);
	  delete msg;
          if(!GetState(SM_NO_REFRESH))
            XRefresh();
	  }
	return true;
        }
      return false;
    case EV_MOUSEMOVE:
      if(GetState(SM_DRAGGING))
	{
        if(__Event->Position.x >= Wnd->Adjust.x + 1
	  && __Event->Position.x + Size.x + (Wnd->GetState(WM_HAVE_SHADOW) ? 2 : 0) - Wnd->Adjust.x - 1 <= XGetSize().x
	  && __Event->Position.y >= 1
	  && __Event->Position.y + Size.y + (Wnd->GetState(WM_HAVE_SHADOW) ? 1 : 0) - 1 <= XGetSize().y)
	  if(Server)
            MoveTo(XPoint(__Event->Position.x - Wnd->Adjust.x - Server->GetBounds().a.x, __Event->Position.y - Server->GetBounds().a.y));
	   else
            MoveTo(XPoint(__Event->Position.x - Wnd->Adjust.x, __Event->Position.y));
        return true;
	}
      return false;
    }
  return Wnd->ProcessMouseEvent(__Event);
}

int XtDialogBox::Run()
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
    if(ev && ev->Class == EV_COMMAND
      && (((XMessage *)ev)->Message == CM_CLOSE
      || ((XMessage *)ev)->Message == CM_OK))
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

void XtDialogBox::SetTitle(const char *__Title)
{
    Wnd->SetTitle(__Title);
}

void XtDialogBox::SetState(unsigned long __Mask, bool __BoolValue)
{
  XObject::SetState(__Mask, __BoolValue);
  if(GetState(SM_VISIBLE))
    Draw();
}

bool XtDialogBox::SetFocus(XObject *__Object)
{
    XMessage *msg;
    ClientList::iterator index = Clients.begin(), end = Clients.end();
    bool found = false;

    if(CurrentFocusedClient)
    {
        msg = new XMessage;
        msg->Message = MT_RELEASE_FOCUS;
        CurrentFocusedClient->ProcessMessage(msg);
        delete msg;
    }
    found = false;
    
    for(index = Clients.begin(); index != end; index++)
    {
        if((*index)->GetState(SM_ACTIVE))
        {
            if (*index == __Object)
                {
                    CurrentFocusedClient = *index;
                    found = true;
                    msg = new XMessage;
                    msg->Message = MT_RESUME_FOCUS;
                    (*index)->ProcessMessage(msg);
                    delete msg;
                    break;
                }
        }
    }

    if(!GetState(SM_NO_REFRESH))
        XRefresh();

    return found;
}
