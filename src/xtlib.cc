/*
  xtlib.cc

  Copyright (c) 1997-99 Dragos Acostachioaie

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

#include <signal.h>
#include <stream.h>
#include <string>
#include <sys/ioctl.h>
#ifndef GWINSZ_IN_SYS_IOCTL
#include <termios.h>
#endif
#include <unistd.h>

#include <RegionalInfo.h>
#include <xtscreen.h>
#include <xconfig.h>
#include <xevent.h>
#include <xtlib.h>

#ifdef DEBUG
#define DEBUG_FILE	"Xterminal.debug"
FILE *_debug_file;
#endif

static RETSIGTYPE _XtSignalHandler(int __sig)
{
  XMessage *msg;
  msg = new XMessage;
  msg->Type = EV_SIGNAL;
  msg->Message = __sig;
  _PutEvent(msg);
}

static RETSIGTYPE _XtExitNow(int __sig)
{
#ifdef DEBUG
  XDebug("Killed by signal %d", __sig);
#endif
  XDone();
  exit(__sig);
}

static RETSIGTYPE _XtSaveState(int __sig)
{
  struct sigaction sa;
  Terminal->Suspend();
  sa.sa_handler = SIG_DFL;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGTSTP, &sa, 0);
  kill(getpid(), SIGTSTP);
}

static RETSIGTYPE _XtRestoreState(int __sig)
{
  struct sigaction sa;
  Terminal->Resume();
  sa.sa_handler = _XtSaveState;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGTSTP, &sa, 0);
}

#if defined(HAVE_RESIZETERM) && defined(SIGWINCH) && defined(TIOCGWINSZ)
static RETSIGTYPE _XtResizeTerminal(int __sig)
{
  struct sigaction sa;
  XMessage *msg;
  struct winsize size;
  if(ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0)
    {
    resizeterm(size.ws_row, size.ws_col);
    Terminal->Resize();
    Terminal->Refresh();
    msg = new XMessage;
    msg->Class = EV_COMMAND;
    msg->Message = CM_RESIZE;
    _PutEventInFront(msg);
    }
  sa.sa_handler = _XtResizeTerminal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGWINCH, &sa, 0);
}
#endif // defined(HAVE_RESIZETERM) && defined(SIGWINCH) && defined(TIOCGWINSZ)

void XInit()
{
  struct sigaction sa;
#ifdef DEBUG
  string s = "XInit: ";
// initialize debugging
  _debug_file = fopen(DEBUG_FILE, "a");
#endif

// initialize terminal
  Terminal = new XTerminal();

// initialize events
  InitEvents();

// initialize signals handling
#ifdef SIGNAL_CATCHING
  sa.sa_handler = _XtSignalHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  for(int i = 0; i < NSIG; i++)
    sigaction(i, &sa, 0);
#endif
  sa.sa_handler = _XtExitNow;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, 0);
  sigaction(SIGQUIT, &sa, 0);
  sigaction(SIGTERM, &sa, 0);
  sigaction(SIGSEGV, &sa, 0);
  sa.sa_handler = _XtSaveState;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGTSTP, &sa, 0);
  sa.sa_handler = _XtRestoreState;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGCONT, &sa, 0);
#if defined(HAVE_RESIZETERM) && defined(SIGWINCH) && defined(TIOCGWINSZ)
  sa.sa_handler = _XtResizeTerminal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGWINCH, &sa, 0);
#endif

#ifdef DEBUG
  s += Terminal->GetName();
  s += " ";
  s += dec(XGetSize().x);
  s += "x";
  s += dec(XGetSize().y);
  if(XGetOutputState(TO_HAVE_COLORS))
    s += " color";
   else
    s += " monochrome";
  if(XGetState(TM_HAVE_MOUSE))
    s += ", mouse support";
  XDebug("%s", s.c_str());
  XDebug("State = %x, InputState = %x, OutputState = %x", XGetState(0xffff),
    XGetInputState(0xffff), XGetOutputState(0xffff));
  s = "curses library: ";
#ifdef HAVE_NCURSES
  s += "ncurses ";
  s += NCURSES_VERSION;
#ifdef NCURSES_VERSION_PATCH
  s += " (";
  s += dec(NCURSES_VERSION_PATCH);
  s += ")";
#endif
#else
#ifdef HAVE_SLANG
  s += "slang ";
  s += dec(SLANG_VERSION);
#else
  s += "unknown";
#endif
#endif // HAVE_NCURSES
  XDebug("%s", s.c_str());
#endif // DEBUG

// initialize internationalization support
  Region = new RegionalInfo();
}

void XDone()
{
  delete Region;
  delete Terminal;
  DoneEvents();
#ifdef DEBUG
  XDebug("XDone");
  if(_debug_file)
    fclose(_debug_file);
#endif
}

void XDebug(char *__fmt, ...)
{
#ifdef DEBUG
  va_list argptr;
  va_start(argptr, __fmt);
  if(_debug_file)
    {
    vfprintf(_debug_file, __fmt, argptr);
    fprintf(_debug_file, "\n");
    fflush(_debug_file);
    }
  va_end(argptr);
#endif
}
