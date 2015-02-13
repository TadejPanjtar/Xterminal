/*
  bclass.h
  Definitions for the basically geometrical classes

  Copyright (c) 1995-99 Dragos Acostachioaie
  Documentation Copyright (c) 1998 Bernd Kalbfuss

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

#ifndef _XBCLASS_H
#define _XBCLASS_H

/** The XPoint class is the two dimensional representation of a
    geometrical point with the coordinates x and y. Methods to assign
    coordinates and operators for mathematical calculation are included.

    @memo Geometrical two-dimensional point
*/  
class XPoint {
  public:
    /// X coordinate of the point
    unsigned x;

    /// Y coordinate of the point
    unsigned y;	

    /// Constructs a clean point with the coordinates (0, 0)
    XPoint();

    /** Constructs a new point with the coordinates (__x, __y)
	@param __x x coordinate
	@param __y y coordinate
    */
    XPoint(unsigned __x, unsigned __y);

    /** Assigns the specified coordinates to the point
	@param __x x coordinate
        @param __y y coordinate
    */
    void Assign(unsigned __x, unsigned __y);

    /** Translates the point by the coordinates of the specified point
	@param __delta translation vector
        @return translated point
    */       
    XPoint& operator += (XPoint __delta);

    /** Negatively translates the point by the coordinates of the specified
	point
        @param __delta translation vector
        @return translated point
    */
    XPoint& operator -= (XPoint __delta);

    /** Adds the coordinates of two points and returns the result as point
        @param __p right operand
        @return sum of points
    */
    XPoint operator + (XPoint __p);

    /** Substracts the coordinates of two points and returns the result as
        point
        @param __p right operand
        @return difference of points
    */
    XPoint operator - (XPoint __p);

    /** Assigns the point the coordinates of the specified point
        @param __p value to assign
        @return assigned point
    */
    XPoint& operator = (XPoint __p);

    /** Compares the coordinates of two points and returns true if they are
        equal
        @param __p right operand
        @return true if equal 
    */
    bool operator == (XPoint __p);

    /** Compares the coordinates of two points and returns false if they are
        equal
        @param __p right operand
        @return false if equal
    */
    bool operator != (XPoint __p);

    /** Compares the coordinates of two points and returns true if the first
        one is nearer to the origin
        @param __p right operand
        @return true if smaller
    */
    bool operator < (XPoint __p);

    /** Compares the coordinates of two points and returns true if the second
        one is nearer to the origin
        @param __p right operand
        @return true if greater
    */
    bool operator > (XPoint __p);

    /** Compares the coordinates of two points and returns true if the first
        one is nearer or as far as the second one from the origin
        @param __p right operand
        @return true if smaller or equal
    */
    bool operator <= (XPoint __p);

    /** Compares the coordinates of two points and returns true if the second
        one is nearer or as far as the first one from the origin
        @param __p right operand
        @return true if greater or equal
    */
    bool operator >= (XPoint __p);
};


/** The XRect class is the two dimensional representation of a geometrical
    rectangle with the upper left corner a and the lower left corner b.
    Methods to assign corners and operators for mathematical calculation
    are included.

    @memo Geometrical two-dimensional rectangle
*/
class XRect {
  public:
    /// Upper left corner
    XPoint a;

    /// Lower right corner
    XPoint b;

    /// Constructs a clean rectangle with the corners a(0, 0), b(0, 0)
    XRect();

    /** Constructs a new point with the corners a(__xa, __ya), b(__xb, __yb)
        @param __xa x and
        @param __ya y coordinate of upper left corner
        @param __xb x and
        @param __yb y coordinate of lower right corner
    */        
    XRect(unsigned __xa, unsigned __ya, unsigned __xb, unsigned __yb);

    /** Constructs a new point with the specified corners
        @param __a upper left corner
        @param __b lower right corner
    */
    XRect(XPoint __a, XPoint __b);

    /** Assigns the specified corners to the rectangle
        @param __a upper left corner
        @param __b lower right corner
    */
    void Assign(XPoint __a, XPoint __b);

    /** Assigns the specified coordinates to the rectangle
        @param __xa x and
        @param __ya y coordinate of upper left corner
        @param __xb x and
        @param __yb y coordinate of lower right corner
    */
    void Assign(unsigned __xa, unsigned __ya, unsigned __xb, unsigned __yb);

    /** Moves the rectangle relatively by the specified coordinates
        @param __delta translation vector
    */
    void Move(XPoint __delta);

    /** Grows the rectangle by adjusting the lower-right corner with the
        specified coordinates
        @param __delta translation vector
    */
    void Grow(XPoint __delta);

    /** Assigns the rectangle the intersection with another rectangle
        @param __r other rectangle
    */
    void Intersect(XRect __r);

    /** Checks if the coordinates can form a rectangle
        @param true if coordinates are valid
    */
    bool IsValid();

    /** Assigns the rectangle the union with another rectangle
        @param __r other rectangle
    */
    void Union(XRect __r);

    /** Returns true if the rectangle contains the specified point
        @param __p point
        @return true if point in rectangle
    */
    bool Contains(XPoint __p);

    /** Assigns the rectangle the corners of the specified rectangle
        @param __r value to assign
        @return assigned rectangle
    */
    XRect& operator = (XRect __r);

    /** Compares to rectangles and returns true if they are equal
        @param __r right operand
        @return true if equal
    */
    bool operator == (XRect __r);

    /** Compares to rectangles and returns false if they are equal
        @param __r right operand
        @return false if equal
    */  
    bool operator != (XRect __r);

    /** Positively translates the rectangle by adjusting the lower-right
	corner with the specified coordinates
        @param __delta translation vector
        @return translated rectangle
    */
    XRect& operator += (XPoint __delta);

    /** Negatively translates the rectangle by adjusting the lower-right
	corner with the specified coordinates
        @param __delta translation vector
        @return translated rectangle
    */
    XRect& operator -= (XPoint __delta);
};

#endif
