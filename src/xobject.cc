/*
  xobject.cc

  Copyright (c) 1995-99 Dragos Acostachioaie
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

#include <string.h>

#include <xtscreen.h>
#include <xevent.h>
#include <xobject.h>

XObject::XObject(XRect __Bounds)
{
  State = 0;
  Server = 0;
  strcpy(ObjClass, OC_XOBJECT);
  Id = _InsertObject(this);
  Origin = __Bounds.a;
  Size = __Bounds.b - __Bounds.a + XPoint(1, 1);
  Bounds = __Bounds;
  PalLen = 0;
}

XObject::~XObject()
{
  ClientList::iterator index = Clients.begin(), temp;
  ClientList::iterator end = Clients.end();
  for(; index != end; index = temp)
    {
    temp = index;
    temp++;
    delete *index;
    }
  Clients.erase(Clients.begin(), Clients.end());
  if(Server)
    Server->UnregisterClient(this);
  _DeleteObject(Id);
}

void XObject::CalculateBounds()
{
  ClientList::iterator index = Clients.begin();
  ClientList::iterator end = Clients.end();
  if(Server)
    Bounds.a = Server->Bounds.a + Origin;
   else
    Bounds.a = Origin;
  Bounds.b = Bounds.a + Size - XPoint(1, 1);
  for(; index != end; index++)
    (*index)->CalculateBounds();
}

bool XObject::DeleteClient(XObject *__Client)
{
  ClientList::iterator index = Clients.begin();
  ClientList::iterator end = Clients.end();
  bool found = false;
  for(; index != end; index++)
    if(*index == __Client)
      {
      found = true;
      Clients.erase(index);
      break;
      }
  return found;
}

void XObject::Draw()
{
  ClientList::iterator index = Clients.begin();
  ClientList::iterator end = Clients.end();
  for(; index != end; index++)
    (*index)->Draw();
}

XRect XObject::GetBounds()
{
  return Bounds;
}

XEvent *XObject::GetEvent()
{
  return FetchEvent(Id);
}

unsigned long XObject::GetId()
{
  return Id;
}

char *XObject::GetObjClass()
{
  return ObjClass;
}

XPoint XObject::GetOrigin()
{
  return Origin;
}

char *XObject::GetPalette()
{
  return Pal; 
}

unsigned XObject::GetPalEntry(unsigned __Index)
{
  return Terminal->GetPalEntry(Pal, PalLen, __Index);
}

XPoint XObject::GetSize()
{
  return Size;
}

unsigned long XObject::GetState()
{
  return State;
}

bool XObject::GetState(unsigned long __Mask)
{
  return State & __Mask;
}

void XObject::HandleEvent(XEvent *__Event)
{
  if(!__Event || __Event->Class == EV_NOTHING)
    return;
  if(!ProcessEvent(__Event))
    XEventBroker::HandleEvent(__Event);
}

bool XObject::InsertClient(XObject *__Client)
{
// "Clients" is a linked list, so we insert at the end of list
  Clients.push_back(__Client);
  return true;
}

void XObject::MoveTo(XPoint __p)
{
  Origin = __p;
  CalculateBounds();
}

bool XObject::ProcessEvent(XEvent *__Event)
{
  if(__Event->Class == EV_KEYBOARD && !GetState(SM_FOCUSED))
    return false;
  return XObjectEventListener::ProcessEvent(__Event);
}

void XObject::RegisterClient(XObject *__Client)
{
  if(InsertClient(__Client))
    {
    __Client->Server = this;
    __Client->CalculateBounds();
    __Client->SetState(SM_NO_REFRESH, true);
    }
}

void XObject::Resize(XPoint __delta)
{
  Size += __delta;
  CalculateBounds();
}

bool XObject::SendMessage(unsigned long __Receiver, unsigned long __Class, long __Message)
{
  XMessage *msg;
  if(IsObjectRegistered(__Receiver))
    {
    msg = new XMessage;
    msg->SenderId = Id;
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

bool XObject::SendMessage(unsigned long __Receiver, unsigned long __Class, void *__Body, unsigned long __Size)
{
  XMessage *msg;
  if(IsObjectRegistered(__Receiver))
    {
    msg = new XMessage;
    msg->SenderId = Id;
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

void XObject::SetPalette(char *__Pal, unsigned __PalLen)
{
  bcopy(__Pal, Pal, 80);
  PalLen = __PalLen;
  if(GetState(SM_VISIBLE))
    Draw();
}

void XObject::SetState(unsigned long __Mask, bool __BoolValue)
{
  State = (__BoolValue)? State | __Mask : State & (~__Mask);
}

void XObject::UnregisterClient(XObject *__Client)
{
  if(DeleteClient(__Client))
    {
    __Client->Server = 0;
    __Client->CalculateBounds();
    __Client->SetState(SM_NO_REFRESH, false);
    }
}

bool XObjectEventListener::ProcessEvent(XEvent *__Event)
{
  switch(__Event->Class)
    {
    case EV_MOUSE:
      return ProcessMouseEvent((XMouseEvent *)__Event);
    case EV_KEYBOARD:
      return ProcessKeyboardEvent((XKeyboardEvent *)__Event);
    case EV_MESSAGE:
      return ProcessMessage((XMessage *)__Event);
    case EV_COMMAND:
      return ProcessCommand((XMessage *)__Event);
    case EV_SIGNAL:
      return ProcessSignal(__Event);
    case EV_BROADCAST:
      return ProcessBroadcast(__Event);
    }
  return false;
}

bool XObjectEventListener::ProcessMouseEvent(XMouseEvent *__Event)
{
  return false;
}

bool XObjectEventListener::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  return false;
}

bool XObjectEventListener::ProcessMessage(XMessage *__Event)
{
  return false;
}

bool XObjectEventListener::ProcessCommand(XMessage *__Event)
{
  return false;
}

bool XObjectEventListener::ProcessSignal(XEvent *__Event)
{
  return false;
}

bool XObjectEventListener::ProcessBroadcast(XEvent *__Event)
{
  return false;
}

typedef std::map < unsigned long, XObject* > ObjectsMap;
typedef std::pair < const unsigned long, XObject* > ObjectsKeyValPair;

// map for binding Ids to XObjects
ObjectsMap _ObjectsMap;

XObject* GetOPtr(unsigned long __Id)
{
  ObjectsMap::iterator index = _ObjectsMap.begin();
  ObjectsMap::iterator end = _ObjectsMap.end();
  bool found = false;
  for(; index != end; index++)
    if((*index).first == __Id)
      {
      found = true;
      break;
      }
  return (found ? (*index).second : 0);
}

unsigned long GetOId(XObject *__Ptr)
{
  ObjectsMap::iterator index = _ObjectsMap.begin();
  ObjectsMap::iterator end = _ObjectsMap.end();
  bool found = false;
  for(; index != end; index++)
    if((*index).second == __Ptr)
      {
      found = true;
      break;
      }
  return (found ? (*index).first : 0);
}

bool IsObjectRegistered(unsigned long __Id)
{
  ObjectsMap::iterator index = _ObjectsMap.begin();
  ObjectsMap::iterator end = _ObjectsMap.end();
  bool found = false;
  for(; index != end; index++)
    if((*index).first == __Id)
      {
      found = true;
      break;
      }
  return found;
}

bool IsObjectRegistered(XObject *__Ptr)
{
  ObjectsMap::iterator index = _ObjectsMap.begin();
  ObjectsMap::iterator end = _ObjectsMap.end();
  bool found = false;
  for(; index != end; index++)
    if((*index).second == __Ptr)
      {
      found = true;
      break;
      }
  return false;
}

unsigned long _InsertObject(XObject *__Ptr)
{
  unsigned long i, found = 0;
  for(i = RESERVED_IDS + 1; i < MAX_OBJECTS; i++)
    if(!IsObjectRegistered(i))
      {
      found = i;
      break;
      }
  if(!found)
    return 0;
  ObjectsKeyValPair *pair = new ObjectsKeyValPair(found, __Ptr);
  _ObjectsMap.insert(*pair);
  return found;
}

void _DeleteObject(unsigned long __Id)
{
  ObjectsMap::iterator index = _ObjectsMap.begin();
  ObjectsMap::iterator end = _ObjectsMap.end();
  for(; index != end; index++)
    if((*index).first == __Id)
      {
      _ObjectsMap.erase(index);
      break;
      }
}

void _DeliverMessage(XMessage *__Msg)
{
  ObjectsMap::iterator index = _ObjectsMap.begin();
  ObjectsMap::iterator end = _ObjectsMap.end();
  if(__Msg->Type == MC_BROADCAST)
    for(; index != end; index++)
      {
      __Msg->Id = (*index).first;
      _PutEvent(__Msg);
      }
    else
     _PutEvent(__Msg);
}
