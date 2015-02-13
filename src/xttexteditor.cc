/*
  xttexteditor.cc

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

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <xttools.h>
#include <xtbutton.h>
#include <xtdialog.h>
#include <xtfield.h>
#include <xttexteditor.h>

char XtTextEditor_LastSearchText[80] = "";
unsigned long XtTextEditor_LastSearchLine;
bool XtTextEditor_SearchBackwards = false, XtTextEditor_IgnoreCase = true;

XtTextEditor::XtTextEditor(XRect __r,
			   const char *__FileName,
			   const char *__Title) :
    XtWindow(__r, __Title)
{
    strcpy(ObjClass, "XtTextEditor");
    FileName = new char[strlen(__FileName) + 1];
    strcpy(FileName, __FileName);
    ReadFile();
    TextChanged = false;
    CurrIndex = Items;
    Position = Top = XPoint(1, 1);
    Cursor = Bounds.a + XPoint(1, 1);
    InitScrollBar();
    XtWindow::SetState(WM_HAVE_SHADOW | WM_CAN_CLOSE | WM_CAN_MOVE, true);
    XObject::SetState(SM_ACTIVE, true);
}

XtTextEditor::~XtTextEditor()
{
    TItem *index = Items;

    if(HScrollBar)
	delete HScrollBar;
    if(VScrollBar)
	delete VScrollBar;
    while(Items)
	{
	index = index->Next;
	free(Items->Text);
	free(Items);
	Items = index;
	}
    delete FileName;
}

void XtTextEditor::Draw()
{
    TItem *temp = Items;
    char *s;
    unsigned long oldstate = State, i;

    XObject::SetState(SM_NO_REFRESH, true);
    XtWindow::Draw();
    XObject::SetState(SM_NO_REFRESH, oldstate & SM_NO_REFRESH);
    if(VScrollBar)
	VScrollBar->SetValue(Top.y);
    if(HScrollBar)
	HScrollBar->SetValue(Top.x);
    for(i = 1; i < Top.y; i++)
	temp = temp->Next;
    i = 1;
    while(temp && i <= Size.y - 2)
	{
	XSetColor(GetPalEntry(P_WINDOW_SURFACE), GetPalEntry(P_WINDOW_SURFACE + 1));
	if(temp->Text[0] != '\n')
	    {
	    s = new char[strlen(temp->Text) + Size.x - 2];
	    if(strlen(temp->Text) < Top.x - 1)
		strcpy(s, " ");
	    else
		strcpy(s, temp->Text + Top.x - 1);
	    for(unsigned c = strlen(s); c <= Size.x - 2; c++)
		s[c] = ' ';
	    s[Size.x - 2] = '\0';
	    for(unsigned c = 0; c < Size.x - 2; c++)
		XPrintChar(XPoint(Bounds.a.x + c + 1, Bounds.a.y + i), s[c]);
	    delete s;
	    }
	i++;
	temp = temp->Next;
	}
    UpdateIndicators();
    if(GetState(SM_FOCUSED))
	XMoveCursor(Cursor);
    if(!GetState(SM_NO_REFRESH))
	XRefresh();
}

void XtTextEditor::InitScrollBar()
{
    VScrollBar = new XtScrollBar(XRect(Size.x - 1, 1, Size.x - 1, Size.y - 2),
	LinesNo, SB_INDICATE_PERCENT);
    RegisterClient(VScrollBar);
    AddListener(VScrollBar->GetId(), EV_MOUSE);
    HScrollBar = new XtScrollBar(XRect(1, Size.y - 1, Size.x - 3, Size.y - 1),
	ColsNo, SB_HORIZONTAL | SB_INDICATE_PERCENT);
    RegisterClient(HScrollBar);
}

bool XtTextEditor::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
    XKeyboardEvent *ev;
    TItem *temp;
    unsigned length;

    if(XtWindow::ProcessKeyboardEvent(__Event))
	return true;

    switch(__Event->KeyCode)
	{
	case KB_IC:
	    XSetInputState(TI_INSERT_MODE, !XGetInputState(TI_INSERT_MODE));
	    XRefresh();
	    return true;
	case KB_DOWN:
	    if(Position.y < LinesNo)
		{
		CurrIndex = CurrIndex->Next;
		Position.y++;
		length = strlen(CurrIndex->Text);
		if(Position.x > length)
		    {
		    Position.x = length + 1;
		    if(length > Size.x - 3)
			{
			Cursor.x = Bounds.b.x - 1;
			Top.x = length - (Size.x - 4);
			}
		    else
			{
			Cursor.x = Bounds.a.x + Position.x;
			Top.x = 1;
			}
		    if(Cursor.y == Bounds.b.y - 1)
			Top.y++;
		    else
			Cursor.y++;
		    Draw();
		    }
		else
		    if(Cursor.y == Bounds.b.y - 1)
			{
			Top.y++;
			Draw();
			}
		    else
			{
			Cursor.y++;
			UpdateIndicators();
			XMoveCursor(Cursor);
			XRefresh();
			}
		return true;
		}
	    break;
	case KB_UP:
	    if(Position.y > 1)
		{
		CurrIndex = Items;
		Position.y--;
		for(unsigned i = 1; i < Position.y; i++)
		    CurrIndex = CurrIndex->Next;
		length = strlen(CurrIndex->Text);
		if(Position.x > length)
		    {
		    Position.x = length + 1;
		    if(length > Size.x - 3)
			{
			Cursor.x = Bounds.b.x - 1;
			Top.x = length - (Size.x - 4);
			}
		    else
			{
			Cursor.x = Bounds.a.x + Position.x;
			Top.x = 1;
			}
		    if(Cursor.y == Bounds.b.y - 1)
			Top.y--;
		    else
			Cursor.y--;
		    Draw();
		    }
		else
		    if(Cursor.y == Bounds.a.y + 1)
			{
			Top.y--;
			Draw();
			}
		    else
			{
			UpdateIndicators();
			Cursor.y--;
			XMoveCursor(Cursor);
			XRefresh();
			}
		return true;
		}
	    break;
	case KB_LEFT:
	    if(Position.x == 1)
		{
		if(Position.y == 1)
		    return false;
		CurrIndex = Items;
		Position.y--;
		for(unsigned i = 1; i < Position.y; i++)
		    CurrIndex = CurrIndex->Next;
		Position.x = strlen(CurrIndex->Text) + 1;
		if(strlen(CurrIndex->Text) > Size.x - 3)
		    {
		    Cursor.x = Bounds.b.x - 1;
		    Top.x = strlen(CurrIndex->Text) - (Size.x - 4);
		    Draw();
		    }
		else
		    Cursor.x = Bounds.a.x + Position.x;
		if(Cursor.y == Bounds.a.y + 1)
		    {
		    Top.y--;
		    Draw();
		    }
		else
		    {
		    Cursor.y--;
		    UpdateIndicators();
		    XMoveCursor(Cursor);
		    XRefresh();
		    }
		}
	    else
		{
		Position.x--;
		Cursor.x--;
		if(Top.x > 1)
		    {
		    Top.x--;
		    Draw();
		    }
		else
		    {
		    UpdateIndicators();
		    XMoveCursor(Cursor);
		    XRefresh();
		    }
		return true;
		}
	    break;
	case KB_RIGHT:
	    if(Position.x == strlen(CurrIndex->Text) + 1)
		{
		if(Position.y == LinesNo)
		    return false;
		CurrIndex = CurrIndex->Next;
		Position.x = 1;
		Position.y++;
		Top.x = 1;
		if(Cursor.y == Bounds.b.y - 1)
		    {
		    Cursor.x = Bounds.a.x + 1;
		    Top.y++;
		    Draw();
		    }
		else
		    {
		    Cursor.y++;
		    if(Cursor.x == Bounds.b.x - 1)
			{
			Cursor.x = Bounds.a.x + 1;
			Draw();
			}
		    else
			{
			Cursor.x = Bounds.a.x + 1;
			UpdateIndicators();
			XMoveCursor(Cursor);
			XRefresh();
			}
		    }
		}
	    else
		{
		Position.x++;
		if(Cursor.x == Bounds.b.x - 1)
		    {
		    Top.x++;
		    Draw();
		    }
		else
		    {
		    Cursor.x++;
		    UpdateIndicators();
		    XMoveCursor(Cursor);
		    XRefresh();
		    }
		}
	    break;
	case KB_HOME:
	    if(Position.x > 1)
		{
		Position.x = 1;
		Cursor.x = Bounds.a.x + 1;
		if(Top.x > 1)
		    {
		    Top.x = 1;
		    Draw();
		    }
		else
		    {
		    UpdateIndicators();
		    XMoveCursor(Cursor);
		    XRefresh();
		    }
		return true;
		}
	    break;
	case KB_END:
	    if(Position.x <= strlen(CurrIndex->Text))
		{
		Position.x = strlen(CurrIndex->Text) + 1;
		if(strlen(CurrIndex->Text) > Size.x - 3)
		    {
		    Cursor.x = Bounds.b.x - 1;
		    Top.x = strlen(CurrIndex->Text) - (Size.x - 4);
		    Draw();
		    }
		else
		    {
		    Cursor.x = Bounds.a.x + Position.x;
		    UpdateIndicators();
		    XMoveCursor(Cursor);
		    XRefresh();
		    }
		return true;
		}
	    break;
	case KB_PPAGE:
	    if(Position.y > 1)
		{
		if(Position.y < Size.y - 2)
		    {
		    Cursor.y = Bounds.a.y + 1;
		    Position.y = 1;
		    UpdateIndicators();
		    XMoveCursor(Cursor);
		    XRefresh();
		    }
		else
		    {
		    Position.y -= Size.y - 2;
		    if(Top.y > Size.y - 2)
			Top.y -= Size.y - 2;
		    else
			Top.y = 1;
		    Draw();
		    }
		CurrIndex = Items;
		for(unsigned i = 1; i < Position.y; i++)
		    CurrIndex = CurrIndex->Next;
		return true;
		}
	    break;
	case KB_NPAGE:
	    if(Position.y < LinesNo)
		{
		if(Top.y + Size.y - 2 > LinesNo)
		    {
		    for(unsigned i = 0; i < LinesNo - Position.y; i++)
			CurrIndex = CurrIndex->Next;
		    Position.y = LinesNo;
		    Cursor.y = Bounds.a.y + 1 + LinesNo - Top.y;
		    UpdateIndicators();
		    XMoveCursor(Cursor);
		    XRefresh();
		    }
		else
		    {
		    Position.y += Size.y - 2;
		    Top.y += Size.y - 2;
		    for(unsigned i = 0; i < Size.y - 2; i++)
			CurrIndex = CurrIndex->Next;
		    Draw();
		    }
		return true;
		}
	    break;
	case KB_BACK:
	case KB_BACKSPACE:
	    if(Position.x > 1)
		{
		TextChanged = true;
		length = strlen(CurrIndex->Text);
		for(unsigned i = Position.x - 1; i <= length - 1; i++)
		    CurrIndex->Text[i - 1] = CurrIndex->Text[i];
		CurrIndex->Text[length - 1] = '\0';
		CurrIndex->Text = (char *)realloc(CurrIndex->Text, length + 1);
		Position.x--;
		if(LinesNo == length)
		    {
		    LinesNo = length - 1;
		    if(HScrollBar)
			HScrollBar->SetTotalNo(LinesNo);
		    }
		if(Cursor.x > 1)
		    Cursor.x--;
		Draw();
		return true;
		}
	    break;
	case KB_DC:
	    length = strlen(CurrIndex->Text);
	    if(length >= 1)
		{
		TextChanged = true;
		for(unsigned i = Position.x; i <= length - 1; i++)
		    CurrIndex->Text[i - 1] = CurrIndex->Text[i];
		CurrIndex->Text[length - 1] = '\0';
		CurrIndex->Text = (char *)realloc(CurrIndex->Text, length + 1);
		if(LinesNo == length)
		    {
		    LinesNo = length - 1;
		    if(HScrollBar)
			HScrollBar->SetTotalNo(LinesNo);
		    }
		Draw();
		return true;
		}
	    break;
	case KB_ENTER:
	    TextChanged = true;
	    length = strlen(CurrIndex->Text);
	    temp = CurrIndex->Next;
	    CurrIndex->Next = new TItem;
	    CurrIndex->Next->Text = (char *)malloc(length - Position.x + 1);
	    strcpy(CurrIndex->Next->Text, CurrIndex->Text + Position.x - 1);
	    CurrIndex->Text[Position.x - 1] = '\0';
	    CurrIndex->Next->Next = temp;
	    CurrIndex = CurrIndex->Next;
	    LinesNo++;
	    if(VScrollBar)
		VScrollBar->SetTotalNo(LinesNo);
	    Position.x = 1;
	    Position.y++;
	    Cursor.x = Bounds.a.x + 1;
	    if(Cursor.y == Bounds.b.y - 1)
		Top.y++;
	    else
		Cursor.y++;
	    Draw();
	    return true;
	case KB_ESC:
	    if(TextChanged)
		{
		XSetOutputState(TO_CURSOR_VISIBLE, false);
		if(!SaveDialog())
		    {
		    XMoveCursor(Cursor);
		    XSetOutputState(TO_CURSOR_VISIBLE, true);
		    return true;
		    }
		}
	    ev = new XKeyboardEvent;
	    ev->KeyCode = WINDOW_CLOSE_KEY;
	    XtWindow::ProcessKeyboardEvent(ev);
	    delete ev;
	    return true;
	default:
	    unsigned key = __Event->KeyCode;
	    if(key >= 0x20 && key <= 0xff)
		{
		TextChanged = true;
		length = strlen(CurrIndex->Text);
		if(Position.x <= length)
		    if(XGetInputState(TI_INSERT_MODE))
			{
			CurrIndex->Text = (char *)realloc(CurrIndex->Text, length + 2);
			for(unsigned i = length; i >= Position.x; i--)
			    CurrIndex->Text[i] = CurrIndex->Text[i - 1];
			CurrIndex->Text[Position.x - 1] = key;
			CurrIndex->Text[length + 1] = '\0';
			}
		    else
			CurrIndex->Text[Position.x - 1] = key;
		else
		    {
		    CurrIndex->Text = (char *)realloc(CurrIndex->Text, length + 2);
		    CurrIndex->Text[length] = key;
		    CurrIndex->Text[length + 1] = '\0';
		    }
		Position.x++;
		if(ColsNo < length + 1)
		    {
		    ColsNo = length + 1;
		    if(HScrollBar)
			HScrollBar->SetTotalNo(ColsNo);
		    }
		if(Cursor.x == Bounds.b.x - 1)
		    Top.x++;
		else
		    Cursor.x++;
		Draw();
		return true;
		}
	    break;
	}
    return false;
}

bool XtTextEditor::ProcessMessage(XMessage *__Event)
{
    XKeyboardEvent *ev;

    switch(__Event->Message)
	{
	case MT_RELEASE_FOCUS:
	    XSetOutputState(TO_CURSOR_VISIBLE, GetState(SM_CURSOR_VISIBLE));
	    SetState(SM_FOCUSED, false);
	    return true;
	case MT_RESUME_FOCUS:
	    XObject::SetState(SM_CURSOR_VISIBLE, XGetOutputState(TO_CURSOR_VISIBLE));
	    if(XGetInputState(TI_INSERT_MODE))
		{
		XSetOutputState(TO_CURSOR_VISIBLE, true);
		XSetOutputState(TO_CURSOR_VERY_VISIBLE, false);
		}
	    else
		{
		XSetOutputState(TO_CURSOR_VISIBLE, true);
		XSetOutputState(TO_CURSOR_VERY_VISIBLE, true);
		}
	    SetState(SM_FOCUSED, true);
	    return true;
	case SCROLLBAR_ITEMUP:
	    ev = new XKeyboardEvent;
	    ev->KeyCode = KB_UP;
	    ProcessKeyboardEvent(ev);
	    delete ev;
	    return true;
	case SCROLLBAR_ITEMDOWN:
	    ev = new XKeyboardEvent;
	    ev->KeyCode = KB_DOWN;
	    ProcessKeyboardEvent(ev);
	    delete ev;
	    return true;
	}
    return XtWindow::ProcessMessage(__Event);
}

int XtTextEditor::ReadFile()
{
    FILE *f;
    struct stat st;
    TItem *temp, *index;
    char *buff, *ptr;
    unsigned long i;

    Items = 0;
    LinesNo = 0;
    ColsNo = 0;
    if((f = fopen(FileName, "r")) == 0)
	return -1;
    fstat(fileno(f), &st);
    buff = new char[st.st_size + 1];
    fread(buff, 1, st.st_size, f);
    fclose(f);
    if(st.st_size == 0)
	return 0;
    i = 0;
    for(ptr = buff; ptr - buff <= st.st_size; ptr++)
	{
	if(*ptr == '\n' || *(ptr + 1) == '\0')
	    {
	    temp = new TItem;
	    temp->Text = (char *)malloc(i + 1);
	    strncpy(temp->Text, ptr - i, i);
	    temp->Text[i] = '\0';
	    if(temp->Text[0] == '\n')
		temp->Text[0] = '\0';
	    for(unsigned c = 0; c < strlen(temp->Text); c++)
		if(temp->Text[c] == 0x09)
		    temp->Text[c] = ' ';
	    if(strlen(temp->Text) > ColsNo)
		ColsNo = strlen(temp->Text);
	    temp->Next = 0;
	    if(Items == 0)
		Items = temp;
	    else
		{
		index = Items;
		while(index->Next)
		    index = index->Next;
		index->Next = temp;
		}
	    if(*(ptr + 1) != '\n')
		ptr++;
	    LinesNo++;
	    i = 0;
	    }
	i++;
	}
    delete buff;

    return 1;
}

int XtTextEditor::WriteFile()
{
    FILE *f;
    TItem *index = Items;

    if((f = fopen(FileName, "w")) == 0)
	return -1;
    while(index)
	{
	fprintf(f, "%s\n", index->Text);
	index = index->Next;
	}
    fclose(f);

    return 1;
}

int XtTextEditor::Run()
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
	if(ev && (ev->Class == EV_COMMAND && ((XMessage *)ev)->Message == CM_CLOSE))
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

bool XtTextEditor::SaveDialog()
{
    class XtMyDialogBox : public XtDialogBox {
    public:
	XtMyDialogBox(XRect __r, const char *__Title) : XtDialogBox(__r, __Title)
	    {
	    };
	virtual int Run()
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
		if(ev && ev->Class == EV_COMMAND)
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
	    };
    };

    XtMyDialogBox *dlg;
    XtButton *butt_cancel, *butt_yes, *butt_no;
    int result;

    dlg = new XtMyDialogBox(XRect(20, 10, 60, 17), "Quit");
    dlg->RegisterClient(new XtStaticText(XRect(4, 2, 37, 2), AS_CENTER,
	"Text was modified, save and exit?"));
    butt_cancel = new XtButton(XRect(4, 5, 13, 5), "~C~ancel", BF_ARROW, CM_CLOSE);
    dlg->RegisterClient(butt_cancel);
    butt_yes = new XtButton(XRect(18, 5, 24, 5), "~Y~es", BF_ARROW, 0x200);
    dlg->RegisterClient(butt_yes);
    butt_no = new XtButton(XRect(29, 5, 34, 5), "~N~o", BF_ARROW, 0x201);
    dlg->RegisterClient(butt_no);
    result = dlg->Run();
    delete dlg;
    if(result == CM_CLOSE)
	return false;
    if(result == 0x200)
	WriteFile();
    return true;
}

void XtTextEditor::SearchDialog(bool __cont)
{
/*
    XtDialogBox *dialog;
    XtInputStringField *data;
    XtCheckButton *options;
    XtButton *butt1, *butt2;
    XtMessageBox *box;
    TItem *index = Items;
    char text[80], *s = new char[ColsNo + 1];
    unsigned long i, j, k, found = 0;

    if(!__cont)
	{
	dialog = new XtDialogBox(XRect(10, 8, 70, 18), "Search");
	data = new XtInputStringField(XRect(2, 2, 50, 2), last_search_text, 80, 0);
	dialog->RegisterClient(data);
	options = new XtCheckButton(XRect(2, 4, 35, 5),
	    NewItem("Search backwards", search_backwards,
	    NewItem("Ignore case", ignore_case,
	0)));
	dialog->RegisterClient(options);
	butt1 = new XtButton(XRect(21, 8, 26, 8), "~O~k", BF_ARROW, CM_OK);
	dialog->RegisterClient(butt1);
	butt2 = new XtButton(XRect(31, 8, 41, 8), "~C~ancel", BF_ARROW, CM_CLOSE);
	dialog->RegisterClient(butt2);
	if(dialog->Run() == CM_CLOSE)
	    {
	    delete dialog;
	    return;
	    }
	strcpy(text, data->GetValue());
	strcpy(last_search_text, text);
	search_backwards = options->GetValue(1);
	ignore_case = options->GetValue(2);
	delete dialog;
	last_search_line = CP;
	}
    else
	strcpy(text, last_search_text);
	i = last_search_line;
	for(j = 0; j < i; j++)
	    index = index->Next;
	while(!found && i < LinesNo)
	    {
	    strcpy(s, index->Text);
	    for(k = 0; k < strlen(s); k++)
		s[k] = toupper(s[k]);
	    for(k = 0; k < strlen(text); k++)
		text[k] = toupper(text[k]);
	    if(strstr(s, text))
		{
		found = i + 1;
		break;
		}
	    if(search_backwards)
		{
		i--;
		index = Items;
		for(j = 0; j < i; j++)
		    index = index->Next;
		}
	    else
		{
		i++;
		index = index->Next;
		}
	    }
    if(found)
	{
	last_search_line = CP = found;
	if(LinesNo < Size.y - 2)
	    CP = 1;
	else
	    if(CP + Size.y - 2 > LinesNo)
	CP = LinesNo - (Size.y - 3);
	Draw();
	}
    else
	{
	sprintf(s, "\"%s\"\nnot found.", text);
	box = new XtMessageBox(XRect(20, 8, 60, 16), "Search", s, OK_BUTTON);
	box->Run();
	delete box;
	}
*/
}

void XtTextEditor::SetState(unsigned long __Mask, bool __BoolValue)
{
    XObject::SetState(__Mask, __BoolValue);
    if(GetState(SM_VISIBLE))
	Draw();
}

void XtTextEditor::UpdateIndicators()
{
    XSetColor(Pal[P_WINDOW_SURFACE - 1], Pal[P_WINDOW_SURFACE]);
    XPrint(XPoint(Bounds.b.x - 10, Bounds.a.y), " %3d:%3d ",  Position.x, Position.y);
    if(TextChanged)
	XPrint(XPoint(Bounds.b.x - 12, Bounds.a.y), "*");
}
