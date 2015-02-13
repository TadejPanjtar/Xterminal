/*
  event.h
  Definitions needed for event programming

  Copyright (c) 1995-2000 Dragos Acostachioaie
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

#ifndef _XEVENT_H
#define _XEVENT_H

#ifdef __GNUG__
#pragma interface
#endif

#include <deque.h>
#include <multimap.h>

#include <xbclass.h>

// Standard Event Classes
#define EV_NOTHING   0x0000
#define EV_KEYBOARD  0x0002
#define EV_MOUSE     0x0004
#define EV_SIGNAL    0x0100
#define EV_MESSAGE   0x0200
#define EV_COMMAND   0x0400
#define EV_BROADCAST 0x0800

/**
    @memo Basic event class
*/  
class XEvent {
 public:

  /// Object event is addressed
  unsigned long Id;

  /// Event class
  unsigned Class;

  /// Event type
  unsigned Type;
};

// Mouse Event Types
#define EV_MOUSEDOWN 0x0001
#define EV_MOUSEUP   0x0002
#define EV_MOUSEMOVE 0x0003

/**
    @memo Mouse event class
*/  
class XMouseEvent : public XEvent {
 public:

  /// Constructs a mouse event
  XMouseEvent();

  /// Button number
  unsigned Button;

  /// Number of clicks
  unsigned Clicks;

  /// Position of mouse pointer at the time event was generated
  XPoint Position;
};

// Keyboard Event Types
#define EV_KEYDOWN   0x0001

/**
    @memo Keyboard event class
*/  
class XKeyboardEvent : public XEvent {
 public:

  /// Constructs a keyboard event
  XKeyboardEvent();

  /// Key code
  unsigned KeyCode;

  /// Key code if it's a simple character
  unsigned CharCode;
};

// Message Classes
#define MC_CLIENT_TO_SERVER	0x01
#define MC_CLIENT_TO_CLIENT	0x02
#define MC_SERVER_TO_CLIENT	0x03
#define MC_BROADCAST		0x04

// Basic Messages
#define MT_ANSWER_REQUEST	0x0000
#define MT_REQUEST_HONORED	0x0001
#define MT_RELEASE_FOCUS	0x0002
#define MT_RESUME_FOCUS		0x0003
#define MT_FORWARD_FOCUS	0x0004
#define MT_BACKWARD_FOCUS	0x0005

/**
    @memo Message event class
*/  
class XMessage : public XEvent {
 public:

  /// Constructs a message
  XMessage();

  /// Destructs a message
  virtual ~XMessage();

  /// Object that sent the message
  unsigned long SenderId;

  /// a "long" message
  long Message;

  /// a "void" message, so we can send pointers between objects
  void *Body;

  /// size of the "void" message
  unsigned long Size;
};

/**
    @memo Event listener registration
*/
class XEventBroker
{
 public:

  /// Destructs the object's instance
  virtual ~XEventBroker();

  /** Registers an event listener for the specified event class.
      @param __Listener object id of listener 
      @param __Class event class
  */
  virtual void AddListener(unsigned long __Listener, unsigned __Class);

  virtual void AddListener(unsigned long __Listener);

  /** Removes the event listener for the specified event class.
      @param __Listener object id of listener 
      @param __Class event class
  */
  virtual void DelListener(unsigned long __Listener, unsigned __Class);

  /** Removes the specified event listener for all event classes.
      @param __Listener object id of listener
  */
  virtual void DelListener(unsigned long __Listener);

  /// Handles an incoming event and distributes it to all registered listeners.
  virtual void HandleEvent(XEvent *__Event);

  /** Sends a "long" message.
      @param __Receiver destination object id
      @param __Class message class
      @param __Message the message itself 
      @return false if receiver does not exist
  */
  virtual bool SendMessage(unsigned long __Receiver, unsigned long __Class, long __Message);

  /** Sends a "void *" message.
      @param __Receiver destination object id
      @param __Class message class
      @param __Body the message itself
      @param __Size size of __Body pointer
      @return false if receiver does not exist */
  virtual bool SendMessage(unsigned long __Receiver, unsigned long __Class, void *__Body, unsigned long __Size);

 protected:
  typedef multimap < unsigned, unsigned long, less < const unsigned > > Map;
  typedef pair < const unsigned, unsigned long > KeyValPair;

  /// map for binding listeners to events
  Map __map;
};

/** This is an abstract class.

    @memo Listener for registration at broker
*/
class XEventListener
{
 public:

  /** Process all events distributed from the broker.
      Derived listeners <b>have</b> to implement this method.
  */
  virtual bool ProcessEvent(XEvent *__Event)
    {
    return false;
    };
};

/** Clears the event passed as parameter.
    In fact, sets the Class field to EV_NOTHING, without deleting it
*/
void ClearEvent(XEvent *__Ev);

/// Returns the first available event for object __Id, zero if none
XEvent *FetchEvent(unsigned long __Id);

void InitEvents();
void DoneEvents();

void _InitEventQueue();
void _PutEvent(XEvent *__Ev);
void _PutEventInFront(XEvent *__Ev);
XEvent *_GetEvent();
XEvent *_GetFirstEvent();
bool _EventQueueIsEmpty();
void _ClearEventQueue();

// Standard Command Codes
#define CM_QUIT		0x0001
#define CM_MENU		0x0002
#define CM_CLOSE	0x0003
#define CM_OK		0x0004
#define CM_CANCEL	0x0005
#define CM_RESIZE	0x0006

#endif
