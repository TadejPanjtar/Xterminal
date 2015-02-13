/*
  xttools.cc

  Copyright (c) 1998 Dragos Acostachioaie

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

#include <xttools.h>
#include <xtscreen.h>

char *YES_BUTTON_MESSAGE = "~Y~es";
char *NO_BUTTON_MESSAGE	= "~N~o";
char *OK_BUTTON_MESSAGE	= "~O~K";
char *CANCEL_BUTTON_MESSAGE = "~C~ancel";

XtMessageBox::XtMessageBox(XRect __r,
			   const char *__Title,
			   const char *__Message,
			   unsigned long __ButtonFlags) :
    XtDialogBox(__r, __Title)
{
  unsigned i, len;
  strcpy(ObjClass, "XtMessageBox");
  switch(__ButtonFlags)
    {
    case YES_NO_BUTTON:
      len = strlen(YES_BUTTON_MESSAGE) + strlen(NO_BUTTON_MESSAGE) + 7;
      i = (Size.x - len) / 2;
      Button1 = new XtButton(XRect(i, Size.y - 3, i + strlen(YES_BUTTON_MESSAGE) + 1, Size.y - 3), YES_BUTTON_MESSAGE, BF_DEFAULT | BF_ARROW, CM_OK);
      RegisterClient(Button1);
      Button2 = new XtButton(XRect(i + strlen(YES_BUTTON_MESSAGE) + 6, Size.y - 3, i + len, Size.y - 3), NO_BUTTON_MESSAGE, BF_ARROW, CM_CANCEL);
      RegisterClient(Button2);
      break;
    case OK_CANCEL_BUTTON:
      len = strlen(OK_BUTTON_MESSAGE) + strlen(CANCEL_BUTTON_MESSAGE) + 7;
      i = (Size.x - len) / 2;
      Button1 = new XtButton(XRect(i, Size.y - 3, i + strlen(OK_BUTTON_MESSAGE) + 1, Size.y - 3), OK_BUTTON_MESSAGE, BF_DEFAULT | BF_ARROW, CM_OK);
      RegisterClient(Button1);
      Button2 = new XtButton(XRect(i + strlen(OK_BUTTON_MESSAGE) + 6, Size.y - 3, i + len, Size.y - 3), CANCEL_BUTTON_MESSAGE, BF_ARROW, CM_CANCEL);
      RegisterClient(Button2);
      break;
    case OK_BUTTON:
    case 0:
      i = (Size.x - strlen(OK_BUTTON_MESSAGE)) / 2;
      Button1 = new XtButton(XRect(i, Size.y - 3, i + strlen(OK_BUTTON_MESSAGE) + 1, Size.y - 3), OK_BUTTON_MESSAGE, BF_DEFAULT | BF_ARROW, CM_OK);
      RegisterClient(Button1);
      break;
    case CANCEL_BUTTON:
      i = (Size.x - strlen(CANCEL_BUTTON_MESSAGE)) / 2;
      Button1 = new XtButton(XRect(i, Size.y - 3, i + strlen(CANCEL_BUTTON_MESSAGE) + 1, Size.y - 3), CANCEL_BUTTON_MESSAGE, BF_DEFAULT | BF_ARROW, CM_OK);
      RegisterClient(Button1);
      break;
    }
  Message = new XtStaticText(XRect(0, 0, Size.x, Size.y - 2), AS_MIDDLE, __Message);
  RegisterClient(Message);
}

int XtMessageBox::Run()
{
  int result;
  XEvent *ev;
  Draw();
  ev = new XMessage;
  ((XMessage *)ev)->Message = MT_RESUME_FOCUS;
  HandleEvent(ev);
  delete ev;
  for(;;)
    {
    ev = FetchEvent();
    HandleEvent(ev);
    if(ev && ev->Class == EV_COMMAND && (((XMessage *)ev)->Message == CM_OK || ((XMessage *)ev)->Message == CM_CANCEL))
      break;
    if(ev)
      delete ev;
    }
  result = ((XMessage *)ev)->Message;
  delete ev;
  ev = new XMessage;
  ((XMessage *)ev)->Message = MT_RELEASE_FOCUS;
  HandleEvent(ev);
  delete ev;
  return result;
}
