/*
  xobject.h
  Definitions for XObject and objects table

  Copyright (c) 1995-99 Dragos Acostachioaie
  Copyright (c) 1998 Bernd Kalfbuss

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

#ifndef _XOBJECT_H
#define _XOBJECT_H

#ifdef __GNUG__
#pragma interface
#endif

#include <list>
#include <map>

#include <xevent.h>

/**
    @memo Listener for registration at broker
*/
class XObjectEventListener : public XEventListener
{
 public:

  /** Receives incoming events and distributes them between the following
      processing methods.

      These methods should be overloaded for active objects, of course only for
      event classes the respective object cares. If the object treats the event,
      these functions must returns true, otherwise false. That's because
      Xterminal can't figure out all the times which object an event is for.
      Instead, events are passed to all registered listeners and then objects
      are watched to see which is responding to it (an example of such event is
      an mouse event). Once the event gets processed, the remaining listeners
      does not receive it any more.
  */
  virtual bool ProcessEvent(XEvent *__Event);

  /** Process a mouse event (<b>EV_MOUSE</b>).
      Be careful with this kind of events because you <b>must</b> check if the
      mouse pointer is inside object's bounds, otherwise you must return false!
  */
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /// Process a keyboard event (<b>EV_KEYBOARD</b>)
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process a message (<i>EV_MESSAGE</i>)
  virtual bool ProcessMessage(XMessage *__Event);

  /// Process a command (<i>EV_COMMAND</i>)
  virtual bool ProcessCommand(XMessage *__Event);

  /// Process a signal (<i>EV_SIGNAL</i>)
  virtual bool ProcessSignal(XEvent *__Event);

  /// Process a broadcast event (<i>EV_BROADCAST</i>)
  virtual bool ProcessBroadcast(XEvent *__Event);
};

/** Inherited by most Xterminal classes

    @memo Foundation class
*/  
class XObject : public XEventBroker, public XObjectEventListener {
 public:

  /// Constructs a new XObject with the specified bounds
  XObject(XRect __Bounds);

  /// Destructs the object's instance
  virtual ~XObject();

  /** Computes the object's size and the real bounds.
      This method is used by the \Ref{RegisterClient} method
  */
  virtual void CalculateBounds();

  /** Paints the visual object content onto the terminal's screen.
      Remember to call XRefresh() before return so the draw becomes visible!
  */
  virtual void Draw();

  /// Returns the object's bounds
  XRect GetBounds();

  /// Returns the first event addressed to this object, zero if none
  virtual XEvent *GetEvent();

  /// Returns the object's id
  unsigned long GetId();

  /// Returns the objects's class name
  char *GetObjClass();

  /// Returns the object's origin
  XPoint GetOrigin();

  /// Returns the object's attached color palette
  virtual char *GetPalette();

  /// Returns the color value with __Index position inside the palette
  virtual unsigned GetPalEntry(unsigned __Index);

  /// Returns the object's size
  XPoint GetSize();

  /** Returns the object state register.
      The returned value must be processed next with the state masks the
      object uses to gain valuable information
  */
  unsigned long GetState();

  /** Returns the value of the specified object's state
      @param __Mask state mask
      @return true if bit set
  */
  virtual bool GetState(unsigned long __Mask);

  /// Handles the event passed as argument, by calling <i>ProcessEvent()</i>
  virtual void HandleEvent(XEvent *__Event);

  /// Moves the object's origin to the specified point
  virtual void MoveTo(XPoint __p);

  /** Receives incoming events and distributes them using the
      \Ref{XObjectEventListener::ProcessEvent} method of
      \Ref{XObjectEventListener} class.
      (also being careful not to treat keyboard events if the object does not
      own the focus)
      @return true if event was processed
  */
  virtual bool ProcessEvent(XEvent *__Event);

  /** Registers the specified XObject as client.
      It also adjusts the client's bounds and enables the <i>SM_NO_REFRESH</i>
      flag for the newly registered client
  */
  virtual void RegisterClient(XObject *__Client);

  /** Adjusts the object's size with the specified coordinates
      @param __delta translation vector
  */
  virtual void Resize(XPoint __delta);

  /** Sets the object's attached color palette
      @param __Pal new palette
      @param __PalLen new palette size
  */
  virtual void SetPalette(char *__Pal, unsigned __PalLen);

  /** Sets the value of the specified object state
      @param __Mask state mask
      @param __BoolValue set bit if non-zero, otherwise clear it
  */
  virtual void SetState(unsigned long __Mask, bool __BoolValue);

  /// Unregisters the specified client
  virtual void UnregisterClient(XObject *__Client);

 protected:

  /// Object's unique identificator
  unsigned long Id;

  /** Current properties of the object.
      The different states can be ORed. The common used states are:
      <p><b>SM_VISIBLE</b> indicates that the object is currently visible on the
           terminal's screen, so it is in a drawn state
      <p><b>SM_CURSOR_VISIBLE</b> indicates that the cursor is currently visible and
           it's inside the object's bounds
      <p><b>SM_FOCUSED</b> indicates that the object has the control at the
	   respective moment (only one object can have the control at a
	   moment, so if the user press, let's say, a key, this object will
	   receive the event)
      <p><b>SM_DRAGGING</b> indicates that the objects is currently dragged on the
           terminal's screen
      <p><b>SM_DISABLED</b> indicates that the object is deactivated at the moment
      <p><b>SM_ACTIVE</b> indicates that the object is active, so the user has
	   control over it
      <p><b>SM_NO_REFRESH</b> indicates that the object's Draw() method will not
           dump it's contents to the screen by calling XRefresh(), but will let
	   it's server to do this instead
  */
  unsigned long State;

  /// Object's upper-left coordinate, relative to it's server
  XPoint Origin;

  /// Object's size
  XPoint Size;

  /// Cursor's position inside the object's bounds
  XPoint Cursor;

  /// This are the *real* bounds, in absolute coordinates
  XRect Bounds;

  /// Name of the class the object belongs, choosen by the programmer
  char ObjClass[40];

  /// Object's attached color palette
  char Pal[80];

  /// Palette size
  unsigned PalLen;

  /** Server to which the object is registered (and which services he uses),
      or zero if the object have no server.
      (If you are not familiar with the client-server concept, try to think
      to the server as the object's parent)
  */
  XObject *Server;

  typedef std::list < XObject * > ClientList;

  /// Linked list of object's registered clients
  ClientList Clients;

  /** Sends a "long" message
      @param __Receiver destination object id
      @param __Class message class
      @param __Message the message itself
      @return false if destination does not exists
  */
  bool SendMessage(unsigned long __Receiver, unsigned long __Class, long __Message);

  /** Sends a "void *" message
      @param __Receiver destination object id
      @param __Class message class
      @param __Body the message itself
      @param __Size size of __Body pointer
      @return false if destination does not exists
  */
  bool SendMessage(unsigned long __Receiver, unsigned long __Class, void *__Body, unsigned long __Size);

 private:
  bool InsertClient(XObject *__Client);
  bool DeleteClient(XObject *__Client);
};

// XObject State Masks
#define SM_VISIBLE		0x0001
#define SM_CURSOR_VISIBLE	0x0002
#define SM_FOCUSED		0x0004
#define SM_DRAGGING		0x0008
#define SM_DISABLED		0x0010
#define SM_EXPOSED		0x0020
#define SM_ACTIVE		0x0040
#define SM_NO_REFRESH		0x0080
#define SM_HANDLE_ARROWS       	0x0100

// Object Classes
#define OC_UNKNOWN       "Unkown"
#define OC_XOBJECT       "XObject"

#define RESERVED_IDS	0x0f
#define MAX_OBJECTS	0xfffffff0

bool IsObjectRegistered(unsigned long __Id);
bool IsObjectRegistered(XObject *__Ptr);
XObject* GetOPtr(unsigned long __Id);
unsigned long GetOId(XObject *__Ptr);

unsigned long _InsertObject(XObject *__Ptr);
void _DeleteObject(unsigned long __Id);
void _DeliverMessage(XMessage *__Msg);

#endif
