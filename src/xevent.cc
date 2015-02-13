/*
  xevent.cc

  Copyright (c) 1995-98 Dragos Acostachioaie
  Copyright (c) 1998 Bernd Kalbfuss

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

#ifdef __GNUG__
#pragma implementation
#endif

#include <xevent.h>
#include <xobject.h>

std::deque < XEvent* > _EventQueue;

void ClearEvent(XEvent *__Ev)
{
  if(__Ev)
    __Ev->Class = EV_NOTHING;
}

void _InitEventQueue()
{
}

void _PutEvent(XEvent *__Ev)
{
  _EventQueue.push_back(__Ev);
}

void _PutEventInFront(XEvent *__Ev)
{
  _EventQueue.push_front(__Ev);
}

XEvent *_GetEvent()
{
  XEvent *ev;
  ev = _EventQueue.front();
  _EventQueue.pop_front();
  return ev;
}

XEvent *_GetFirstEvent()
{
  return _EventQueue.front();
}

bool _EventQueueIsEmpty()
{
  return _EventQueue.empty();
}

void _ClearEventQueue()
{
  while(!_EventQueue.empty())
    _EventQueue.pop_front();
}

void InitEvents()
{
  _InitEventQueue();
}

void DoneEvents()
{
  _ClearEventQueue();
}

XEvent *FetchEvent(unsigned long __Id)
{
  XEvent *ev;
  if((ev = _GetFirstEvent()) && ev->Id == __Id)
    return ev;
   else
    return 0;
}

XMouseEvent::XMouseEvent()
{
  Class = EV_MOUSE;
}

XKeyboardEvent::XKeyboardEvent()
{
  Class = EV_KEYBOARD;
}

XMessage::XMessage()
{
  Class = EV_MESSAGE;
  Body = 0;
}

XMessage::~XMessage()
{
  if(Body)
    delete Body;
}

XEventBroker::~XEventBroker()
{
  Map::iterator index = __map.begin();
  Map::iterator end = __map.end();
  for(; index != end; index++)
    __map.erase(index);
}

void XEventBroker::AddListener(unsigned long __Listener, unsigned __Class)
{
// Ensure that the listener is not already registered for the specified event.
  Map::iterator lower = __map.lower_bound(__Class);
  Map::iterator upper = __map.upper_bound(__Class);
// Search only for the listener if at least one listener is registered.
  if(lower != __map.end())
    for(; lower != upper; lower++)
      if((*lower).second == __Listener)
        return;

/* Register the listener for the specified event by adding a new
   key value pair to the map using the event as key and the listener
   as value.
*/
  KeyValPair *pair = new KeyValPair(__Class, __Listener);
  __map.insert(*pair);
}

void XEventBroker::AddListener(unsigned long __Listener)
{
  AddListener(__Listener, EV_MOUSE);
  AddListener(__Listener, EV_KEYBOARD);
  AddListener(__Listener, EV_MESSAGE);
  AddListener(__Listener, EV_COMMAND);
}

void XEventBroker::DelListener(unsigned long __Listener, unsigned __Class)
{
/* Scan through listeners registered for the specified event 
   and delete the specified listener */	   
  Map::iterator lower = __map.lower_bound(__Class);
  Map::iterator upper = __map.upper_bound(__Class);
// Search only for the listener if at least one listener is registered.
  if(lower != __map.end())
    for(; lower != upper; lower++)
      if((*lower).second == __Listener) 
	{
	__map.erase(lower);
	return;
	}
}

void XEventBroker::DelListener(unsigned long __Listener)
{
/* Scan through registerd listeners and delete the specified
   listener whenever it occurs. */
  Map::iterator lower = __map.begin();
  Map::iterator upper = __map.end();
  for(; lower != upper; lower++)
  /* Erase the listener from the map. We cannot simply continue
     however, as an erase invalidates the iterators. So we
     erase successing entries recursivly. */
    if((*lower).second == __Listener)
      {
      __map.erase(lower);
      DelListener(__Listener);
      return;
      }
}

void XEventBroker::HandleEvent(XEvent *__Event)
{
  XEventListener *listener;
  Map::iterator lower = __map.lower_bound(__Event->Class);
  Map::iterator upper = __map.upper_bound(__Event->Class);
  if(lower != __map.end())
    for(; lower != upper; lower++)
      {		
      listener = GetOPtr((*lower).second);
      if(listener->ProcessEvent(__Event))
        {
	ClearEvent(__Event);
	break;
	}
      }
}
	
bool XEventBroker::SendMessage(unsigned long __Receiver, unsigned long __Class, long __Message)
{
  XMessage *msg;
  if(IsObjectRegistered(__Receiver))
    {
    msg = new XMessage;
    msg->SenderId = GetOId((XObject *)this);
    msg->Id = __Receiver;
    msg->Type = __Class;
    msg->Message = __Message;
    msg->Body = 0;
    msg->Size = 0;
    _DeliverMessage(msg);
    return true;
    }
  return false;
}
	
bool XEventBroker::SendMessage(unsigned long __Receiver, unsigned long __Class, void *__Body, unsigned long __Size)
{
  XMessage *msg;
  if(IsObjectRegistered(__Receiver))
    {
    msg = new XMessage;
    msg->SenderId = GetOId((XObject *)this);
    msg->Id = __Receiver;
    msg->Type = __Class;
    msg->Message = 0;
    msg->Body = __Body;
    msg->Size = __Size;
    _DeliverMessage(msg);
    return true;
    }
  return false;
}
