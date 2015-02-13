/*
  xtbar.h
  Definitions for XtScrollBar and XtProgressBar

  Copyright (c) 1996-97 Adrian Teohar
  Copyright (c) 1996-99 Dragos Acostachioaie
  Documentation Copyright (c) 1998-99 Dragos Acostachioaie

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

#ifndef _XTBAR_H
#define _XTBAR_H

#include <xobject.h>

// Scroll Bar Flags
#define SB_HORIZONTAL		0x01
#define SB_INDICATE_PERCENT	0x02

/**
    @memo Scroll bar
*/
class XtScrollBar : public XObject {
 public:

  /** Constructs a new XtScrollBar
      @param __r bar bounds
      @param __TotalNo total items number
      @param __ Flags options, which can be:
         <p><b>SB_HORIZONTAL</b> scroll bar is horizontally positioned
	 <p><b>SB_INDICATE_PERCENT</b> scroll bar indicates what percent of
	   total items are being viewed. By default, the scroll bar indicates
	   the current item number.
  */
  XtScrollBar(XRect __r, unsigned long __TotalNo, unsigned long __Flags);

  /// Draws the object
  virtual void Draw();

  /// Returns the total items number
  virtual unsigned long GetTotalNo();

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /// Change the total items number
  virtual void SetTotalNo(unsigned long __ItemNo);

  /// Change the current item number
  virtual void SetValue(unsigned long __ItemNo);

 protected:
  unsigned long Flags;
  unsigned long CurrentNo, MaxNo;
};  

// Progress Bar directions
#define PB_LEFT_TO_RIGHT	0x01
#define PB_RIGHT_TO_LEFT	0x02
#define PB_BOTTOM_TO_TOP	0x03
#define PB_TOP_TO_BOTTOM	0x04

/**
    @memo Progress bar
*/
class XtProgressBar : public XObject {
 public:

  /** Constructs a new XtProgressBar
      @param __r bar bounds
      @param __TotalNo total items number
      @param __Direction direction the progress is going, which can be:
         <p><b>PB_LEFT_TO_RIGHT</b> left to the right
	 <p><b>PB_RIGHT_TO_LEFT</b> right to the left
	 <p><b>PB_BOTTOM_TO_TOP</b> bottom to the top
	 <p><b>PB_TOP_TO_BOTTOM</b> top to the bottom
  */
  XtProgressBar(XRect __r, unsigned long __TotalNo, unsigned __Direction);

  /// Draws the object
  virtual void Draw();

  /// Returns the progress percentual value
  virtual float GetPercent();

  /// Returns the total items number
  virtual unsigned long GetTotalNo();

  /// Returns the current count value
  virtual unsigned long GetValue();

  /// Change the total items number
  virtual void SetTotalNo(unsigned long __ItemNo);

  /// Change the current count value
  virtual void SetValue(unsigned long __ItemNo);

 protected:
  unsigned long CurrentNo, MaxNo;
  float Percent;
  unsigned Len, Direction;
};

// Events
#define SCROLLBAR_ITEMUP	0x0010
#define SCROLLBAR_ITEMDOWN	0x0011

// Palette entries
#define P_BAR_LEN	4

#define P_BAR_MARKERS	0x01
#define P_BAR_BAR	0x03

// Default palette
#define P_BAR		"\x47\x06\x07\x06\x17\x00\x07\x00"

#endif
