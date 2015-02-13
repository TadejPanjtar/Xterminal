/*
  xthlist.cc

  Written by Ric Klaren based on xtlist by Dragos Acostachioaie

  Copyright (c) 1998 Dragos Acostachioaie
  Copyright (c) 1999 Ric Klaren

  This file is part of Xterminal library.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the license, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <ctype.h>
#include <string.h>

#include <xtscreen.h>
#include <xtlib.h>
#include <xthlist.h>

// create new hlist without contents
XtHList::XtHList(XRect __r, unsigned long __Flags, unsigned int __max_selectable = 1) :
  XObject(__r)
{
  Flags = __Flags;
  number_selected = 0;
  max_selectable = __max_selectable;
  if((Flags & HLIST_SELECT_LIMIT) == 0)
    max_selectable = 0;
  CurrIndex = 0;
  CurrItem = 0;
  ItemCount = LineCount = 0;
  LineOffset = 1;
  CurY = 1;
  draw_index = indent = ndrawn = 0;
  root = 0;
  strcpy(ObjClass, "XtHList");
  SetPalette(P_HLIST, P_HLIST_LEN);
  InitScrollBar();
  XObject::SetState(SM_ACTIVE, true);
}

void XtHList::InitScrollBar()
{
  ScrollBar = new XtScrollBar(
    XRect(Size.x - 1, 0, Size.x - 1, Size.y - 1),
    GetItemCount(),
    false);
  ScrollBar->SetState(SM_NO_REFRESH, true);

  RegisterClient(ScrollBar);
  AddListener(ScrollBar->GetId(), EV_MOUSE);
}

XtHList::~XtHList()
{
  XtHLItem *t;
  while(root != 0)
    {
    t = root;
    root = root->Next();
    delete t;
    }
  if(ScrollBar)
    delete ScrollBar;
  XDebug("Destructing XtHList");
}

void XtHList::DeselectFirst()
{
  XtHLItem *i = root;
  while(i)
    {
    if(i->Selected())
      {
      i->Deselect();
      number_selected--;
      return;
      }
    i = i->Next();
    }
}

void XtHList::Select(XtHLItem *item)
{
  if(item->Type() == HLI_TYPE_LEAF && !item->Selected())
    {
    if((Flags & HLIST_SELECT_LIMIT) && number_selected == max_selectable)
      DeselectFirst();

    item->Select();
    number_selected++;
    }
}

void XtHList::Deselect(XtHLItem *item)
{
  if(item->Type() == HLI_TYPE_LEAF && item->Selected())
    {
    item->Deselect();
    number_selected--;
    }
}

unsigned int XtHList::NumberSelected() const
{
  return number_selected;
}

void XtHList::ClearSelected()
{
  XtHLItem *i = root;
  while(i)
    {
    Deselect(i);
    i = i->Next();
    }
  number_selected = 0;
}

void XtHList::SelectedKeys(vector<unsigned long>& values) const
{
  XtHLItem *item = root;

  values.clear();

  while(item)
    {
    if(item->Selected() && item->Type() == HLI_TYPE_LEAF)
      values.push_back(((XtHLItemLeaf *)item)->Key());
    item = item->Next();
    }
}

void XtHList::SelectedStrings(vector<char *>& values) const
{
  XtHLItem *item = root;

  values.clear();

  while(item)
    {
    if(item->Selected())
    values.push_back(item->Text());
    item = item->Next();
    }
}

int XtHList::AddNode(char *__Text, int state = HLI_NONE)
{
  if(root == 0)
    {
    CurrItem = root = new XtHLItemNode(__Text, state, 0);
    CurrIndex = 1;
    ItemCount++;
    }
   else
    {
    LastItem()->SetNext(new XtHLItemNode(__Text, state, 0));
    ItemCount++;
    }
  UpdateLineCount();
  return 0;
}

bool XtHList::AddNode(const char *to_node, char *text, int state = HLI_NONE)
{
  // find struct...
  XtHLItemNode *node = FindNode(to_node);

  if(node == 0)
    return false;

  node->AddNode(text, state);
  if(++ItemCount == 1)
    {
    CurrIndex = 1;
    CurrItem = root;
    }
  UpdateLineCount();
  return true;
}

bool XtHList::Add(const char *to_node, char *text, unsigned long value,
  int state = HLI_NONE)
{
  // find struct...
  XtHLItemNode *node = FindNode(to_node);

  if(node == 0)
    return false;

  node->Add(text, value, state);
  if(++ItemCount == 1)
    {
    CurrIndex = 1;
    CurrItem = root;
    }

  UpdateLineCount();

  return true;
}

XtHLItemNode *XtHList::FindNode(const char *text) const
{
  for(XtHLItem *item = root; item; item = item->Next() )
    {
    if(item->Type() == HLI_TYPE_LEAF)
      continue;
    if(strcmp( item->Text(), text ) == 0)
    return (XtHLItemNode *)item;
    }
  return 0;
}

void XtHList::DrawItem(const XtHLItem *const item, const XtHLItem *const last)
{
  unsigned int j = 1;

  if(CurrItem == item)
    {
    if(item->Disabled())
      XSetColor(GetPalEntry(P_HLIST_SELECTED_DISABLED), GetPalEntry(P_HLIST_SELECTED_DISABLED + 1));
     else
      XSetColor(GetPalEntry(P_HLIST_SELECTED), GetPalEntry(P_HLIST_SELECTED + 1));
    }
   else
    {
    if(item->Disabled())
      XSetColor(GetPalEntry(P_HLIST_DISABLED), GetPalEntry(P_HLIST_DISABLED + 1));
     else
      XSetColor(GetPalEntry(P_HLIST_SURFACE), GetPalEntry(P_HLIST_SURFACE + 1));
    }

  for(unsigned int i = 1; i <= indent; i++)
    {
    XPrintChar(XPoint(Bounds.a.x + j++, Bounds.a.y + ndrawn), ' ');
    if((i == indent || !item->Next()) && (item == last) ||
	(item->Type() == HLI_TYPE_NODE &&
	!((XtHLItemNode *)item)->Opened() &&
	((XtHLItemNode *)item)->LastChild() &&
	(((XtHLItemNode *)item)->LastChild()->Next() == 0)))
      XPrintChar(XPoint(Bounds.a.x + j++, Bounds.a.y + ndrawn), ACS_LLCORNER);
     else
      XPrintChar(XPoint(Bounds.a.x + j++, Bounds.a.y + ndrawn), ACS_VLINE);
    XPrintChar(XPoint(Bounds.a.x + j++, Bounds.a.y + ndrawn), ' ');
    }

  // put an indicator in front of it if necessary...
  if(item->Type() == HLI_TYPE_NODE)
    XPrintChar(XPoint(Bounds.a.x + j++, Bounds.a.y + ndrawn),
      ((XtHLItemNode *)item)->Indicator());
   else
    if((Flags & HLIST_SELECTABLE_ITEMS) && item->Selected())
      XPrintChar(XPoint(Bounds.a.x + j++, Bounds.a.y + ndrawn), ACS_DIAMOND);
     else
      XPrintChar(XPoint(Bounds.a.x + j++, Bounds.a.y + ndrawn), ' ');

  char *s = item->Text();

  for(int i = 0; j < Size.x - 2 && s[i]; i++, j++)
    XPrintChar(XPoint(Bounds.a.x + j, Bounds.a.y + ndrawn), s[i]);

  ndrawn++;
}

/* the snode itself is drawn already...
   this one takes care of it's children
   Returns the last item in branch
*/
XtHLItem *XtHList::DrawNode(XtHLItemNode *snode)
{
  XtHLItem *item;
  XtHLItemNode *node;

  if(snode->Next() == 0 || snode->LastChild() == 0)
    return snode;			// node has no children

  indent++;				// all children have an indent...

  item = snode->Next();			// skip to first child

  while(item && ndrawn < Size.y)
    {
    draw_index++;

    switch(item->Type())
      {
      case HLI_TYPE_NODE:
	if(draw_index >= LineOffset)	// is visible?
	  DrawItem(item, snode->LastChild());

	// see if we should draw the children of the item
	node = (XtHLItemNode *)item;

	if(node->Opened())
	  item = DrawNode(node);	// returns last item in branch
	 else
	  if(node->LastChild())		// if children exist
	    item = node->LastChild();
					// else item is last in branch...
	break;
      case HLI_TYPE_LEAF:
	if(draw_index >= LineOffset)	// have we entered the visible area?
	  DrawItem(item, snode->LastChild());
	break;
      default:
	XDebug("XtHList::DrawNode Unknown HLI_TYPE");
      }
    if((item == snode->LastChild()) || (ndrawn == Size.y))
      {
      indent--;
      return snode->LastChild();
      }
    item = item->Next();
    }
//  XDebug("XtHList::DrawNode() BUG!");
  return item;				// and prolly let it crash...
}

void XtHList::Draw()
{
  // draw non item stuff...
  if(ScrollBar && !ScrollBar->GetState(SM_VISIBLE))
    ScrollBar->Draw();

  XColorWindow(XRect (Bounds.a.x, Bounds.a.y, Bounds.b.x - 1, Bounds.b.y),
    GetPalEntry(P_HLIST_SURFACE), GetPalEntry(P_HLIST_SURFACE + 1));

  // draw the cursor
  if(GetState(SM_FOCUSED) && CurrItem)
    if(CurrItem->Disabled())
      XColorWindow(XRect(Bounds.a.x, Bounds.a.y + CurY - 1, Bounds.b.x - 1, Bounds.a.y + CurY - 1),
	GetPalEntry(P_HLIST_SELECTED_DISABLED),
	GetPalEntry(P_HLIST_SELECTED_DISABLED + 1));
     else
      XColorWindow(XRect(Bounds.a.x, Bounds.a.y + CurY - 1, Bounds.b.x - 1, Bounds.a.y + CurY - 1),
	GetPalEntry(P_HLIST_SELECTED),
	GetPalEntry(P_HLIST_SELECTED + 1));

  // now take care of items...
  XtHLItem *item = root;

  draw_index = indent = ndrawn = 0;

  while(item && ndrawn < Size.y)
    {
    draw_index++;

    switch(item->Type())
      {
      case HLI_TYPE_NODE:
	if(draw_index >= LineOffset)	// have we entered the visible area?
	  DrawItem(item, 0);

        // see if we should draw the children of the branch
	if(((XtHLItemNode *)item)->Opened())
	  item = DrawNode((XtHLItemNode *)item);
         else
	  item = ((XtHLItemNode *)item)->LastChild();

        break;
      case HLI_TYPE_LEAF:
	if(draw_index >= LineOffset)	// have we entered the visible area?
	  DrawItem(item, 0);

        break;
      default:
	XDebug("XtHList::Draw Unknown HLI_TYPE");
      }
    if(item)
      item = item->Next();
    }

  if(ScrollBar)
    {
    ScrollBar->SetTotalNo(LineCount);
    ScrollBar->SetValue(CurrIndex - 1);
    }

  if(!GetState(SM_NO_REFRESH))
    XRefresh();

  XObject::SetState(SM_VISIBLE, true);
}

// Return the next item keeping in mind the state of a node (open close)
XtHLItem *XtHList::NextItem(XtHLItem *item) const
{
  if(item->Type() == HLI_TYPE_LEAF)
    return item->Next();
   else
    {
    XtHLItemNode *n = (XtHLItemNode *)item;
    if(n->Opened())
      return item->Next();
     else
      if(n->LastChild())
	return n->LastChild()->Next();
       else
	return n->Next();
    }
}

// return the number of items in the list
unsigned XtHList::GetItemCount() const
{
  return ItemCount;
}

// Update the number of visible lines (eg. don't count items in closed branches)
unsigned int XtHList::UpdateLineCount()
{
  if(root == 0)
    return 0;

  XtHLItem *t1 = root, *t2;

  LineCount = 1;

  for(;;)
    {
    t2 = NextItem(t1);
    if(t2 == 0)
      return LineCount;
    t1 = t2;
    LineCount++;
    }
}

// retrieve a pointer to the item that is residing on index 'line'
XtHLItem *XtHList::GetItemOnLine(unsigned int line) const
{
  if(root == 0 || line > LineCount)
    return 0;

  XtHLItem *t1 = root, *t2;

  for(unsigned int n = 1;;)
    {
    if(n == line)
      return t1;

    t2 = NextItem(t1);
    if(t2 == 0)				// should not happen...
      return 0;
    t1 = t2;
    n++;
    }
}

// search the last visible item
XtHLItem *XtHList::LastVisibleItem() const
{
  XtHLItem *t1 = root, *t2;
  int n = 0;

  for(;;)
    {
    if(t1 == 0)
      return 0;

    n++;

    t2 = NextItem(t1);
    if(t2 == 0)
      return t1;
    t1 = t2;
    }
}

// get the end of the complete list
XtHLItem *XtHList::LastItem() const
{
  XtHLItem *item = root, *t;

  for(;;)
    {
    if(item->Type() == HLI_TYPE_LEAF)
      t = item->Next();
     else
      {
      XtHLItemNode *n = (XtHLItemNode *)item;
      if(n->LastChild())
	t = n->LastChild();
       else
        t = n->Next();
      }
    if(t == 0)
      return item;
    item = t;
    }
}

// calculate the line (in the list) of XtHLItem target
unsigned int XtHList::GetItemLine(XtHLItem *target) const
{
  XtHLItem *item = root;
  unsigned int line = 0;

  while(item)
    {
    line++;
    if(target == item)
      return line;
    item = NextItem(item);
    }
  return 1;
}

// Open all branches on the way to item target
bool XtHList::OpenBranches(XtHLItem *target)
{
  XtHLItem *item = root,*nxt;

  while(item)
    {
    switch(item->Type())
      {
      case HLI_TYPE_NODE:
	OpenBranchesNode(target, (XtHLItemNode*)item);
	nxt = ((XtHLItemNode *)item)->LastChild();
	if(nxt)
	  item = nxt;
        break;
      case HLI_TYPE_LEAF:
	if(item == target)
	  return true;

        break;
      default:
	XDebug("XtHList::OpenBranches Unknown HLI_TYPE");
      }
    item = item->Next();
    }
  return false;
}

/* Utility function for OpenBranches
   NOTE: used recursively
*/
bool XtHList::OpenBranchesNode(XtHLItem *target, XtHLItemNode *node)
{
  XtHLItem *item = node->Next();

  while(item)
    {
    switch(item->Type())
      {
      case HLI_TYPE_NODE:
        if(item == target)
	  {
	  node->Open();
	  return true;
	  }

	if(OpenBranchesNode(target, (XtHLItemNode*)item))	// item was in there?
	  {
	  node->Open();			// open this one..
	  return true;			// and let previous invocations know too..
	  }
	 else				// skip over children...
	  {
	  XtHLItem *nxt = ((XtHLItemNode *)item)->LastChild();
	  if(nxt)
	    item = nxt;
	  }
	break;
      case HLI_TYPE_LEAF:
	if(item == target)
	  {
	  node->Open();
	  return true;
	  }
	break;
      default:
	XDebug("XtHList::OpenBranches Unknown HLI_TYPE");
      }
    if(node && (item == node->LastChild()))	// last one of a node sweep
      return false;

    item = item->Next();
    }
  return false;
}

bool XtHList::SearchLetter(XtHLItem *item, int KeyCode)
{
	while(item)
	{
		if( tolower(item->FirstLetter()) == KeyCode )
		{
			ExposeItem( item );
			return true;
		}
		item = item->Next();
	}
	return false;
}

bool XtHList::ProcessKeyboardEvent(XKeyboardEvent * __Event)
{
  XMessage *msg;
  XtHLItem *item = 0;

  switch(__Event->KeyCode)
    {
    case KB_RIGHT:
    case KB_DOWN:
      if(CurrIndex < LineCount)
        {
	if((item = NextItem(CurrItem)) != 0)
	  {
	  CurrIndex++;
	  CurrItem = item;

	  if(CurY < Size.y)
	    CurY++;
	   else
	    LineOffset++;
	  Draw();
	  }
	return true;
	}
       else
        if((Flags & HLIST_CURSOR_WRAP) && CurY == Size.y)
	  {
	  CurrIndex = 1;
	  CurrItem = root;
	  LineOffset = CurY = 1;
	  Draw();
	  return true;
	  }
      break;
    case KB_LEFT:
    case KB_UP:
      if(CurrIndex > 1)
	{
	CurrIndex--;
	CurrItem = GetItemOnLine(CurrIndex);

        if(CurY > 1)
	  CurY--;
	 else
	  LineOffset--;
	Draw();
	return true;
        }
       else
        if(Flags & HLIST_CURSOR_WRAP)
	  {
	  if(LineCount <= Size.y)		// it fits in the window
	    {
	    LineOffset = 1;
	    CurY = LineCount;
	    }
	   else
	    {
	    LineOffset = LineCount - Size.y + 1;
	    CurY = Size.y;
	    }
	  CurrIndex = LineCount;
	  CurrItem = LastVisibleItem();
	  CurrIndex = LineCount;		// and get the index..

	  Draw ();
	  return true;
	  }
      break;
    case KB_SPACE:
      if((Flags & HLIST_SELECTABLE_ITEMS) && !CurrItem->Disabled())
	{
	switch(CurrItem->Type())
	  {
	  XtHLItemNode *n;
	  case HLI_TYPE_LEAF:
	    if(CurrItem->Selected())
	      Deselect(CurrItem);
	     else
	      Select(CurrItem);
	    break;
	  case HLI_TYPE_NODE:
	    n = (XtHLItemNode *)CurrItem;
	    n->ToggleOpen();
	    UpdateLineCount();
	    break;
	  default:
	    XDebug("Can't happen in XtHList keyboardhandler");
	  }
	Draw();
	return true;
	}
      break;
    case KB_ENTER:
      if(!CurrItem->Disabled())
	{
	msg = new XMessage;
	msg->Class = EV_COMMAND;
	msg->Type = MC_CLIENT_TO_SERVER;
	msg->Message = CM_OK;
	_PutEventInFront(msg);
	return true;
	}
       else
	XBeep();
	break;
    case KB_ESC:
      msg = new XMessage;
      msg->Class = EV_COMMAND;
      msg->Type = MC_CLIENT_TO_SERVER;
      msg->Message = CM_CLOSE;
      _PutEventInFront(msg);
      return true;
    case KB_PPAGE:
      if(CurrIndex > 1)
	{
	if(CurrIndex > Size.y)
	  {
	  if(CurY == 1)
	    {
	    CurrIndex -= Size.y - 1;
	    CurrItem = GetItemOnLine(CurrIndex);
	    LineOffset -= Size.y - 1;
	    }
	   else
	    {
	    CurrIndex -= CurY - 1;
	    CurrItem = GetItemOnLine(CurrIndex);
	    }
	  CurY = 1;
	  }
         else
	  {
	  CurrIndex = 1;
	  CurrItem = root;
	  LineOffset = CurY = 1;
	  }
        Draw();
	return true;
	}
      break;
    case KB_NPAGE:
      if(CurrIndex < LineCount)
	{
	if(CurrIndex + Size.y < LineCount)
	  {
	  CurrIndex += Size.y - 1;
	  CurrItem = GetItemOnLine(CurrIndex);
	  if(CurY > 1)
	    LineOffset += CurY - 1;
	  CurY = Size.y;
	  }
	 else
	  {
	  CurrIndex = LineCount;
	  CurrItem = LastVisibleItem();

	  if(LineCount > Size.y)
	    {
	    LineOffset = LineCount - Size.y + 1;
	    CurY = Size.y;
	    }
	   else
	    {
	    LineOffset = 1;
	    CurY = ndrawn;
	    }
	  }
	Draw();
        return true;
	}
      break;
    case KB_HOME:
      if(CurrIndex > 1)
	{
	CurrIndex = 1;
	CurrItem = root;
	LineOffset = CurY = 1;
	Draw();
	return true;
	}
      break;
    case KB_END:
      if(CurrIndex < LineCount)
	{
	CurrIndex = LineCount;
	CurrItem = LastVisibleItem();
      	if(LineCount > Size.y)
	  {
	  LineOffset = LineCount - Size.y + 1;
	  CurY = Size.y;
      	  }
      	 else
	  {
	  LineOffset = 1;
	  CurY = LineCount;
      	  }
	Draw();
	return true;
	}
      break;
    case KB_META('a'):
      if((Flags & HLIST_SELECTABLE_ITEMS))
	{
	item = root;
	while(item)
	  {
	  if(!item->Disabled())
	    Select(item);
	  item = item->Next();
	  }
	Draw();
        return true;
	}
      break;
    case KB_META('c'):
      if((Flags & HLIST_SELECTABLE_ITEMS))
	{
	ClearSelected();
	Draw();
	return true;
	}
      break;
    case KB_META('r'):
      if((Flags & HLIST_SELECTABLE_ITEMS))
	{
	item = root;
	while(item)
	  {
	  if(!item->Disabled())
	    {
	    if(item->Selected())
	      Deselect(item);
	     else
	      Select(item);
	    }
	  item = item->Next();
	  }
	Draw();
	return true;
	}
      break;
    default:
      if(__Event->KeyCode >= 0x20 && __Event->KeyCode <= 0xff)
	{
	// first attempt starting from the current item..
	if( CurrItem && CurrItem->Next())
	  {
	  if( SearchLetter( CurrItem->Next(), tolower( __Event->KeyCode )) )
	    return true;
	  }
	// not found retry from start...
	if( SearchLetter( root, tolower( __Event->KeyCode )) )
	    return true;
	}
      break;
    }
  return false;
}

/* Changes CurrItem/Index! does redraw too...
 * target should be valid...
 */
void XtHList::ExposeItem( XtHLItem *target )
{
	CurrItem = target;
	/* these three calls are very inefficient...
	   if need be make specialized OpenBranches
	*/
	OpenBranches(target);					// open all branches to target...
	UpdateLineCount();				// recount!
	CurrIndex = GetItemLine(target);		// and get the index..

	// now update window position
	if(CurrIndex > Size.y)			// see if currindex is in view
	  {						// nawp
	  if(CurrIndex > LineCount - Size.y)	// item is in 'last' window
	    {
	    LineOffset = LineCount - Size.y + 1;
	    CurY = CurrIndex - LineOffset + 1;
	    }
	   else
	    {
	    LineOffset = CurrIndex;
	    CurY = 1;
	    }
	  }
	 else
	  {
	  LineOffset = 1;
	  CurY = CurrIndex;
	  }
	Draw();
}

bool XtHList::ProcessMessage(XMessage * __Event)
{
  XMessage *msg;
  XKeyboardEvent *ev;
  switch(__Event->Message)
    {
    case MT_RELEASE_FOCUS:
      SetState(SM_FOCUSED, false);
      return true;
    case MT_RESUME_FOCUS:
      SetState(SM_FOCUSED, true);
      return true;
    case MT_ANSWER_REQUEST:
      msg = new XMessage;
      msg->Type = MC_CLIENT_TO_SERVER;
      msg->Message = number_selected;
      _PutEventInFront(msg);
      return true;
    case SCROLLBAR_ITEMUP:
      ev = new XKeyboardEvent;
      ev->KeyCode = KB_UP;
      ProcessKeyboardEvent(ev);
      delete ev;
      return true;
    case SCROLLBAR_ITEMDOWN:
      ev = new XKeyboardEvent;
      ev->KeyCode = KB_DOWN;
      ProcessKeyboardEvent(ev);
      delete ev;
      return true;
    }
  return false;
}

bool XtHList::ProcessMouseEvent(XMouseEvent *__Event)
{
  if(__Event->Type == EV_MOUSEDOWN)
    if(Bounds.Contains(__Event->Position))
      if(LineOffset + __Event->Position.y - Bounds.a.y <= ItemCount)
	{
	// let the scrollbar handle this...
	if(__Event->Position.x == (Bounds.b.x))
	  return false;

        if(__Event->Position.y == Bounds.a.y + CurY - 1 &&
	 GetState(SM_FOCUSED) && (Flags & HLIST_SELECTABLE_ITEMS))
          {
	  // click was on current item..
	  if(CurrItem && !CurrItem->Disabled())
	    {
	    if(__Event->Clicks > 1)
	      {
	      XMessage *msg = new XMessage;
	      msg->Type = MC_CLIENT_TO_SERVER;
	      msg->Message = CM_OK;
	      _PutEventInFront (msg);
	      return true;
	      }
	    if(CurrItem->Type() == HLI_TYPE_LEAF)
	      {
	      if(CurrItem->Selected())
	        Deselect(CurrItem);
	       else
	        Select(CurrItem);
	      }
	     else
              {
	      ((XtHLItemNode *)CurrItem)->ToggleOpen();
	      UpdateLineCount();
	      }
	    Draw();
	    return true;
	    }
	  }
	 else
	  {	// click was not on current item update item & stuff...
	  XMessage *msg = new XMessage;
	  msg->Type = MC_CLIENT_TO_SERVER;
	  msg->Message = MT_RESUME_FOCUS;

	  if(Server)
	    Server->HandleEvent(msg);

	  if(CurrItem == 0)
	    return true;

	  unsigned int click_pos = __Event->Position.y - Bounds.a.y;

	  if(click_pos > CurY)
	    {
	    for(unsigned int i = CurY; i <= click_pos; i++)
	      {
	      XtHLItem *t = NextItem(CurrItem);
	      if(t == 0)
	        break;
	      CurrItem = t;
	      }
	    }
	   else
	    {
	    CurrItem = GetItemOnLine(LineOffset);

	    for(unsigned int i = 0; i < click_pos; i++)
	      {
	      XtHLItem *t = NextItem(CurrItem);
	      if(t == 0)
	        break;
	      CurrItem = t;
	      }
	    }

	  // now fixup index and CurY
	  CurrIndex = GetItemLine(CurrItem);
	  CurY = CurrIndex - LineOffset + 1;

	  SetState(SM_FOCUSED, 1);
	  }
	return true;
	}
  return false;
}

int XtHList::Run()
{
  int result;
  XEvent *ev;
  Draw();
  ev = new XMessage;
  ((XMessage *)ev)->Message = MT_RESUME_FOCUS;
  HandleEvent (ev);
  delete ev;
  for(;;)
    {
    ev = FetchEvent();
    HandleEvent(ev);
    if(ev && ev->Class == EV_COMMAND && (((XMessage *) ev)->Message == CM_CLOSE || ((XMessage *) ev)->Message == CM_OK))
      break;
    if(ev)
      delete ev;
    }
  result = ((XMessage *) ev)->Message;
  delete ev;
  ev = new XMessage;
  ((XMessage *)ev)->Message = MT_RELEASE_FOCUS;
  HandleEvent(ev);
  delete ev;
  return result;
}

void XtHList::SetState(unsigned long __Mask, bool __BoolValue)
{
  XObject::SetState(__Mask, __BoolValue);
  if(ScrollBar)
    ScrollBar->SetState(__Mask, __BoolValue);
  if(GetState(SM_VISIBLE))
    Draw();
}

#ifdef DEBUG
void XtHList::Dump()
{
  XtHLItem *item = root;
  XDebug("----------------------------------------------------");
  while(item)
    {
    if(item->Type() == HLI_TYPE_NODE)
      XDebug("%p %s %p", item, item->Text(), ((XtHLItemNode *)item)->LastChild());
     else
      XDebug("%p %s", item, item->Text());
    item = item->Next();
    }
  XDebug("----------------------------------------------------");
}
#endif
