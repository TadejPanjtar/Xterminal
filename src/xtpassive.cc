/*
  xtpassive.cc

  Copyright (c) 1996-2000 Dragos Acostachioaie

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

#ifdef __GNUG__
#pragma implementation
#endif

#include <xtpassive.h>
#include <xtscreen.h>

XtStaticText::XtStaticText(XRect __r, unsigned __Alignment, const char *__Text) :
  XObject(__r)
{
  strcpy(ObjClass, "XtStaticText");
  Text = new std::string(__Text);
  Alignment = __Alignment;
  SetPalette(P_STATICTEXT, P_STATICTEXT_LEN);
}

XtStaticText::~XtStaticText()
{
  delete Text;
}

void XtStaticText::Draw()
{
  unsigned i, lines = 1;
  std::string::iterator index, begin = Text->begin();
  std::string::iterator end = Text->end();
  XPoint pos;
  div_t x, y;
  i = 1;
  for(index = begin; index != end; index++, i++)
    {
    if(*index == '\n')
      i = 0;
    if(i == Size.x + 1)
      {
      Text->insert(index++, '\n');
      end = Text->end();
      i = 0;
      }
    }
  for(index = begin; index != end; index++)
    if(*index == '\n')
      lines++;
  if(lines > Size.y)
    lines = Size.y;
  index = begin;
  i = 1;
  XSetColor(GetPalEntry(P_STATICTEXT_TEXT), GetPalEntry(P_STATICTEXT_TEXT + 1));
  while(i <= lines)
    {
    switch(Alignment)
      {
      case AS_LEFT:
        pos.x = Bounds.a.x;
	pos.y = Bounds.a.y + i - 1;
	break;
      case AS_CENTER:
        x = div(Size.x - ((i == lines ? Text->length() : Text->find('\n', index - begin)) - (index - begin)), 2);
	pos.x = Bounds.a.x + x.quot;
	pos.y = Bounds.a.y + i - 1;
	break;
      case AS_RIGHT:
        pos.x = Bounds.b.x - ((i == lines ? Text->length() : Text->find('\n', index - begin)) - (index - begin)) + 1;
	pos.y = Bounds.a.y + i - 1;
	break;
      case AS_TOP:
        pos.x = Bounds.a.x;
        pos.y = Bounds.a.y + i - 1;
	break;
      case AS_BOTTOM:
        pos.x = Bounds.a.x;
        pos.y = Bounds.b.y - lines + i;
	break;
      case AS_MIDDLE:
        x = div(Size.x - ((i == lines ? Text->length() : Text->find('\n', index - begin)) - (index - begin)), 2);
	pos.x = Bounds.a.x + x.quot;
        y = div(Size.y - lines, 2);
	pos.y = Bounds.a.y + y.quot + i - 1;
        break;
      default:
        pos.x = Bounds.a.x;
        pos.y = Bounds.a.y + i - 1;
      }
      for(; index != (i == lines ? end : begin + Text->find('\n', index - begin)); index++, pos.x++)
          if(pos.x <= Bounds.b.x)
              XPrintChar(pos, *index);
      index++;
      i++;
    }
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
  XObject::SetState(SM_VISIBLE, true);
}

const char *XtStaticText::GetValue()
{
  return Text->c_str();
}

void XtStaticText::SetValue(const char *__Text)
{
  if(GetState(SM_VISIBLE) && strlen(__Text) < Text->length())
    {
    std::string::iterator index = Text->begin();
    std::string::iterator end = Text->end();
    for(; index != end; index++)
      if(*index != '\n')
        *index = ' ';
    XtStaticText::Draw();
    }
  Text->assign(__Text);
  if(GetState(SM_VISIBLE))
    XtStaticText::Draw();
}

XtContextualHelp::XtContextualHelp(XRect __r, unsigned __Alignment, unsigned long __Flags) :
  XtStaticText(
    (__Flags & DISPLAY_MARGINS) ? XRect(__r.a + XPoint(1, 1), __r.b - XPoint(1, 1)) : __r,
    __Alignment, "")
{
  strcpy(ObjClass, "XtContextualHelp");
  Flags = __Flags;
  Context = 0;
  Wnd = 0;
  if(__Flags & DISPLAY_MARGINS)
    Wnd = new XtWindow(__r, "\0");
}

XtContextualHelp::~XtContextualHelp()
{
  ContextsMap::iterator index = ContextMap.begin();
  ContextsMap::iterator end = ContextMap.end();
  for(; index != end; index++)
    {
    delete (*index).second;
    ContextMap.erase(index);
    }
}

void XtContextualHelp::Draw()
{
  ContextsMap::iterator index = ContextMap.begin();
  ContextsMap::iterator end = ContextMap.end();
  if(Flags & DISPLAY_MARGINS)
    Wnd->Draw();
  for(; index != end; index++)
    if((*index).first == Context)
      {
      XtStaticText::SetValue((*index).second->c_str());
      XtStaticText::Draw();
      break;
      }
}

unsigned long XtContextualHelp::GetContext()
{
  return Context;
}

const char *XtContextualHelp::GetValue(unsigned long __Context)
{
  ContextsMap::iterator index = ContextMap.begin();
  ContextsMap::iterator end = ContextMap.end();
  for(; index != end; index++)
    if((*index).first == __Context)
      {
      return (*index).second->c_str();
      break;
      }
  return 0;
}

void XtContextualHelp::SetContext(unsigned long __Context)
{
  ContextsMap::iterator index = ContextMap.begin();
  ContextsMap::iterator end = ContextMap.end();
  bool found = false;
  for(; index != end; index++)
    if((*index).first == __Context)
      {
      found = true;
      break;
      }
  if(found)
    Context = __Context;
  if(GetState(SM_VISIBLE))
    Draw();
}

void XtContextualHelp::SetValue(unsigned long __Context, const char *__Text)
{
  std::string *temp;
  ContextsMap::iterator index = ContextMap.begin();
  ContextsMap::iterator end = ContextMap.end();
  bool found = false;
  for(; index != end; index++)
    if((*index).first == __Context)
      {
      found = true;
      break;
      }
  if(found)
    (*index).second->assign(__Text, strlen(__Text) + 1);
   else
    {
    temp = new std::string(__Text, strlen(__Text) + 1);
    ContextsKeyValPair *pair = new ContextsKeyValPair(__Context, temp);
    ContextMap.insert(*pair);
    }
}
