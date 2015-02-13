/*
  xbclass.cc

  Copyright (c) 1995-99 Dragos Acostachioaie

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

#include <xbclass.h>

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

void XPoint::Assign(unsigned __x, unsigned __y)
{
  x = __x;
  y = __y;
}

XPoint::XPoint()
{
  Assign(0, 0);
}

XPoint::XPoint(unsigned __x, unsigned __y)
{
  Assign(__x, __y);
}

XPoint& XPoint::operator += (XPoint __delta)
{
  x += __delta.x;
  y += __delta.y;
  return *this;
}

XPoint& XPoint::operator -= (XPoint __delta)
{
  x -= __delta.x;
  y -= __delta.y;
  return *this;
}

XPoint XPoint::operator + (XPoint __p)
{
  return XPoint(x + __p.x, y + __p.y);
}

XPoint XPoint::operator - (XPoint __p)
{
  return XPoint(x - __p.x, y - __p.y);
}

XPoint& XPoint::operator = (XPoint __p)
{
  x = __p.x;
  y = __p.y;
  return *this;
}

bool XPoint::operator == (XPoint __p)
{
  return x == __p.x && y == __p.y;
}

bool XPoint::operator != (XPoint __p)
{
  return !(*this == __p);
}

bool XPoint::operator < (XPoint __p)
{
  return x < __p.x && y < __p.y;
}

bool XPoint::operator > (XPoint __p)
{
  return x > __p.x && y > __p.y;
}

bool XPoint::operator <= (XPoint __p)
{
  return x <= __p.x && y <= __p.y;
}

bool XPoint::operator >= (XPoint __p)
{
  return x >= __p.x && y >= __p.y;
}

void XRect::Assign(XPoint __a, XPoint __b)
{
  a = __a;
  b = __b;
}

void XRect::Assign(unsigned __xa, unsigned __ya, unsigned __xb, unsigned __yb)
{
  a.x = __xa;
  a.y = __ya;
  b.x = __xb;
  b.y = __yb;
}

XRect::XRect()
{
  Assign(0, 0, 0, 0);
}

XRect::XRect(unsigned __xa, unsigned __ya, unsigned __xb, unsigned __yb)
{
  Assign(__xa, __ya, __xb, __yb);
}

XRect::XRect(XPoint __a, XPoint __b)
{
  Assign(__a, __b);
}

void XRect::Move(XPoint __delta)
{
  a += __delta;
  b += __delta;
}

void XRect::Grow(XPoint __delta)
{
  b += __delta;
}

void XRect::Intersect(XRect __r)
{
  a.x = max(a.x, __r.a.x);
  a.y = max(a.y, __r.a.y);
  b.x = min(b.x, __r.b.x);
  b.y = min(b.y, __r.b.y);
}

bool XRect::IsValid()
{
  return b.x >= a.x && b.y >= a.y;
}

void XRect::Union(XRect __r)
{
  a.x = min(a.x, __r.a.x);
  a.y = min(a.y, __r.a.y);
  b.x = max(b.x, __r.b.x);
  b.y = max(b.y, __r.b.y);
}

bool XRect::Contains(XPoint __p)
{
  return __p.x >= a.x && __p.x <= b.x && __p.y >= a.y && __p.y <= b.y;
}

XRect& XRect::operator = (XRect __r)
{
  a = __r.a;
  b = __r.b;
  return *this;
}

bool XRect::operator == (XRect __r)
{
  return a == __r.a && b == __r.b;
}

bool XRect::operator != (XRect __r)
{
  return !(*this == __r);
}

XRect& XRect::operator += (XPoint __delta)
{
  Grow(__delta);
  return *this;
}

XRect& XRect::operator -= (XPoint __delta)
{
  Grow(XPoint((-1) * __delta.x, (-1) * __delta.y));
  return *this;
}
