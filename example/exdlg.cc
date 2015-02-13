/*
  exdlg.cc

  Copyright (c) 1997-99 Dragos Acostachioaie

  This program is public domain; you can redistribute it and/or
  modify without restrictions. This program is distributed in the hope that
  will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <xterminal.h>

#define CM_OPEN 0x10

void open()
{
  XtDialogBox *dialog;
  XtButton *button1, *button2;
  XtCheckButton *check;
  XtRadioButton *radio;
  XtInputStringField *input_string, *input_hidden;
  XtInputNumericField *input_numeric;
  dialog = new XtDialogBox(XRect(10, 6, 70, 18), "Test");
  input_string = new XtInputStringField(XRect(3, 2, 24, 4), "default", 100, 0);
  dialog->RegisterClient(input_string);
  input_hidden = new XtInputStringField(XRect(27, 3, 44, 3), 0, 15, FIELD_HIDDEN);
  dialog->RegisterClient(input_hidden);
  input_numeric = new XtInputNumericField(XRect(47, 3, 56, 3), 520, 5, 2, FIELD_JUSTIFIED_RIGHT | ALLOW_NEGATIVE_VALUES);
  dialog->RegisterClient(input_numeric);
  check = new XtCheckButton(XRect(14, 6, 24, 7),
    NewItem("test1", 1,
    NewItem("test2", 0,
    0)));
  dialog->RegisterClient(check);
  radio = new XtRadioButton(XRect(34, 6, 45, 7),
    NewItem("item 1", 0,
    NewItem("item 2", 1,
    0)));
  dialog->RegisterClient(radio);
  button1 = new XtButton(XRect(21, 10, 26, 10), "~O~k", BF_ARROW, CM_OK);
  dialog->RegisterClient(button1);
  button2 = new XtButton(XRect(31, 10, 40, 10), "~C~ancel", BF_ARROW, CM_CLOSE);
  dialog->RegisterClient(button2);
  dialog->Run();
  delete dialog;
}

class MyApplication : public XtApplication {
  virtual void InitStatusLine()
    {
    StatusLine = new XtStatusLine(XRect(Bounds.a.x, Bounds.b.y, Bounds.b.x, Bounds.b.y), 0,
      NewStatusItem("~Alt-X~ Exit", KB_META('x'), CM_QUIT,
      NewStatusItem("~F1~ Test", KB_F(1), CM_OPEN,
      0)));
    };
    
  virtual bool ProcessCommand(XMessage *__Event)
    {
    switch(__Event->Message)
      {
      case CM_OPEN:
        open();
        return 1;
      }
    return 0;
    };
};

int main()
{
  MyApplication *app;
  XInit();
  app = new MyApplication();
  app->Run();
  delete app;
  XDone();
}
