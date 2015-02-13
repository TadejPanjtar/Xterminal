/*
  xtother.cc

  Copyright (c) 1998-99 Dragos Acostachioaie

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
#include <xtother.h>

char last_search_text[80] = "";
unsigned long last_search_line;
bool search_backwards = false, ignore_case = true;

XtTextViewer::XtTextViewer(XRect __r,
			   const char *__FileName,
			   const char *__Title) :
    XtWindow(__r, __Title)
{
  strcpy(ObjClass, "XtTextViewer");
  FileName = new char[strlen(__FileName) + 1];
  strcpy(FileName, __FileName);
  ReadFile();
  CP = CC = 1;
  InitScrollBar();
  XtWindow::SetState(WM_HAVE_SHADOW | WM_CAN_CLOSE | WM_CAN_MOVE, true);
}

XtTextViewer::~XtTextViewer()
{
  TItem *index = Items;
  if(HScrollBar)
    delete HScrollBar;
  if(VScrollBar)
    delete VScrollBar;
  while(Items)
    {
    index = index->Next;
    delete Items->Text;
    delete Items;
    Items = index;
    }
  delete FileName;
}

void XtTextViewer::Draw()
{
  TItem *temp = Items;
  char *s;
  unsigned long oldstate = State, i;
  XObject::SetState(SM_NO_REFRESH, true);
  XtWindow::Draw();
  XObject::SetState(SM_NO_REFRESH, oldstate & SM_NO_REFRESH);
  if(VScrollBar)
    VScrollBar->SetValue(CP);
  if(HScrollBar)
    HScrollBar->SetValue(CC);
  for(i = 1; i < CP; i++)
    temp = temp->Next;
  i = 1;
  while(temp && i <= Size.y - 2)
    {
    XSetColor(GetPalEntry(P_WINDOW_SURFACE), GetPalEntry(P_WINDOW_SURFACE + 1));
    if(temp->Text[0] != '\n')
      {
      s = new char[strlen(temp->Text) + Size.x - 2];
      if(strlen(temp->Text) < CC - 1)
        strcpy(s, " ");
       else
        strcpy(s, temp->Text + CC - 1);
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
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
}

void XtTextViewer::InitScrollBar()
{
  VScrollBar = new XtScrollBar(XRect(Size.x - 1, 1, Size.x - 1, Size.y - 2),
    LinesNo, SB_INDICATE_PERCENT);
  RegisterClient(VScrollBar);
  AddListener(VScrollBar->GetId(), EV_MOUSE);
  HScrollBar = new XtScrollBar(XRect(1, Size.y - 1, Size.x - 3, Size.y - 1),
    ColsNo, SB_HORIZONTAL | SB_INDICATE_PERCENT);
  RegisterClient(HScrollBar);
}

bool XtTextViewer::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  XKeyboardEvent *ev;
  if(XtWindow::ProcessKeyboardEvent(__Event))
    return true;
  switch(__Event->KeyCode)
    {
    case KB_DOWN:
      if(CP + Size.y - 3 < LinesNo)
        {
	CP++;
	Draw();
	}
      return true;
    case KB_UP:
      if(CP > 1)
	{
	CP--;
	Draw();
	}
      return true;
    case KB_RIGHT:
      if((CC < ColsNo - (Size.x - 3)) && ColsNo > Size.x - 3)
	{
	CC++;
	Draw();
	}
      return true;
    case KB_LEFT:
      if(CC > 1)
	{
	CC--;
	Draw();
	}
      return true;
    case KB_NPAGE:
      if(CP + Size.y - 2 < LinesNo)
	{
	if(CP + Size.y - 2 > LinesNo - (Size.y - 2))
	  CP = LinesNo - (Size.y - 3);
	 else
	  CP += Size.y - 2;
	Draw();
	}
      return true;
    case KB_PPAGE:
      if(CP > 1)
	{
	if(CP > Size.y - 2)
	  CP -= Size.y - 2;
	 else
	  CP = 1;
	Draw();
	}
      return true;
    case KB_HOME:
      if(CP > 1)
        {
	CP = 1;
	Draw();
	}
      return true;
    case KB_END:
      if(CP + Size.y - 2 < LinesNo)
	{
	CP = LinesNo - (Size.y - 3);
	Draw();
	}
      return true;
    case KB_ESC:
      ev = new XKeyboardEvent;
      ev->KeyCode = WINDOW_CLOSE_KEY;
      XtWindow::ProcessKeyboardEvent(ev);
      delete ev;
      return true;
    case KB_META('s'):
      if(LinesNo > 0)
	SearchDialog(false);
      return true;
    case KB_META('n'):
      if(LinesNo > 0)
	SearchDialog(true);
      return true;
    }
  return false;
}

bool XtTextViewer::ProcessMessage(XMessage *__Event)
{
  XKeyboardEvent *ev;
  switch(__Event->Message)
    {
    case MT_RELEASE_FOCUS:
      SetState(SM_FOCUSED, false);
      return true;
    case MT_RESUME_FOCUS:
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

int XtTextViewer::ReadFile()
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
      temp->Text = new char[i + 1];
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

int XtTextViewer::Run()
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

void XtTextViewer::SearchDialog(bool __cont)
{
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
}
