/*
  xttab.cc

  Copyright (c) 2001 Dragos Acostachioaie

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

#include <xtscreen.h>
#include <xttab.h>

XtTab::XtTab(XRect __r, const char *__Title) :
    XtDialogBox(__r, __Title)
{
    strcpy(ObjClass, "XtTab");
    Wnd->SetState(WM_CAN_CLOSE | WM_CAN_MOVE, false);
    Wnd->SetFrame(FS_SINGLEFRAME);
}

XtTab::~XtTab()
{
}

XtTabDialog::XtTabDialog(XRect __r) :
    XtDialog(__r)
{
    strcpy(ObjClass, "XtTabDialog");
    XObject::SetState(SM_ACTIVE, true);
}

XtTabDialog::~XtTabDialog()
{
}

void XtTabDialog::Draw()
{
    XObject::Draw();
    if(!GetState(SM_NO_REFRESH))
	XRefresh();
    XObject::SetState(SM_VISIBLE, true);
}

bool XtTabDialog::ProcessKeyboardEvent(XKeyboardEvent* __Event)
{
    switch(__Event->KeyCode)
	{
	case KB_META(KB_TAB):
	    FocusNextClient();
	    return true;
	}

    return XtDialog::ProcessKeyboardEvent(__Event);
}

int XtTabDialog::Run()
{
    XEvent *ev;
    int result;

    Draw();
    ev = new XMessage;
    ((XMessage *)ev)->Message = MT_RESUME_FOCUS;
    HandleEvent(ev);
    delete ev;
    for(;;)
	{
	ev = FetchEvent();
	HandleEvent(ev);
	if(ev && ev->Class == EV_COMMAND &&
	    (((XMessage *)ev)->Message == CM_CLOSE ||
	    ((XMessage *)ev)->Message == CM_OK))
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
