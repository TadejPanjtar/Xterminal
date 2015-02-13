/*
  exfreeapp.cc

  Copyright (c) 1997-98 Dragos Acostachioaie

  This program is public domain; you can redistribute it and/or
  modify without restrictions. This program is distributed in the hope that
  will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <xterminal.h>

#define bgMyPalette "\x07\x00\x07\x00"

class MyApplication : public XtApplication {
  virtual void InitMenuBar()
    {
    Menu = 0;
    };

  virtual void InitStatusLine()
    {
    StatusLine = 0;
    };
};

int main()
{
  MyApplication *app;
  XInit();
  app = new MyApplication();
  app->SetPalette(bgMyPalette, P_BACKGROUND_LEN);
  app->SetPattern(' ');
  app->Run();
  delete app;
  XDone();
}
