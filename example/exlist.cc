/*
  exlist.cc

  Copyright (c) 1997-2000 Dragos Acostachioaie
  Copyright (c) 2000 Udo Kreckel

  This program is public domain; you can redistribute it and/or
  modify without restrictions. This program is distributed in the hope that
  will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <stdio.h>
#include <xterminal.h>

int main()
{
    XtList *list;
    int result, result1;

    XInit();
    XtWindow *win;
    win = new XtWindow(XRect(15, 10, 65, 17), "List Box Example");
    win->SetPalette(P_WINDOW_ALT, P_WINDOW_LEN);
    win->SetFrame(FS_SINGLEFRAME);
    list = new XtList(XRect(01, 01, 50, 7),
	SELECTABLE_ITEMS | CIRCULAR_SELECTION | LIST_V_SCROLLBAR | MAX_SELECTED,
	NewLItem("item 1 - this is the first item in list and it's selected", 0, 0,
	NewLItem("item 2 - a second element in list", 0, 0,
	NewLItem("item 3 - foo", 0, 0,
	NewLItem("item 4 - item that cannot be selected", 0, ITEM_DISABLED,
	NewLItem("item 5 - the last item", 0, 0,
    0))))));
    win->RegisterClient(list);
    win->Draw();
    list->SetFlags(1, ITEM_SELECTED);
    list->SetState(SM_NO_REFRESH, false);
    list->AddLast("item 6 - dynamic added", 0, 0);
    list->Run();
    result = list->GetCurrentItem();
    result1 = list->GetSelected(0);
    delete list;
    delete win;
    XDone();
    printf("cursor in item %d\n", result);
    printf("item %d selected\n", result1);
}
