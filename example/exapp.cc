/*
  exapp.cc

  Copyright (c) 1997-98 Dragos Acostachioaie

  This program is public domain; you can redistribute it and/or
  modify without restrictions. This program is distributed in the hope that
  will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <xterminal.h>

int main()
{
  XtApplication *app;
  XInit();
  app = new XtApplication();
  app->Run();
  delete app;
  XDone();
}
