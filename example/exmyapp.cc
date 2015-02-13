/*
  exmyapp.cc

  Copyright (c) 1997-2001 Dragos Acostachioaie

  This program is public domain; you can redistribute it and/or
  modify without restrictions. This program is distributed in the hope that
  will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <xterminal.h>

#include <time.h>

#define CM_FILE		0x10
#define CM_FILE_NEW	0x11
#define CM_FILE_OPEN	0x12
#define CM_FILE_VIEW	0x13
#define CM_FILE_EDIT	0x14
#define CM_FILE_SUB	0x15
#define CM_FILE_SUB1	0x16
#define CM_FILE_SUB2	0x17
#define CM_HELP		0x18
#define CM_HELP_INDEX	0x19
#define CM_TEST		0x1A

void open()
{
  XtMessageBox *box;
  box = new XtMessageBox(XRect(10, 8, 70, 16), "Test", "Hello, World!", OK_BUTTON);
  box->Run();
  delete box;
}

class MyApplication : public XtApplication {
 public:
  virtual void InitMenuBar()
    {
    Menu = new XtMenu(XRect(Bounds.a.x, Bounds.a.y, Bounds.b.x, Bounds.a.y), 0,
    NewMenuItem("~F~ile", 0, KB_META('f'), CM_FILE,
      NewMenuItem("~N~ew", "", 0, CM_FILE_NEW, 0,
      NewMenuItem("~O~pen", "F2", KB_F(2), CM_FILE_OPEN, 0,
      NewMenuItem("~V~iew", "F3", KB_F(3), CM_FILE_VIEW, 0,
      NewMenuItem("~E~dit", "F4", KB_F(4), CM_FILE_EDIT, 0,
      NewLine(
      NewMenuItem("~S~ubmenu", "", 0, CM_FILE_SUB,
        NewMenuItem("Item ~1~", "", 0, CM_FILE_SUB1, 0,
        NewMenuItem("Item ~2~", "", 0, CM_FILE_SUB2, 0,
        0)),
      NewLine(
      NewMenuItem("E~x~it", "Alt-X", KB_META('x'), CM_QUIT, 0,
      0)))))))),
    NewMenuItem("~O~ptions", 0, KB_META('o'), 0,
      NewMenuItem("~C~olor", "", 0, 0, 0,
      NewMenuItem("~T~est", "", 0, 0, 0,
      0)),
    NewMenuItem("~H~elp", 0, KB_META('h'), CM_HELP,
      NewMenuItem("~I~ndex", "", 0, CM_HELP_INDEX, 0,
      0),
    NewMenuItem("~T~est", 0, KB_META('t'), CM_TEST, 0,
    0)))));

    // Disable a command
    DisableCommand(Menu->GetValue(), CM_FILE_NEW);

    // Setup a contextual help on the bottom line of the screen
    Menu->SetHelp(XRect(Bounds.a.x + 15, Bounds.b.y, Bounds.b.x - 1, Bounds.b.y),
      AS_RIGHT, 0);

    // Set some help messages
    SetContextualHelp(Menu, CM_FILE, "Files stuff");
    SetContextualHelp(Menu, CM_FILE_NEW, "Open a new window");
    SetContextualHelp(Menu, CM_FILE_OPEN, "Open a window");
    SetContextualHelp(Menu, CM_FILE_VIEW, "View `/etc/passwd'");
    SetContextualHelp(Menu, CM_FILE_EDIT, "Edit `/etc/passwd'");
    SetContextualHelp(Menu, CM_FILE_SUB, "File/Submenu stuff");
    SetContextualHelp(Menu, CM_FILE_SUB1, "Submenu1's help");
    SetContextualHelp(Menu, CM_FILE_SUB2, "Submenu2's help");
    SetContextualHelp(Menu, CM_HELP, "Help");
    SetContextualHelp(Menu, CM_HELP_INDEX, "Help index");
    SetContextualHelp(Menu, CM_TEST, "Test - some help message");
    SetContextualHelp(Menu, CM_QUIT, "Exit `exmyapp'");
    };

  virtual bool ProcessCommand(XMessage *__Event)
    {
    switch(__Event->Message)
      {
      case CM_FILE_OPEN:
        open();
        return true;
      case CM_FILE_VIEW:
	View();
        return true;
      case CM_FILE_EDIT:
	Edit();
        return true;
      }
    return XtApplication::ProcessCommand(__Event);
    };

  virtual void Idle()
    {
    time_t t;
    tm *ct;
    char *s;
    t = time(0);
    ct = localtime(&t);
    s = Menu->GetPalette();
    XSetColor(s[P_STATUSLINE_SURFACE - 1], s[P_STATUSLINE_SURFACE]);
    XPrint(XPoint(Bounds.b.x - 8, Bounds.a.y), "%02d:%02d:%02d",
      ct->tm_hour, ct->tm_min, ct->tm_sec);
    XRefresh();
    };

  void View()
    {
    view = new XtTextViewer(XRect(10, 5, 70, 20), "/etc/passwd", "XtTextViewer - /etc/passwd");
    RunClient(view);
    delete view;
    };

  void Edit()
    {
    edit = new XtTextEditor(XRect(10, 5, 70, 20), "/etc/passwd", "XtTextEditor - /etc/passwd");
    edit->Run();
    delete edit;
    };

 private:
  XtTextViewer *view;
  XtTextEditor *edit;
};

int main()
{
  MyApplication *app;
  XInit();

  // Switch region to Germany
  Region->SetRegion("deutsch");
  app = new MyApplication();
  app->Run();
  delete app;
  XDone();
}
