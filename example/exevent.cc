/*
  exevent.cc

  Copyright (c) 1998 Bernd Kalbfuss

  This program is public domain; you can redistribute it and/or
  modify without restrictions. This program is distributed in the hope that
  will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <stream.h>
#include <string>

#include <xterminal.h>

class MyXtTargetWindow : public XtWindow
{
 public:
  MyXtTargetWindow(XRect __Bounds, char* __title) : XtWindow(__Bounds, __title)
    {
    SetState(WM_HAVE_SHADOW, true);
    }

  virtual bool ProcessMouseEvent(XMouseEvent *__Event)
    {
    if(!Bounds.Contains(__Event->Position))
      return true;
    return false;
    }
};

class MyXtWindow : public XtWindow
{
 public:

  unsigned type;

  MyXtWindow(XRect __Bounds, char* __title, unsigned __type)
   : XtWindow(__Bounds, __title)
    {
    SetState(WM_HAVE_SHADOW, true);
    type = __type;
    }

  virtual bool ProcessMouseEvent(XMouseEvent *__Event)
    {
    string s;
    if(__Event->Type == type && type == EV_MOUSEDOWN)
      {
      s = "mouse click at (";
      s += dec(__Event->Position.x);
      s += ", ";
      s += dec(__Event->Position.y);
      s += ")  ";
      XSetColor(GetPalEntry(P_WINDOW_SURFACE), GetPalEntry(P_WINDOW_SURFACE + 1));
      XPrint(Bounds.a + XPoint(4, 2), "%s", s.data());
      XRefresh();
      return true;
      }
    if(__Event->Type == type && type == EV_MOUSEMOVE)
      {
      s = "mouse move to (";
      s += dec(__Event->Position.x);
      s += ", ";
      s += dec(__Event->Position.y);
      s += ")  ";
      XSetColor(GetPalEntry(P_WINDOW_SURFACE), GetPalEntry(P_WINDOW_SURFACE + 1));
      XPrint(Bounds.a + XPoint(4, 2), "%s", s.data());
      XRefresh();
      return true;
      }
    return false;
    }

};

int main()
{
  XInit();
  XtApplication *app;
  MyXtWindow *win1, *win2;
  MyXtTargetWindow *win3;
  app = new XtApplication();
  win1 = new MyXtWindow(XRect(5, 4, 35, 9), "mouse clicks", EV_MOUSEDOWN);
  win2 = new MyXtWindow(XRect(5, 13, 35, 18), "mouse moves", EV_MOUSEMOVE);
  win3 = new MyXtTargetWindow(XRect(45, 6, 75, 16), "target window");
  win3->AddListener(win1->GetId(), EV_MOUSE);
  win3->AddListener(win2->GetId(), EV_MOUSE);
  app->RegisterClient(win3);
  app->AddListener(win3->GetId(), EV_MOUSE);
  app->RegisterClient(win1);
  app->RegisterClient(win2);
  app->Run();
  delete app;
  XDone();
}
