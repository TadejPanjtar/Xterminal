/*
  exwin.cc

  Copyright (c) 1996-2002 Dragos Acostachioaie

  This program is public domain; you can redistribute it and/or
  modify without restrictions. This program is distributed in the hope that
  will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <xterminal.h>

class XtRegWin : public XtWindow {
 public:
  XtRegWin(XRect __r) : XtWindow(__r, "Registered Objects")
    {
    strcpy(ObjClass, "XtRegWin");
    SetFrame(FS_DEFAULT);
    };
  virtual void Update()
    {
    typedef multimap < unsigned long, XObject*, less < const unsigned long > > ObjectsMap;
    extern ObjectsMap _ObjectsMap;
    XObject *temp;
    ObjectsMap::iterator index = _ObjectsMap.begin();
    ObjectsMap::iterator end = _ObjectsMap.end();
    unsigned i = 2;

    if(!GetState(SM_VISIBLE))
      XtWindow::Draw();
    XColorWindow(XRect(Bounds.a.x + 1, Bounds.a.y + 1, Bounds.b.x - 1, Bounds.b.y - 1), Pal[0], Pal[1]);
    XSetColor(GetPalEntry(P_WINDOW_MARKERS), GetPalEntry(P_WINDOW_MARKERS + 1));
    XPrint(XPoint(Bounds.a.x + 2, Bounds.a.y + 1), "Id   Bounds           State ObjClass");
    XSetColor(GetPalEntry(P_WINDOW_SURFACE), GetPalEntry(P_WINDOW_SURFACE + 1));
    for(; index != end; index++)
      {
      temp = (*index).second;
      XPrint(XPoint(Bounds.a.x + 2, Bounds.a.y + i++), "%04x (%2d, %2d, %2d, %2d) %04x  %s",
        temp->GetId(),
	temp->GetBounds().a.x,
	temp->GetBounds().a.y,
	temp->GetBounds().b.x,
	temp->GetBounds().b.y,
	temp->GetState(),
	temp->GetObjClass());
      }
    XRefresh();
    };
};

int main()
{
  XtRegWin *reg;
  XtWindow *win, *win1;
  XtStaticText *text;
  XEvent *ev;

  XInit();
// so FetchEvent() will wait for key presses
  Terminal->SetInputState(TI_BLOCKING_INPUT, true);
  reg = new XtRegWin(XRect(29, 1, 80, 7));
  reg->Update();
  ev = FetchEvent();
  delete ev;
  win = new XtWindow(XRect(10, 10, 70, 20), "Test");
  win->Draw();
  reg->Update();
  ev = FetchEvent();
  delete ev;
  win1 = new XtWindow(XRect(20, 7, 60, 15), "win1");
  win1->SetState(WM_HAVE_SHADOW, true);
  win1->SetPalette(P_WINDOW_ALT, P_WINDOW_LEN);
  win1->SetFrame(FS_SINGLEFRAME);
  text = new XtStaticText(XRect(2, 1, 40, 8), AS_MIDDLE,
    "Hello, World!\n\n"
    "My name is Xterminal,\n"
    "please send your opinions to\n"
    "xterminal-users@lists.sourceforge.net");
  text->SetPalette(P_STATICTEXT_ALT, P_STATICTEXT_LEN);
  win1->RegisterClient(text);
  win1->Draw();
  reg->Update();
  ev = FetchEvent();
  delete ev;
  delete win1;
  reg->Update();
  ev = FetchEvent();
  delete ev;
  delete win;
  reg->Update();
  ev = FetchEvent();
  delete ev;
  delete reg;
  XDone();
}
