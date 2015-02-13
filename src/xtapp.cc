/*
  xtapp.cc

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

#include <xtmenu.h>
#include <xtapp.h>
#include <string.h>

XtApplication::XtApplication() :
  XtDesktop(XRect(XPoint(1, 1), XGetSize()))
{
  strcpy(ObjClass, "XtApplication");
  CurrentFocusedClient = 0;
  StatusLine = 0;
  Menu = 0;
  XObject::SetState(SM_ACTIVE, true);
}

XtApplication::~XtApplication()
{
  if(StatusLine)
    delete StatusLine;
  if(Menu)
    delete Menu;
}

void XtApplication::Draw()
{
  XtDesktop::Draw();
  if(StatusLine)
    StatusLine->Draw();
  if(Menu)
    Menu->Draw();
  XObject::Draw();
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE | SM_FOCUSED, true);
}

void XtApplication::HandleEvent(XEvent *__Event)
{
  ClientList::iterator index = Clients.begin(), end = Clients.end();
  if(!__Event || __Event->Class == EV_NOTHING)
    return;
  if(ProcessEvent(__Event))
    return;
  XEventBroker::HandleEvent(__Event);
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

void XtApplication::Idle()
{
}

void XtApplication::InitMenuBar()
{
  Menu = new XtMenu(XRect(Bounds.a.x, Bounds.a.y, Bounds.b.x, Bounds.a.y), 0, 0);
}

void XtApplication::InitStatusLine()
{
  StatusLine = new XtStatusLine(XRect(Bounds.a.x, Bounds.b.y, Bounds.b.x, Bounds.b.y), 0,
    NewStatusItem("~Alt-X~ Exit", KB_META('x'), CM_QUIT,
    NewStatusItem("", KB_F(10), CM_MENU,
    0)));
}

bool XtApplication::ProcessCommand(XMessage *__Event)
{
  XMessage *msg;
  XObject *temp;
  ClientList::iterator index = Clients.begin(), end = Clients.end();
  bool found = false;
  switch(__Event->Message)
    {
    case CM_RESIZE:
      XClear();
      Size = XGetSize() - Bounds.a + XPoint(1, 1);
      CalculateBounds();
      if(Menu)
        {
        Menu->Origin = Bounds.a;
        Menu->Size.x = Size.x;
        Menu->CalculateBounds();
	}
      if(StatusLine)
        {
        StatusLine->Origin = XPoint(Bounds.a.x, Bounds.b.y);
        StatusLine->Size.x = Size.x;
        StatusLine->CalculateBounds();
	}
      for(; index != end; index++)
        {
	(*index)->CalculateBounds();
	if((*index)->GetState(SM_VISIBLE))
	  (*index)->Draw();
	}
      Draw();
      XRefresh();
      return true;
    case CM_CLOSE:
      if(CurrentFocusedClient)
        {
	temp = CurrentFocusedClient;
        for(index = Clients.begin(); *index != CurrentFocusedClient; index++);
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
	delete temp;
	end = Clients.end();
        if(!found)
	  {
	  CurrentFocusedClient = 0;
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
	  }
	}
      return true;
    }
  return false;
}

bool XtApplication::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  XMessage *msg;
  ClientList::iterator index = Clients.begin(), end = Clients.end();
  bool found = false;
  switch(__Event->KeyCode)
    {
    case KB_META(KB_TAB):
      if(!CurrentFocusedClient)
        return false;
      msg = new XMessage;
      msg->Message = MT_RELEASE_FOCUS;
      CurrentFocusedClient->ProcessMessage(msg);
      delete msg;
      for(index = Clients.begin(); *index != CurrentFocusedClient; index++);
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
      return true;
    }
  return false;
}

void XtApplication::RegisterClient(XObject *__Client)
{
  XMessage *msg;
  msg = new XMessage;
  msg->Message = MT_RELEASE_FOCUS;
  if(CurrentFocusedClient)
    CurrentFocusedClient->ProcessMessage(msg);
  XObject::RegisterClient(__Client);
  __Client->SetState(SM_NO_REFRESH, false);
  msg->Message = MT_RESUME_FOCUS;
  __Client->ProcessMessage(msg);
  delete msg;
  CurrentFocusedClient = __Client;
}

void XtApplication::Run()
{
  XEvent *ev;
  InitStatusLine();
  InitMenuBar();
  if(StatusLine)
    {
    AddListener(StatusLine->GetId());
    StatusLine->SetState(SM_FOCUSED, true);
    }
  if(Menu)
    AddListener(Menu->GetId());
  Draw();
  for(;;)
    {
    ev = FetchEvent();
    HandleEvent(ev);
    if(ev && ev->Class == EV_COMMAND && ((XMessage *)ev)->Message == CM_QUIT)
      break;
    Idle();
    if(ev)
      delete ev;
    }
  delete ev;
}

void XtApplication::RunClient(XObject *__Client)
{
  XEvent *ev;
  XMessage *msg;
  XObject::RegisterClient(__Client);
  __Client->SetState(SM_NO_REFRESH, false);
  msg = new XMessage;
  msg->Message = MT_RESUME_FOCUS;
  __Client->ProcessMessage(msg);
  delete msg;
  __Client->Draw();
  for(;;)
    {
    ev = FetchEvent();
    __Client->HandleEvent(ev);
    if(ev && ev->Class == EV_COMMAND && ((XMessage *)ev)->Message == CM_CLOSE)
      break;
    Idle();
    if(ev)
      delete ev;
    }
  delete ev;
  XObject::UnregisterClient(__Client);
}
