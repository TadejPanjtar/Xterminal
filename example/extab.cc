/*
  extab.cc

  Copyright (c) 2001 Dragos Acostachioaie

  This program is public domain; you can redistribute it and/or
  modify without restrictions. This program is distributed in the hope that
  will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <stdio.h>
#include <xterminal.h>

int main()
{
    XtTab *tab1, *tab2;
    XtTabDialog *dlg;
    XtButton *button1, *button2;
    XtInputStringField *field;

    XInit();
    dlg = new XtTabDialog(XRect(10, 5, 70, 20));
    tab1 = new XtTab(XRect(10, 5, 40, 20), "tab 1");
    button1 = new XtButton(XRect(6, 8, 11, 8), "~O~k", BF_ARROW, CM_OK);
    tab1->RegisterClient(button1);
    button2 = new XtButton(XRect(16, 8, 25, 8), "~C~ancel", BF_ARROW, CM_CLOSE);
    tab1->RegisterClient(button2);
    tab2 = new XtTab(XRect(10, 5, 40, 20), "tab 2");
    field = new XtInputStringField(XRect(3, 2, 24, 2), "default", 20, 0);
    tab2->RegisterClient(field);
    dlg->RegisterClient(tab1);
    dlg->RegisterClient(tab2);
    dlg->Run();
    delete dlg;

    XDone();
}
