/*
  xtscreen.h
  Definitions for terminal-related functions and classes

  Copyright (c) 1995-2000 Dragos Acostachioaie

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

#ifndef _XTSCREEN_H
#define _XTSCREEN_H

#include <xconfig.h>

extern "C"
{
#ifdef HAVE_LIBGPM
#include <gpm.h>
#endif
#ifdef HAVE_NCURSES
#include <ncurses.h>
#endif
#ifdef HAVE_SLANG
#include <slcurses.h>
#undef bool		// why in hell does slang redefine `bool' as `int'?
#endif
};

#undef clear
#undef erase

#include <xbclass.h>
#include <xevent.h>
#include <string.h>
#include <stdlib.h>     /* div, div_t */

#ifndef chtype
typedef unsigned long chtype;
#endif

/**
    @memo Class that abstractize the input/output operations with the terminal
*/  
class XTerminal {
 public:

  /// Initialize the terminal
  XTerminal();

  /// Deinitialize the terminal
  virtual ~XTerminal();

  /// Sounds an audible alarm on the terminal
  virtual void Beep();

  /// Clears all terminal's screen content
  virtual void Clear();

  /** Fills a area with a specified color, clearing it's content
      @param __r area to fill
      @param __f desired foreground color
      @param __b desired background color
  */
  virtual void ColorWindow(XRect __r, unsigned __f, unsigned __b);

  /** Change the color of a specified area, without altering it's content
      @param __r area to fill
      @param __f desired foreground color
      @param __b desired background color
  */
  virtual void ColorWindowContent(XRect __r, unsigned __f, unsigned __b);

  /** Draws a box using some specified characters for it's margins
      @param __r box coordinates
      @param __frame margins type, with the following contents:
      <p>__frame[0] - the upper-left corner character
      <p>__frame[1] - the upper horizontal line character
      <p>__frame[2] - the upper-right corner character
      <p>__frame[3] - the left vertical line character
      <p>__frame[4] - the right vertical line character
      <p>__frame[5] - the lower-left corner character
      <p>__frame[6] - the lower horizontal line character
      <p>__frame[7] - the lower-right corner character
      <p>Predefined values:
      <p><b>FS_SINGLEFRAME</b> - single-line frame
      <p><b>FS_DOUBLEFRAME</b> - double-line frame
      <p><b>FS_FULLFRAME</b> - full-filled frame
  */
  virtual void DrawBorder(XRect __r, char *__frame);

  virtual void Dump();

  /** Fills a area with a specified character
      @param __r area to fill
      @param __c desired filling character
  */
  virtual void FillWindow(XRect __r, chtype __c);

  /// Gets the current foreground and background color, respectively
  virtual void GetColor(unsigned *__f, unsigned *__b);

  /// Returns the current cursor position
  virtual XPoint GetCursorPosition();

  /** Returns the first available event.
      This method have the following behaviour:
      <p>- if <i>TI_BLOCKING_INPUT</i> is set, the function waits until an event
           coming from keyboard or mouse arrise, and then returns it
      <p>- if <i>TI_BLOCKING_INPUT</i> is not set, the function returns the first
           available event coming from keyboard or mouse, if any. If nothing is
	   available, the function returns zero
  */
  virtual XEvent *GetEvent();

  /** Returns the value of the terminal input state. The common used states are:
      <p><b>TI_BLOCKING_INPUT</b> indicates that \Ref{GetEvent} waits until an
           event from keyboard or mouse arrise
      <p><b>TI_USE_FUNCTION_KEYS</b> indicates that pressing the function keys will
           generate a keyboard event; this flag should normally be set
      <p><b>TI_INSERT_MODE</b> indicates that pressing a letter inserts it into
           the text being edited
      @param __Mask state mask
      @return non-zero if bit set
  */
  virtual unsigned GetInputState(unsigned long __Mask);

  /// Returns the current timeout used for special codes receiving
  virtual int GetInputTimeout();

  /// Returns the current terminal's name
  virtual char *GetName();

  /** Returns the correct palette entry
      @param __Pal object's palette
      @param __Len object's palette length
      @param __Index color position into the palette (1 = first)
  */
  virtual unsigned GetPalEntry(char *__Pal, unsigned __Len, unsigned __Index);

  /** Returns the value of the terminal output state. The common used states:
      <p><b>TO_HAVE_COLORS</b> indicates that the terminal can display colors
      <p><b>TO_CURSOR_VISIBLE</b> indicates that the cursor is visible
      <p><b>TO_CURSOR_VERY_VISIBLE</b> indicates that the cursor is very visible
      @param __Mask state mask
      @return non-zero if bit set
  */
  virtual unsigned GetOutputState(unsigned long __Mask);

  /// Returns the terminal's size
  virtual XPoint GetSize();

  /** Returns the terminal general state. The common used states are:
      <p><b>TM_IS_PC_CONSOLE</b> indicates that the terminal is a PC console
      <p><b>TM_CAN_RESIZE</b> indicates that the terminal window can be resized
      <p><b>TM_HAVE_MOUSE</b> indicates that the terminal have mouse support
      <p><b>TM_HAVE_GPM_MOUSE</b> indicates that the GPM mouse support is activated
      @param __Mask state mask
      @return true if bit set
  */
  virtual bool GetState(unsigned long __Mask);

  /** Save the visual content of a specified area into a memory area.
      This method DOES NOT allocate memory space for the saved area content,
      instead it assumes that at least (__r.b.x - __r.a.x + 1) * (__r.b.y -
      __r.b.x + 1) * sizeof(chtype) bytes are already allocated
      @param __r area to save
      @param __buff pointer to the beggining of memory area
  */
  virtual void GetWindow(XRect __r, chtype *__buff);

  /** Checks if a point have valid coordinates
      @param __p point to check
  */
  virtual bool IsValid(XPoint __p);

  /** Checks if a rectangle have valid coordinates
      @param __r rectangle to check
  */
  virtual bool IsValid(XRect __r);

  /** Moves the cursor to a specified location
      @param __p desired new cursor location
  */
  virtual void MoveCursor(XPoint __p);

  /** Prints something to the terminal. This function is analog to printf, and
      performs the output using a variable argument list
      @param __p location to start
  */
  virtual void Print(XPoint __p, char *__fmt, ...);

  /** Prints a single character to the terminal
      @param __p location to start
      @param __c character to output
  */
  virtual void PrintChar(XPoint __p, chtype __c);

  /** Effectively dumps the current terminal contents to the screen.
      You must understand that all the output methods of \Ref{XTerminal}
      class does not implies screen actualization. The drawing is done only
      when calling this function
  */
  virtual void Refresh();

  /// Resizes the terminal using current values
  virtual void Resize();

  /// Restores a previous suspended terminal
  virtual void Resume();

  /** Copies a previous saved screen area to the terminal
      @param __r area to copy
      @param __buff pointer to the beggining of memory area
  */
  virtual void PutWindow(XRect __r, chtype *__buff);

  /** Sets the current terminal color. Different attributes can be ORed with
      the __f parameter
      <p><b>TA_UNDERLINE</b> underlining
      <p><b>TA_REVERSE</b> reverse video
      <p><b>TA_BLINK</b> blinking
      <p><b>TA_BOLD</b> extra bright
      <p><b>TA_HALF_BRIGHT</b> half bright
      @param __f desired foreground color
      @param __b desired background color
  */
  virtual void SetColor(unsigned __f, unsigned __b);

  /** Sets the state of the specified terminal input state.
      Please see GetInputState() for details
      @param __Mask state mask
      @param __BoolValue set bit if non-zero, otherwise clear it
  */
  virtual void SetInputState(unsigned long __Mask, bool __BoolValue);

  /** Sets the state of the specified terminal output state.
      Please see GetOutputState() for details
      @param __Mask state mask
      @param __BoolValue set bit if non-zero, otherwise clear it
  */
  virtual void SetOutputState(unsigned long __Mask, bool __BoolValue);

  /** Sets the state of the specified terminal general state.
      Please see GetState() for details
      @param __Mask state mask
      @param __BoolValue set bit if non-zero, otherwise clear it
  */
  virtual void SetState(unsigned long __Mask, bool __BoolValue);

  /// Saves the current terminal state and falls back to startup state
  virtual void Suspend();

 protected:

  /// Current input, output and respectively general properties of the terminal
  unsigned long InputState, OutputState, State;

  /// Current cursor position
  XPoint Cursor;

  /// Terminal size
  XPoint Size;

  /// Terminal name
  char *Name;

  /// Current foreground, respectively background color
  unsigned ForegroundColor, BackgroundColor;

  /// True if generating events on function keys pressing (normally set)
  unsigned UseFunctionKeys;

  /// Timeout for receiving special codes (e.g. for function keys)
  int InputTimeout;

  /// Timeout for receiving ESC codes
  int EscTimeout;

  /// Called by constructor
  virtual void Init();

  /// Called by destructor
  virtual void Done();
};

/// Returns the first available event, zero if none
XEvent *FetchEvent();

/** Global XTerminal class instantiation.
    Initialized/destroyed normally by <i>XInit()</i> and <i>XDone()</i>
*/
extern XTerminal *Terminal;

extern char FS_DEFAULT[9];

// Input options
#define TI_BLOCKING_INPUT	0x0001
#define TI_USE_FUNCTION_KEYS	0x0002
#define TI_INSERT_MODE		0x0004

// Output options
#define TO_HAVE_COLORS		0x0001
#define TO_CURSOR_VISIBLE	0x0002
#define TO_CURSOR_VERY_VISIBLE  0x0004

// Miscellaneous options
#define TM_IS_PC_CONSOLE	0x0001
#define TM_CAN_RESIZE		0x0002
#define TM_HAVE_MOUSE		0x0004
#define TM_HAVE_GPM_MOUSE	0x0008

// Attributes
#define TA_UNDERLINE		0x0008
#define TA_REVERSE		0x0010
#define TA_BLINK		0x0020
#define TA_BOLD			0x0040
#define TA_HALF_BRIGHT		0x0080

// Key definitions
#define KB_TAB			0x0009
#ifdef HAVE_SLANG
#define KB_ENTER		KEY_ENTER
#else
#define KB_ENTER		0x000a
#endif
#define KB_ESC			0x001b
#define KB_SPACE		0x0020
#define KB_BACK			0x007f
#define KB_BACKSPACE		KEY_BACKSPACE
#define KB_DOWN			KEY_DOWN
#define KB_UP			KEY_UP
#define KB_LEFT			KEY_LEFT
#define KB_RIGHT		KEY_RIGHT
#define KB_HOME			KEY_HOME
#define KB_NPAGE		KEY_NPAGE
#define KB_PPAGE		KEY_PPAGE
#define KB_HOME			KEY_HOME
#define KB_END			KEY_END
#ifdef HAVE_SLANG
#define KB_IC			SL_KEY_IC
#define KB_DC			SL_KEY_DELETE
#else
#define KB_IC			KEY_IC
#define KB_DC			KEY_DC
#endif
#define KB_F(n)			(KEY_F0 + (n))
#define KB_META(c)		(32768 + (c))

// Window frames (borders) definitions
#define FS_SINGLEFRAME		"\xda\xc4\xbf\xb3\xb3\xc0\xc4\xd9"
#define FS_DOUBLEFRAME		"\xc9\xcd\xbb\xba\xba\xc8\xcd\xbc"
#define FS_FULLFRAME		"\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb"

// Various definitions
#define ESC_TIMEOUT		100
#define SHORTCUT_LETTER		'~'

// Action keys
#define TERMINAL_REFRESH_KEY	KB_META('l')

// Defitions for terminal access functions
#define XBeep() Terminal->Beep()
#define XClear() Terminal->Clear()
#define XColorWindow(__r, __f, __b) Terminal->ColorWindow(__r, __f, __b)
#define XColorWindowContent(__r, __f, __b) Terminal->ColorWindowContent(__r, __f, __b)
#define XDrawBorder(__r, __frame) Terminal->DrawBorder(__r, __frame)
#define XFillWindow(__r, __c) Terminal->FillWindow(__r, __c)
#define XGetColor(__f, __b) Terminal->GetColor(__f, __b)
#define XGetCursorPosition() Terminal->GetCursorPosition()
#define XGetEvent() Terminal->GetEvent()
#define XGetInputState(__Mask) Terminal->GetInputState(__Mask)
#define XGetOutputState(__Mask) Terminal->GetOutputState(__Mask)
#define XGetSize() Terminal->GetSize()
#define XGetState(__Mask) Terminal->GetState(__Mask)
#define XGetWindow(__r, __buff) Terminal->GetWindow(__r, __buff)
#define XIsValid(__p) Terminal->IsValid(__p)
#define XMoveCursor(__p) Terminal->MoveCursor(__p)
#define XPrint Terminal->Print
#define XPrintChar(__p, __c) Terminal->PrintChar(__p, __c)
#define XRefresh() Terminal->Refresh()
#define XPutWindow(__r, __buff) Terminal->PutWindow(__r, __buff)
#define XSetColor(__f, __b) Terminal->SetColor(__f, __b)
#define XSetInputState(__Mask, __BoolValue) Terminal->SetInputState(__Mask, __BoolValue)
#define XSetOutputState(__Mask, __BoolValue) Terminal->SetOutputState(__Mask, __BoolValue)
#define XSetState(__Mask, __BoolValue) Terminal->SetState(__Mask, __BoolValue)

#endif
