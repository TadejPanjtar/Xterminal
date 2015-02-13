/*
  xtscreen.cc

  Copyright (c) 1995-2002 Dragos Acostachioaie

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

#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <xtlib.h>
#include <xtscreen.h>

char FS_DEFAULT[9] = "+-+||+-+";
XTerminal *Terminal;
chtype *_buffer;
unsigned long _State, _InputState, _OutputState;
XPoint _Cursor;
unsigned _ForegroundColor, _BackgroundColor;
#ifdef HAVE_LIBGPM
fd_set _GpmFdSet;
#endif
#ifndef HAVE_ESCDELAY
int ESCDELAY;
#endif

XTerminal::XTerminal()
{
  Init();
}

XTerminal::~XTerminal()
{
  delete Name;
  Done();
}

void XTerminal::Beep()
{
  beep();
}

void XTerminal::Clear()
{
  wclear(stdscr);
}

void XTerminal::ColorWindow(XRect __r, unsigned __f, unsigned __b)
{
  if(!IsValid(__r))
    {
#ifdef DEBUG
    XDebug("XTerminal::ColorWindow(): (%d, %d, %d, %d) invalid coordinates",
      __r.a.x, __r.a.y, __r.b.x, __r.b.y);
#endif
    return;
    }
  SetColor(__f, __b);
  FillWindow(__r, ' ');
}

void XTerminal::ColorWindowContent(XRect __r, unsigned __f, unsigned __b)
{
  chtype c;
  if(!IsValid(__r))
    {
#ifdef DEBUG
    XDebug("XTerminal::ColorWindowContent(): (%d, %d, %d, %d) invalid coordinates",
      __r.a.x, __r.a.y, __r.b.x, __r.b.y);
#endif
    return;
    }
  for(unsigned j = __r.a.y; j <= __r.b.y; j++)
    for(unsigned i = __r.a.x; i <= __r.b.x; i++)
      {
#if defined(HAVE_NCURSES) && defined(USE_NCURSES_HACKS)
#ifdef NCURSES_VERSION
      c = stdscr->_line[j - 1].text[i - 1];
#else
      c = stdscr->_line[j - 1][i - 1];
#endif
#else
      c = mvwinch(stdscr, j - 1, i - 1);
#endif
      SetColor(__f, __b);
      PrintChar(XPoint(i, j), (c & A_CHARTEXT) | (c & A_ALTCHARSET));
      }
}

void XTerminal::Done()
{
  SetOutputState(TO_CURSOR_VISIBLE, 1);
  wclear(stdscr);
  refresh();
  endwin();
#ifdef HAVE_LIBGPM
  if(GetState(TM_HAVE_GPM_MOUSE))
    Gpm_Close();
#endif
}

void XTerminal::DrawBorder(XRect __r, char *__frame)
{
  unsigned i, j;
  bool isdefault = false;
  if(!IsValid(__r))
    {
#ifdef DEBUG
    XDebug("XTerminal::DrawBorder(): (%d, %d, %d, %d) invalid coordinates",
      __r.a.x, __r.a.y, __r.b.x, __r.b.y);
#endif
    return;
    }
  if(GetState(TM_IS_PC_CONSOLE))
    attron(A_ALTCHARSET);
  if(!strcmp(__frame, FS_DEFAULT))
    isdefault = true;
  if(isdefault)
    PrintChar(XPoint(__r.a.x, __r.a.y), ACS_ULCORNER);
   else
    PrintChar(XPoint(__r.a.x, __r.a.y), (unsigned char)__frame[0]);
  for(i = __r.a.x + 1; i < __r.b.x; i++)
    if(isdefault)
      {
      PrintChar(XPoint(i, __r.a.y), ACS_HLINE);
      PrintChar(XPoint(i, __r.b.y), ACS_HLINE);
      }
     else
      {
      PrintChar(XPoint(i, __r.a.y), (unsigned char)__frame[1]);
      PrintChar(XPoint(i, __r.b.y), (unsigned char)__frame[6]);
      }
  if(isdefault)
    PrintChar(XPoint(__r.b.x, __r.a.y), ACS_URCORNER);
   else
    PrintChar(XPoint(__r.b.x, __r.a.y), (unsigned char)__frame[2]);
  for(j = __r.a.y + 1; j < __r.b.y; j++) 
    if(isdefault)
      {
      PrintChar(XPoint(__r.a.x, j), ACS_VLINE);
      PrintChar(XPoint(__r.b.x, j), ACS_VLINE);
      }
     else
      {
      PrintChar(XPoint(__r.a.x, j), (unsigned char)__frame[3]);
      PrintChar(XPoint(__r.b.x, j), (unsigned char)__frame[4]);
      }
  if(isdefault)
    {
    PrintChar(XPoint(__r.a.x, __r.b.y), ACS_LLCORNER);
    PrintChar(XPoint(__r.b.x, __r.b.y), ACS_LRCORNER);
    }
   else
    {
    PrintChar(XPoint(__r.a.x, __r.b.y), (unsigned char)__frame[5]);
    PrintChar(XPoint(__r.b.x, __r.b.y), (unsigned char)__frame[7]);
    }
}

void XTerminal::Dump()
{
    chtype buff[Size.x * Size.y];

    GetWindow(XRect(1, 1, Size.x, Size.y), buff);
    Clear();
    PutWindow(XRect(1, 1, Size.x, Size.y), buff);
    Refresh();
}

void XTerminal::FillWindow(XRect __r, chtype __c)
{
  if(!IsValid(__r))
    {
#ifdef DEBUG
    XDebug("XTerminal::FillWindow(): (%d, %d, %d, %d) invalid coordinates",
      __r.a.x, __r.a.y, __r.b.x, __r.b.y);
#endif
    return;
    }
  for(unsigned j = __r.a.y; j <= __r.b.y; j++)
    for(unsigned i = __r.a.x; i <= __r.b.x; i++)
      PrintChar(XPoint(i, j), __c);
}

void XTerminal::GetColor(unsigned *__f, unsigned *__b)
{
  *__f = ForegroundColor;
  *__b = BackgroundColor;
}

XPoint XTerminal::GetCursorPosition()
{
  return Cursor;
}

XEvent* XTerminal::GetEvent()
{
  int ch;
  XEvent *ev;
#ifdef HAVE_LIBGPM
  timeval t;
  fd_set ready = _GpmFdSet;
  int _GetTtyKey();

  if(GetState(TM_HAVE_GPM_MOUSE))
    {
    if(GetInputState(TI_BLOCKING_INPUT))
      ch = Gpm_Getchar();
     else
      {
      /* Check if gpm_fd descriptor is readable. Otherwise, Gpm_Getchar() will
         block until an event occurs.
      */
      t.tv_sec = 0;
      t.tv_usec = 0;
      if(select(FD_SETSIZE, &ready, 0, 0, &t) <= 0)
        return 0;
      ch = Gpm_Getchar();
      }
    // a mouse event occured (queued)
    if(gpm_hflag)
      return _GetEvent();
    }
   else
    ch = getch();
#else
  ch = getch();
#endif // HAVE_LIBGPM
  if(ch == ERR)
    return 0;
#ifdef NCURSES_MOUSE_VERSION
  if(ch == KEY_MOUSE)
    {
    MEVENT _ev;
    int i, j, _clicks = 0, _press = 0;
    if(getmouse(&_ev) == ERR)
      return 0;
    for(i = 1; i <= 4; i++)
      {
      if(BUTTON_RELEASE(_ev.bstate, i))
        {
	_press = 0;
	break;
	}
      if(BUTTON_PRESS(_ev.bstate, i))
	{
	_press = 1;
 	break;
	}
      if(BUTTON_CLICK(_ev.bstate, i))
        {
	j = 1;
	break;
	}
      if(BUTTON_DOUBLE_CLICK(_ev.bstate, i))
        {
	j = 2;
	break;
	}
      if(BUTTON_TRIPLE_CLICK(_ev.bstate, i))
        {
	j = 3;
	break;
	}
      }
    ev = new XMouseEvent;
    ((XMouseEvent *)ev)->Position.x = _ev.x + 1;
    ((XMouseEvent *)ev)->Position.y = _ev.y + 1;
    ((XMouseEvent *)ev)->Button = i;
    if(_clicks > 0)
      {
      ev->Type = EV_MOUSEUP;
      ((XMouseEvent *)ev)->Clicks = _clicks;
      _PutEvent(ev);
      ev = new XMouseEvent;
      ((XMouseEvent *)ev)->Position.x = _ev.x + 1;
      ((XMouseEvent *)ev)->Position.y = _ev.y + 1;
      ((XMouseEvent *)ev)->Button = i;
      ev->Type = EV_MOUSEDOWN;
      }
     else
      {
      _clicks = 1;
      if(_press)
	ev->Type = EV_MOUSEDOWN;
       else
	ev->Type = EV_MOUSEUP;
      }
    ((XMouseEvent *)ev)->Clicks = _clicks;
    return ev;
    }
#endif // NCURSES_MOUSE_VERSION
  ev = new XKeyboardEvent;
  ev->Type = EV_KEYDOWN;
#ifdef HAVE_LIBGPM
  if(GetState(TM_HAVE_GPM_MOUSE))
    {
    ungetch(ch);
    ch = _GetTtyKey();
    }
   else
#endif
    if(ch == 27)
      {
      ch = getch();
      if(ch == ERR)
        ((XKeyboardEvent *)ev)->KeyCode = 27;
       else
        ((XKeyboardEvent *)ev)->KeyCode = 32768 + ch;
      if(((XKeyboardEvent *)ev)->KeyCode == TERMINAL_REFRESH_KEY)
        Dump();
      return ev;
      }
  ((XKeyboardEvent *)ev)->KeyCode = ch;

  return ev;
}

unsigned XTerminal::GetInputState(unsigned long __Mask)
{
  return(InputState & __Mask);
}

int XTerminal::GetInputTimeout()
{
  return InputTimeout;
}

char *XTerminal::GetName()
{
  return Name;
}

unsigned XTerminal::GetPalEntry(char *__Pal, unsigned __Len, unsigned __Index)
{
  if(!XGetOutputState(TO_HAVE_COLORS))
    return __Pal[__Len + __Index - 1];
  return __Pal[__Index - 1];
}

unsigned XTerminal::GetOutputState(unsigned long __Mask)
{
  return(OutputState & __Mask);
}

XPoint XTerminal::GetSize()
{
  return Size;
}

bool XTerminal::GetState(unsigned long __Mask)
{
  return(State & __Mask);
}

void XTerminal::GetWindow(XRect __r, chtype *__buff)
{
  unsigned t = 0;

  if(!IsValid(__r))
    {
#ifdef DEBUG
    XDebug("XTerminal::GetWindow(): (%d, %d, %d, %d) invalid coordinates",
      __r.a.x, __r.a.y, __r.b.x, __r.b.y);
#endif
    return;
    }
  for(unsigned j = __r.a.y; j <= __r.b.y; j++)
    for(unsigned i = __r.a.x; i <= __r.b.x; i++)
#if defined(HAVE_NCURSES) && defined(USE_NCURSES_HACKS)
#ifdef NCURSES_VERSION
      __buff[t++] = stdscr->_line[j - 1].text[i - 1];
#else
      __buff[t++] = stdscr->_line[j - 1][i - 1];
#endif
#else
      __buff[t++] = mvwinch(stdscr, j - 1, i - 1);
#endif
}

void XTerminal::Init()
{
  int i, j;
  char s[14];

#ifdef HAVE_LIBGPM
  Gpm_Connect connect;
  int _XtMouseHandler(Gpm_Event *__Ev, void *__Data);
#endif

  InputState = OutputState = State = 0;

// initialize the curses library
  initscr();
  start_color();
  cbreak();
  noecho();
#ifdef HAVE_NCURSES
  strcpy(s, termname());
#else
  strcpy(s, getenv("TERM"));
#endif // HAVE_NCURSES
  Name = new char[strlen(s) + 1];
  strcpy(Name, s);

// initialize colors
  for(i = 0; i < 8; i++)
    for(j = 0; j < 8; j++)
      init_pair(i * 8 + j + 1, j, i);

// clear the terminal's content at startup
  refresh();

// setup initial terminal state
  SetInputState(TI_BLOCKING_INPUT, false);
  SetInputState(TI_USE_FUNCTION_KEYS, true);
  SetInputState(TI_INSERT_MODE, true);
  SetOutputState(TO_CURSOR_VISIBLE, false);
  SetOutputState(TO_HAVE_COLORS, has_colors());
  if(strcmp(Name, "linux") == 0)
    SetState(TM_IS_PC_CONSOLE, true);
#if defined(SIGWINCH) && defined(TIOCGWINSZ) && defined(NCURSES_VERSION)
  SetState(TM_CAN_RESIZE, true);
#endif
  Cursor = XPoint(1, 1);
  Size = XPoint(COLS, LINES);
  ForegroundColor = COLOR_WHITE;
  BackgroundColor = COLOR_BLACK;
  EscTimeout = ESC_TIMEOUT;
  ESCDELAY = EscTimeout;
#ifdef HAVE_SLANG
  InputTimeout = stdscr->delay_off;
#else
  InputTimeout = stdscr->_delay;
#endif

// initialize GPM mouse
#ifdef HAVE_LIBGPM
  connect.eventMask = ~0;
  connect.defaultMask = 0;
  connect.maxMod = ~0;
  connect.minMod = 0;
  gpm_zerobased = 0;
  gpm_visiblepointer = 1;
  FD_ZERO(&_GpmFdSet);
  FD_SET(STDIN_FILENO, &_GpmFdSet);
  if((i = Gpm_Open(&connect, 0)) >= 0)
    {
    SetState(TM_HAVE_MOUSE, true);
    SetState(TM_HAVE_GPM_MOUSE, true);
    gpm_handler = _XtMouseHandler;
    FD_SET(gpm_fd, &_GpmFdSet);
    }
#endif
#ifdef NCURSES_MOUSE_VERSION
  if(!GetState(TM_HAVE_GPM_MOUSE))
    if(mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, 0))
      SetState(TM_HAVE_MOUSE, true);
#endif
}

bool XTerminal::IsValid(XPoint __p)
{
  return (__p.x > 0 && __p.y > 0) && (__p.x <= Size.x && __p.y <= Size.y);
}

bool XTerminal::IsValid(XRect __r)
{
  return __r.IsValid() && IsValid(__r.a) && IsValid(__r.b);
}

void XTerminal::MoveCursor(XPoint __p)
{
  if(!IsValid(__p))
    {
#ifdef DEBUG
    XDebug("XTerminal::MoveCursor(): (%d, %d) invalid coordinates",
      __p.x, __p.y);
#endif
    return;
    }
  Cursor = __p;
  move(__p.y - 1, __p.x - 1);
}

void XTerminal::Print(XPoint __p, char *__fmt, ...)
{
  char s[255];
  va_list argptr;
  va_start(argptr, __fmt);
  vsprintf(s, __fmt, argptr);
  va_end(argptr);
  if(!IsValid(__p))
    {
#ifdef DEBUG
    XDebug("XTerminal::Print(): (%d, %d) invalid coordinates", __p.x, __p.y);
#endif
    return;
    }
  mvprintw(__p.y - 1, __p.x - 1, s);
}

void XTerminal::PrintChar(XPoint __p, chtype __c)
{
  if(!IsValid(__p))
    {
#ifdef DEBUG
    XDebug("XTerminal::PrintChar(): (%d, %d) invalid coordinates",
      __p.x, __p.y);
#endif
    return;
    }
  mvaddch(__p.y - 1, __p.x - 1, __c);
}

void XTerminal::PutWindow(XRect __r, chtype *__buff)
{
  unsigned t = 0;
  if(!IsValid(__r))
    {
#ifdef DEBUG
    XDebug("XTerminal::PutWindow(): (%d, %d, %d, %d) invalid coordinates",
      __r.a.x, __r.a.y, __r.b.x, __r.b.y);
#endif
    return;
    }
  for(unsigned j = __r.a.y; j <= __r.b.y; j++)
    {
    for(unsigned i = __r.a.x; i <= __r.b.x; i++)
#if defined(HAVE_NCURSES) && defined(USE_NCURSES_HACKS)
      {
// ... the following stuff just because ncurses' mvaddch() doesn't work :(
#ifdef NCURSES_VERSION
// ugly, but _line structure has been changed from ncurses 1.8.6 in 1.9.4
      stdscr->_line[j - 1].text[i - 1] = __buff[t++];
#else
      stdscr->_line[j - 1][i - 1] = __buff[t++];
#endif
      }
#else
      mvaddch(j - 1, i - 1, __buff[t++]);
#endif
#if defined(HAVE_NCURSES) && defined(USE_NCURSES_HACKS)
#ifdef NCURSES_VERSION
    stdscr->_line[j - 1].firstchar = __r.a.x - 1;
    stdscr->_line[j - 1].lastchar = __r.b.x - 1;
#else
    stdscr->_firstchar[j - 1] = __r.a.x - 1;
    stdscr->_lastchar[j - 1] = __r.b.x - 1;
#endif
#endif
    }
}

void XTerminal::Refresh()
{
  refresh();
}

void XTerminal::Resize()
{
  Size = XPoint(COLS, LINES);
#ifdef HAVE_LIBGPM
  if(GetState(TM_HAVE_GPM_MOUSE))
    {
    // tell GPM we've been resized
    gpm_mx = Size.x;
    gpm_my = Size.y;
    }
#endif
}

void XTerminal::Resume()
{
  Init();
  SetState(_State, 1);
  SetInputState(_InputState, true);
  SetOutputState(_OutputState, true);
  MoveCursor(_Cursor);
  SetColor(_ForegroundColor, _BackgroundColor);
  PutWindow(XRect(1, 1, XGetSize().x, XGetSize().y), _buffer);
  Refresh();
  delete _buffer;
}

void XTerminal::SetColor(unsigned __f, unsigned __b)
{
  unsigned long attr = 0;
  ForegroundColor = __f & 7;
  BackgroundColor = __b;
  if(__f & TA_UNDERLINE)
    attr |= A_UNDERLINE;
  if(__f & TA_REVERSE)
    attr |= A_REVERSE;
  if(__f & TA_BLINK)
    attr |= A_BLINK;
  if(__f & TA_BOLD)
    attr |= A_BOLD;
  if(__f & TA_HALF_BRIGHT)
    attr |= A_DIM;
  attrset(COLOR_PAIR(BackgroundColor * 8 + ForegroundColor + 1) | attr);
}

void XTerminal::SetInputState(unsigned long __Mask, bool __BoolValue)
{
  InputState = (__BoolValue)? InputState | __Mask : InputState & (~__Mask);
  if(__Mask & TI_BLOCKING_INPUT)
    nodelay(stdscr, !__BoolValue);
  if(__Mask & TI_USE_FUNCTION_KEYS)
    keypad(stdscr, __BoolValue);
  if(__Mask & TI_INSERT_MODE)
    {
    SetOutputState(TO_CURSOR_VISIBLE, true);
    if(__BoolValue)
      SetOutputState(TO_CURSOR_VERY_VISIBLE, false);
     else
      SetOutputState(TO_CURSOR_VERY_VISIBLE, true);
    }
}

void XTerminal::SetOutputState(unsigned long __Mask, bool __BoolValue)
{
  OutputState = (__BoolValue)? OutputState | __Mask : OutputState & (~__Mask);
  if(__Mask & TO_CURSOR_VERY_VISIBLE)
    if(OutputState & TO_CURSOR_VISIBLE)
      if(__BoolValue)
        curs_set(2);
       else
        curs_set(1);
  if(__Mask & TO_CURSOR_VISIBLE)
    curs_set(__BoolValue);
}

void XTerminal::SetState(unsigned long __Mask, bool __BoolValue)
{
  State = (__BoolValue)? State | __Mask : State & (~__Mask);
}

void XTerminal::Suspend()
{
  _buffer = new chtype[XGetSize().x * XGetSize().y];
  GetWindow(XRect(1, 1, XGetSize().x, XGetSize().y), _buffer);
  _State = State;
  _InputState = InputState;
  _OutputState = OutputState;
  _Cursor = Cursor;
  _ForegroundColor = ForegroundColor;
  _BackgroundColor = BackgroundColor;
  Done();
}

XEvent *FetchEvent()
{
  XEvent *ev;
  if(!_EventQueueIsEmpty())
    ev = _GetEvent();
   else
    ev = XGetEvent();
  if(!ev || ev->Class == EV_NOTHING)
    usleep(1);       // 1 us of wait won't hurt, isn't it?
  return ev;
}

#ifdef HAVE_LIBGPM
int _XtMouseHandler(Gpm_Event *__Ev, void *__Data)
{
  XMouseEvent *ev;
  Gpm_FitEvent(__Ev);
  ev = new XMouseEvent;
  ev->Position.x = __Ev->x;
  ev->Position.y = __Ev->y;
  ev->Button = __Ev->buttons;
  ev->Clicks = __Ev->clicks;
  switch(__Ev->type & 0x0f)
    {
    case GPM_MOVE:
    case GPM_DRAG:
      ev->Type = EV_MOUSEMOVE;
      break;
    case GPM_DOWN:
      ev->Type = EV_MOUSEDOWN;
      break;
    case GPM_UP:
      ev->Type = EV_MOUSEUP;
      break;
    }
  _PutEvent(ev);
  GPM_DRAWPOINTER(__Ev);
  return 255;
}

int _GetTtyKey()
{
  int ch, key1, key2;
  ch = getch();
  if(ch == ERR)
    return ERR;
  if(ch == 13)          // seems that GPM generates 13 instead of 10 for ENTER
    ch = 10;
  if(ch == 27)
    {
    ch = getch();       // ESC
    if(ch == ERR)
      return KB_ESC;
    if(ch == 91)
      {
      ch = getch();     // ESC [
      switch(ch)
        {
        case 65:
          return KB_UP;
        case 66:
	  return KB_DOWN;
	case 67:
	  return KB_RIGHT;
	case 68:
	  return KB_LEFT;
	}
      key1 = getch();
      if(key1 < 48)
	return ERR;
      if(key1 == 126)
	switch(ch)
	  {
	  case 49:
	    return KB_HOME;
	  case 50:
	    return KB_IC;
	  case 51:
	    return KB_DC;
	  case 52:
	    return KB_END;
	  case 53:
	    return KB_PPAGE;
	  case 54:
	    return KB_NPAGE;
	  }
      if(ch == 91)
        if(key1 >= 65 && key1 <= 69)
	  return KB_F(key1 - 64);
      key2 = getch();
      if(key2 == 126)
	{
        if(ch == 49 && key1 >= 55)
	  return KB_F(6) + key1 - 55;
	if(ch == 50 && key1 <= 49)
	  return KB_F(9) + key1 - 48;
	if(ch == 50)
	  return KB_F(11) + key1 - 50;
	if(ch == 51)
	  return KB_F(17) + key1 - 49;
	}
      }
     else
      return ch + 32768;
    }
   else
    return ch;
  return ERR;
}
#endif // HAVE_LIBGPM
