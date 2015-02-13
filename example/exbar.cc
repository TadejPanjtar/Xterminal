/*
  exbar.cc

  Copyright (c) 1997-99 Dragos Acostachioaie

  This program is public domain; you can redistribute it and/or
  modify without restrictions. This program is distributed in the hope that
  will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <xterminal.h>

int main()
{
  XtProgressBar *progressbar;
  XtScrollBar *scrollbar;
  unsigned i;
  XEvent *ev;
  XInit();
  progressbar = new XtProgressBar(XRect(1, 1, 20, 1), 50, PB_LEFT_TO_RIGHT);
  scrollbar = new XtScrollBar(XRect(1, 5, 20, 5), 50, SB_HORIZONTAL);
  Terminal->SetInputState(TI_BLOCKING_INPUT, true);
  for(i = 0; i < 50; i++)
    {
    progressbar->SetValue(i);
    progressbar->Draw();
    scrollbar->SetValue(i);
    scrollbar->Draw();
    ev = FetchEvent();
    ClearEvent(ev);
    }
  delete progressbar;
  delete scrollbar;
  XDone();
}
