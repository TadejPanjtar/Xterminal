/*
  xtdesktop.cc
  
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

#include <xtdesktop.h>
#include <string.h>


XtBackground::XtBackground(XRect __r, chtype __ch) :
  XObject(__r)
{
  strcpy(ObjClass, "XtBackground");
  if(XGetState(TM_IS_PC_CONSOLE))
    Pattern = __ch;
   else
    Pattern = ' ';
  SetPalette(P_BACKGROUND, P_BACKGROUND_LEN);
}

void XtBackground::Draw()
{
  XSetColor(GetPalEntry(P_BACKGROUND_SURFACE), GetPalEntry(P_BACKGROUND_SURFACE + 1));
  XFillWindow(Bounds, Pattern);
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
}

chtype XtBackground::GetPattern()
{
    return Pattern;
}

void XtBackground::SetPattern(chtype __Pattern)
{
  Pattern = __Pattern;
  if(GetState(SM_VISIBLE))
    Draw();
}

XtDesktop::XtDesktop(XRect __r) :
  XtBackground(__r, ACS_BOARD)
{
  strcpy(ObjClass, "XtDesktop");
}

void XtDesktop::Draw()
{
  XtBackground::Draw();
  XObject::Draw();
}
