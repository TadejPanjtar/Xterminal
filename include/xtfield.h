/*
  xtfield.h
  Definition for XtInputStringField, XtInputNumericField and XtInputDateField

  Copyright (c) 1996-2000 Dragos Acostachioaie
  Copyright (c) 1999-2000 Udo Kreckel

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

#ifndef _XTFIELD_H
#define _XTFIELD_H

#include <xtscreen.h>
#include <xobject.h>

/**
    @memo Field to read strings
*/
class XtInputStringField : public XObject {
 public:

  /** Constructs a new XtInputStringField.
      @param __r field bounds
      @param __DefaultString default string
      @param __MaxLen maximum string length
      @param __Flags options, which can be:
         <p><b>FIELD_HIDDEN</b> - the output should be hidden
	 <p><b>FIELD_READONLY</b> - the content cannot be changed
	 <p><b>FIELD_JUSTIFIED_RIGHT</b> - the field is right-justified
	 <p><b>FIELD_UPPER_CASE</b> - the input is converted to upper case
	 <p><b>FIELD_LOWER_CASE</b> - the input is converted to lower case
	 <p><b>FIELD_AUTO_SKIP</b> - at end of field skip to next field
	 <p><b>FIELD_NO_CLEAR</b> - don't clear exist value at resume focus
  */
  XtInputStringField(XRect __r, const char *__DefaultString, unsigned __MaxLen, unsigned long __Flags);

  /// Destructs the object's instance
  virtual ~XtInputStringField();

  /// Clears the value
  virtual void ClearValue();

  /// Draws the object
  virtual void Draw();

  /// Returns the default value
  virtual const char *GetDefaultValue();

  /// Returns the actual input length of the input field
  virtual short GetInputLength();

  /// Returns the current value
  virtual const char *GetValue();

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Process messages
  virtual bool ProcessMessage(XMessage *__Event);

  /// Process mouse events
  virtual bool ProcessMouseEvent(XMouseEvent *__Event);

  /// Sets the object's state
  virtual void SetState(unsigned long __Mask, bool __BoolValue);

  /// Sets the default value
  virtual void SetValue(const char *__DefaultString);

 protected:
  char *DefaultString, *CurrString;
  unsigned short FieldLength, StringLength, MaxLen, CP, curX, curY;
  unsigned short LineLength, LineCount, StringPos;
  char bchar;
  bool InputText;
  unsigned long Flags;
};

#define FIELD_HIDDEN		0x0001
#define FIELD_READONLY		0x0002
#define FIELD_JUSTIFIED_RIGHT	0x0004
#define FIELD_LOWER_CASE        0x0008
#define FIELD_UPPER_CASE        0x0010
#define FIELD_AUTO_SKIP		0x0020
#define FIELD_NO_CLEAR		0x0040
#define FIELD_NO_MARGIN		0x0080

/**
    Field to read numeric values
*/
class XtInputNumericField : public XtInputStringField {
 public:

  /** Constructs a new XtInputNumericField.
      @param __r field bounds
      @param __DefaultNumber default number
      @param __IntegerDigits maximum integer digits allowed
      @param __DecimalDigits maximum decimal digits allowed
      @param __Flags options, which can be:
	 <p><b>FIELD_READONLY</b> - the content cannot be changed
	 <p><b>FIELD_JUSTIFIED_RIGHT</b> - the field is right-justified
         <p><b>ALLOW_NEGATIVE_VALUES</b> - to allow entering negative values
  */
  XtInputNumericField(XRect __r, double __DefaultNumber, unsigned char __IntegerDigits, unsigned char __DecimalDigits, unsigned long __Flags);

  /// Clears the value
  virtual void ClearValue();

  /// Returns the current value
  virtual double GetNumericValue();

  /// Process keyboard events
  virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);

  /// Sets the default value
  virtual void SetValue(const char *__DefaultString);

  /// Sets the default value
  virtual void SetValue(double __DefaultNumber);

 protected:
  unsigned short IntegerDigits, DecimalDigits;
};

#define ALLOW_NEGATIVE_VALUES	0x0010

class XtInputDateField : public XtInputStringField {
    public:
	XtInputDateField(XRect __r, unsigned char __DefaultDay, unsigned char __DefaultMonth, unsigned short __DefaultYear, unsigned long Flags);
	virtual unsigned long GetTime();
	virtual bool ProcessKeyboardEvent(XKeyboardEvent *__Event);
	virtual void SetValue(const char *__DefaultString);
	virtual void SetValue(unsigned char __DefaultDay, unsigned char __DefaultMonth, unsigned short __DefaultYear);
    protected:
	unsigned char CountrySetting;
	unsigned char DayPos, MonthPos, YearPos;
};

#define COUNTRY_GERMAN		0x05

// Palette entries
#define P_FIELD_LEN	6

#define P_FIELD_TEXT	0x01
#define P_FIELD_DEFAULT	0x03
#define P_FIELD_FOCUSED	0x05

// Default palette
#define P_FIELD 	"\x07\x00\x00\x06\x07\x00\x07\x00\x10\x06\x07\x00"

#endif
