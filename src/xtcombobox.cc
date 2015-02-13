/*
  xtcombobox.cc

  Copyright (c) 1999-2000 Pavel Andreew

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

#include <ctype.h>
#include <string.h>

//#include <RegionalInfo.h>
#include <xtcombobox.h>


XtComboBox::XtComboBox(XRect __r, LItem *__Items, short __Rows) :
    XtInputStringField((__r -= XPoint(1u,0u)),
		       (__Items)? __Items->Text : (char *)"",
		       (__r.b - __r.a).x, FIELD_READONLY)
{
    strcpy(ObjClass, "XtComboBox");
    DropList = new XtList( XRect(XPoint(0, 1),
				 __r.b - __r.a + XPoint(1, __Rows + 1)),
			   LIST_V_SCROLLBAR, __Items);
    DropList->SetState(SM_VISIBLE, false);
    DropList->SetState(SM_DISABLED, true);
    RegisterClient(DropList);
    Template = new char[MaxLen + 1];
    bzero(Template, MaxLen + 1);
    ItemsNo = DropList->GetItemsNumber();
    CurrentItem = (ItemsNo > 0)? 1 : 0;
}

XtComboBox::~XtComboBox()
{
    delete Template;
    delete DropList;
}

void XtComboBox::AddFirst(const char *__Text, long __Value, short __Flags)
{
    DropList->AddFirst(__Text, __Value, __Flags);
    ItemsNo = DropList->GetItemsNumber();
    SetCurrentItem(1);
}

void XtComboBox::AddFirst(const char *__Text)
{
    AddFirst(__Text, 0, 0);
}

void XtComboBox::AddLast(const char *__Text, long __Value, short __Flags)
{
    DropList->AddLast(__Text, __Value, __Flags);
    ItemsNo = DropList->GetItemsNumber();
    SetCurrentItem(1);
}

void XtComboBox::AddLast(const char *__Text)
{
    AddLast(__Text, 0, 0);
}

bool XtComboBox::DeleteItem(unsigned long __Item)
{
    bool ret;

    ret = DropList->DeleteItem(__Item);
    ItemsNo = DropList->GetItemsNumber();
    if (ret) SetCurrentItem(1);
    return ret;
}

void XtComboBox::Draw()
{
    XSetColor(GetPalEntry(P_FIELD_DEFAULT), GetPalEntry(P_FIELD_DEFAULT + 1));
    XPrintChar(XPoint(Bounds.b.x + 1, Bounds.b.y), ACS_DARROW);
    XtInputStringField::Draw();
}

bool XtComboBox::InsertItem(unsigned long __Position, LItem *__Item)
{
    bool ret;

    ret = DropList->InsertItem(__Position, __Item);
    ItemsNo = DropList->GetItemsNumber();
    if (ret) SetCurrentItem(1);
    return ret;
}

bool XtComboBox::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
    char		ch;
    bool		oldtext = InputText;

    InputText = true;
    switch(__Event->KeyCode)
    {
	case KB_IC:
	case KB_LEFT:
	case KB_RIGHT:
	case KB_ESC:
	    return true;

	case KB_BACK:
	case KB_BACKSPACE:
	    if (CP > 1) {
		Template[strlen(Template) - 1] = '\0';
		SetCurrentItem(Template);
		CP--; curX--;
		Draw();
	    }
	    else {
        	XBeep();
        	XRefresh();
	    }
	    return true;

        case KB_DC:
	    InputText = false;
	    ClearTemplate();
	    return true;

	case KB_UP:
	    if (CurrentItem > 1)
		SetCurrentItem(--CurrentItem);
	    else
		SetCurrentItem(ItemsNo);
	    return true;

	case KB_DOWN:
	    if (CurrentItem < ItemsNo)
		SetCurrentItem(++CurrentItem);
	    else
		SetCurrentItem(1);
	    return true;

	case KB_HOME:
	    SetCurrentItem(1);
	    return true;

	case KB_END:
	    SetCurrentItem(ItemsNo);
	    return true;

	case KB_ENTER:
	    DropList->SetState(SM_VISIBLE, true);
	    DropList->SetState(SM_DISABLED, false);
	    DropList->SetCurrentItem(CurrentItem);
	    XSetOutputState(TO_CURSOR_VISIBLE, false);
	    if (DropList->Run() == CM_OK)
		SetCurrentItem(DropList->GetCurrentItem());
	    DropList->SetState(SM_VISIBLE, false);
	    DropList->SetState(SM_DISABLED, true);
	    XSetOutputState(TO_CURSOR_VISIBLE, true);
	    if (XtInputStringField::Server)
		XtInputStringField::Server->Draw();
	    XtInputStringField::Draw();
	    return true;

	default:
	    ch = __Event->KeyCode;
	    if (isprint(ch) && (!oldtext || CP <= MaxLen)) {
		if (!oldtext)
    		    bzero(Template, MaxLen);
		Template[strlen(Template)+1] = '\0';
		Template[strlen(Template)] = ch;
		SetCurrentItem(Template);
		CP++; curX++;
		Draw();
	    }
	    else {
        	XBeep();
        	XRefresh();
	    }
	    return true;
    }
    return XtInputStringField::ProcessKeyboardEvent(__Event);
}

void XtComboBox::ClearTemplate()
{
    bzero(Template, MaxLen);
    CP = curX = 1;
    if (GetState(SM_VISIBLE))
	Draw();
}

void XtComboBox::SetCurrentItem(char *__Template)
{
    char	*s1 = NULL, *s2, *ptr;
    bool	found;
    LItem	*index = DropList->GetValue();

    if (!index) return;
    s2 = xstrdup(__Template);
    for (ptr=s2; *ptr; ptr++) *ptr = tolower(*ptr);
    CurrentItem = 1;

    while (index->Next) {
	s1 = xstrdup(index->Text);
	for (ptr=s1; *ptr; ptr++) *ptr = tolower(*ptr);
	found = (xstrcoll(s1, s2) >= 0);
	delete s1;
	if (found) break;
	index = index->Next;
	CurrentItem++;
    }
    delete s2;

    strncpy(DefaultString, index->Text, MaxLen);
    DefaultString[MaxLen - 1] = '\0';	// strncpy() doesn't copy extra '\0' !
    strcpy(CurrString, DefaultString);
    StringLength = strlen(CurrString);
}

void XtComboBox::SetCurrentItem(unsigned long __Index)
{
    LItem		*temp = DropList->GetValue();
    unsigned long	i;

    if (!temp) return;
    CurrentItem = __Index;
    for (i = 1; i < __Index && temp->Next; i++)
	temp = temp->Next;
    strncpy(DefaultString, temp->Text, MaxLen);
    DefaultString[MaxLen - 1] = '\0';	// strncpy() doesn't copy extra '\0' !
    strcpy(CurrString, DefaultString);
    StringLength = strlen(CurrString);
    ClearTemplate();
}


void XtComboBox::SetKey(long __Key)
{
    LItem	*index = DropList->GetValue();

    if (!index) return;
    CurrentItem = 1;
    while (index->Value != __Key && index->Next) {
	index = index->Next;
	CurrentItem++;
    }
    strncpy(DefaultString, index->Text, MaxLen);
    DefaultString[MaxLen - 1] = '\0';	// strncpy() doesn't copy extra '\0' !
    strcpy(CurrString, DefaultString);
    StringLength = strlen(CurrString);
    ClearTemplate();
}

void XtComboBox::SetValue(char *__DefaultString)
{
    SetCurrentItem(__DefaultString);
    ClearTemplate();
}

void XtComboBox::SetValue(LItem *__Items)
{
    DropList->SetState(SM_VISIBLE, false);
    DropList->SetValue(__Items);
    ItemsNo = DropList->GetItemsNumber();
    if (DropList->GetValue())
	SetCurrentItem(1);
    else {
	CurrentItem = 0;
	strcpy(DefaultString, "");
	strcpy(CurrString, DefaultString);
	StringLength = strlen(CurrString);
	ClearTemplate();
    }
}

long XtComboBox::GetKey()
{
    return DropList->GetValue(CurrentItem);
}

/***********************************************************************\
*	strdup() wrapper for C++					*
\***********************************************************************/
char *xstrdup(const char *str)
{
    char *p;

    if (!str) return NULL;
    p = new char[strlen(str) + 1];
    if (p) strcpy(p, str);
    return p;
}

/***********************************************************************\
*    	strcoll() wrapper						*
*	I'm use it, because strcoll() doesn't work correctly		*
*	with russian locale						*
\***********************************************************************/
int xstrcoll(const char *__s1, const char *__s2)
{
    register unsigned	i, r, len;
    register char	s1[2], s2[2];

    if (strlen(__s1)==0) return -1;
    if (strlen(__s2)==0) return 1;
    len = (strlen(__s1) <= strlen(__s2))? strlen(__s1) : strlen(__s2); 

    s1[1] = s2[1] = '\0';
    for (i = 0; i < len; i++) {
	s1[0] = __s1[i]; s2[0] = __s2[i];
	if ((r = strcoll(s1, s2)) != 0)
	    return r;
    }

    if (strlen(__s1) > strlen(__s2)) return len;
    else
        if (strlen(__s1) < strlen(__s2)) return -len;
        else
	    return 0;
}

