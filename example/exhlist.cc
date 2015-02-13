/*
  exhlist.cc

  Copyright (c) 1999 Ric Klaren

  This program is public domain; you can redistribute it and/or
  modify without restrictions. This program is distributed in the hope that
  will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <stdio.h>
#include <xterminal.h>

int main()
{
  XtHList *things;
  XInit();

  things = new XtHList(XRect(3, 2, 30, 8),
    HLIST_SELECTABLE_ITEMS | HLIST_SELECT_LIMIT, 1);

  /* Caveat: Nodes have to have unique names. For simple things it will suffice
     but some things will be hard to make with the current implementation.
  */
  things->AddNode("People", HLI_OPEN);	// this branch will be open on display
  things->AddNode("Hosts");		// this one will be closed

  /* Add some leaves to the branches....
     the number parameter is a key that will be attached to the name
     practical for database id's and stuff.
  */
  things->Add("People", "John Doe", 0);
  things->Add("People", "Jane Doe", 1);
  things->Add("Hosts", "zaphod", 2);
  things->Add("Hosts", "theluggage", 3);
  things->Add("Hosts", "eniac", 4);
  things->Add("Hosts", "eniac", 5);

  things->Run();
  XDone();

  if(things->NumberSelected() > 0)
    {
    /* get the names of the selected items. The SelectedKeys method will return
       the keys that were assigned to the nodes as they were added to the list
    */
    vector<char *> items;
    things->SelectedStrings(items);
    vector<char *>::const_iterator i = items.begin(), end = items.end();
    for(; i < end; i++)
      printf("%s\n", *i);
    }
  delete things;
  return 0;
}
