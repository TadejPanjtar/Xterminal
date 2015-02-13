/*
  xtfield.cc

  Copyright (c) 1996-2002 Dragos Acostachioaie
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

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <RegionalInfo.h>
#include <xtfield.h>

// Maximum string length
#define IF_MAXSTRLEN		1024

// Control chars
#define IF_ENCRYPT_CONTROL_CHAR	'*'
#define IF_LEFT_MARGIN		ACS_LARROW
#define IF_RIGHT_MARGIN		ACS_RARROW
#define IF_ELEFT_MARGIN		'['
#define IF_ERIGHT_MARGIN	']'
#define IF_EBACKGROUND_CHAR	'.'

XtInputStringField::XtInputStringField(XRect __r,
                                       const char *__DefaultString,
                                       unsigned __MaxLen,
                                       unsigned long __Flags) :
    XObject(__r)
{
  strcpy(ObjClass, "XtInputStringField");
  if(__MaxLen > IF_MAXSTRLEN)
    __MaxLen = IF_MAXSTRLEN;
  LineLength = Size.x - 2;
  LineCount = __r.b.y - __r.a.y + 1;
  FieldLength = LineLength * LineCount;
  MaxLen = __MaxLen;
  Flags = __Flags;
  SetPalette(P_FIELD, P_FIELD_LEN);
  if(Flags & FIELD_HIDDEN)
    {
    if(Flags & FIELD_READONLY)
      Flags &= ~FIELD_READONLY;
    if(Flags & FIELD_JUSTIFIED_RIGHT)
      Flags &= ~FIELD_JUSTIFIED_RIGHT;
    DefaultString = 0;
    MaxLen = FieldLength;
    StringLength = 0;
    CurrString = new char[MaxLen + 1];
    bzero(CurrString, MaxLen);
    bchar = IF_EBACKGROUND_CHAR;
    }
   else
    {
    if((Flags & FIELD_JUSTIFIED_RIGHT) && LineCount > 1)
      Flags &= ~FIELD_JUSTIFIED_RIGHT;
    DefaultString = new char[MaxLen + 1];
    strcpy(DefaultString, __DefaultString);
    StringLength = strlen(DefaultString);
    CurrString = new char[MaxLen + 1];
    strcpy(CurrString, DefaultString);
    bchar = ' ';
    }
  InputText = (Flags & FIELD_NO_CLEAR);
  CP = 1;
  if(Flags & FIELD_NO_MARGIN)
    curX = 0;
   else
    curX = 1;
  curY = 0;
  StringPos = 0;
  XObject::SetState(SM_ACTIVE, true);
}

XtInputStringField::~XtInputStringField()
{
    delete[] CurrString;
    delete[] DefaultString;
}

void XtInputStringField::ClearValue()
{
    CP = 1;
    if(Flags & FIELD_NO_MARGIN)
	curX = 0;
    else
	curX = 1;
    curY = 0;

    StringPos = 0;
    strcpy(DefaultString, "");
    StringLength = strlen(DefaultString);
    strcpy(CurrString, DefaultString);
    if(GetState(SM_VISIBLE))
	Draw();
}

void XtInputStringField::Draw()
{
  unsigned max;
  int i, just, cnt, l;

  XObject::SetState(SM_VISIBLE, true);
  Cursor = Bounds.a + XPoint(0, curY);
  if(Flags & FIELD_JUSTIFIED_RIGHT)
    {
    just = (FieldLength - StringLength);
    if(just < 0)
      just = 0;
    Cursor.x += just + curX;
    }
   else
    {
    just = 0;
    Cursor.x += curX;
    }
  if(GetState(SM_FOCUSED))
    XSetColor(GetPalEntry(P_FIELD_FOCUSED), GetPalEntry(P_FIELD_FOCUSED + 1));
   else
    XSetColor(GetPalEntry(P_FIELD_TEXT), GetPalEntry(P_FIELD_TEXT + 1));
  XFillWindow(Bounds, bchar);
  if(Flags & FIELD_HIDDEN)
    {
    XPrintChar(Bounds.a, IF_ELEFT_MARGIN);
    XPrintChar(XPoint(Bounds.b.x, Bounds.a.y), IF_ERIGHT_MARGIN);
    if(StringLength > 0)
      XFillWindow(XRect(Bounds.a.x + 1, Bounds.a.y, Bounds.a.x + StringLength, Bounds.a.y), IF_ENCRYPT_CONTROL_CHAR);
    XMoveCursor(Cursor);
    if(!GetState(SM_NO_REFRESH))
      XRefresh();
    return;
    }
  if(StringPos > 0)
    XPrintChar(Bounds.a, IF_LEFT_MARGIN);
  if(StringPos + FieldLength < StringLength)
    XPrintChar(Bounds.b, IF_RIGHT_MARGIN);
  if(GetState(SM_FOCUSED))
    if(!InputText)
      XSetColor(GetPalEntry(P_FIELD_DEFAULT), GetPalEntry(P_FIELD_DEFAULT + 1));
  l = 0;
  cnt = StringPos;
  if(StringLength - StringPos > FieldLength)
    max = FieldLength;
  else
    max = StringLength - StringPos;
  while(max > 0)
    {
    int j;
    if(max > LineLength)
      j = LineLength;
     else
      j = max;
    for(i = 0; i < j; i++)
      {
      if(Flags & FIELD_NO_MARGIN)
        XPrintChar(XPoint(Bounds.a.x + i + just, Bounds.a.y + l), CurrString[cnt]);
       else
        XPrintChar(XPoint(Bounds.a.x + i + just + 1, Bounds.a.y + l), CurrString[cnt]);
      cnt++;
      max--;
      }
    l++;
    }
  if(Flags & FIELD_NO_MARGIN)
    Cursor.x--;
  XMoveCursor(Cursor);
  if(!GetState(SM_NO_REFRESH))
    XRefresh();
}

const char *XtInputStringField::GetDefaultValue()
{
    return DefaultString;
}

short XtInputStringField::GetInputLength()
{
    return StringLength;
}

const char *XtInputStringField::GetValue()
{
    return CurrString;
}

bool XtInputStringField::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  unsigned key, i;
  bool oldtext = InputText;
  InputText = true;

  switch(__Event->KeyCode)
    {
    case KB_IC:
      if(!(Flags & FIELD_HIDDEN))
        {
	XSetInputState(TI_INSERT_MODE, !XGetInputState(TI_INSERT_MODE));
        XRefresh();
        return true;
        }
      break;
    case KB_LEFT:
      if(CP > 1 && !(Flags & FIELD_HIDDEN))
        {
        CP--;
        if(curX > 1)
          curX--;
         else
          if(curY > 0)
            {
            curX = LineLength;
            curY--;
            }
	   else
	    if(StringPos > 0)
	      StringPos--;
        Draw();
        return true;
	}
       else
	{
        if(Server && Server->GetState(SM_HANDLE_ARROWS))
    	    {
            XMessage *msg = new XMessage;
            msg->Type = MC_CLIENT_TO_SERVER;
            msg->Message = MT_BACKWARD_FOCUS;
            _PutEventInFront(msg);
            return true;
            }
        else
            {
	    XBeep();
	    XRefresh();
	    }
	}
      break;
    case KB_RIGHT:
      if(CP <= StringLength && !(Flags & FIELD_HIDDEN))
        {
        CP++;
        if(curX < LineLength)
          curX++;
         else
          if(LineCount > curY + 1)
            {
            curX = 1;
            curY++;
	    }
	  else
            if(CP <= StringLength)
	      StringPos++;
	     else
	      curX++;
        Draw();
        return true;
	}
       else
	{
        if(Server && Server->GetState(SM_HANDLE_ARROWS))
    	    {
            XMessage *msg = new XMessage;
            msg->Type = MC_CLIENT_TO_SERVER;
            msg->Message = MT_FORWARD_FOCUS;
            _PutEventInFront(msg);
            return true;
            }
        else
            {
	    XBeep();
	    XRefresh();
	    }
	}
      break;
    case KB_UP:
      if(curY > 0 && !(Flags & FIELD_HIDDEN))
        {
        curY--;
	CP -= LineLength;
        Draw();
        return true;
	}
       else
	{
        if(Server && Server->GetState(SM_HANDLE_ARROWS))
    	    {
            XMessage *msg = new XMessage;
            msg->Type = MC_CLIENT_TO_SERVER;
            msg->Message = MT_BACKWARD_FOCUS;
            _PutEventInFront(msg);
            return true;
            }
          else
            {
	    XBeep();
	    XRefresh();
	    }
	}
      break;
    case KB_DOWN:
      if(LineCount > curY + 1 && !(Flags & FIELD_HIDDEN))
        {
  	if(CP + LineLength > StringLength)
	  {
          CP = StringLength + 1;
	  if(StringLength > (curY + 1) * LineLength)
              curY++;
	  curX = StringLength - curY * LineLength + 1;
	  }
        else
          {
          CP += LineLength;
          curY++;
          }
        Draw();
        return true;
	}
       else
	{
        if(Server && Server->GetState(SM_HANDLE_ARROWS))
    	    {
            XMessage *msg = new XMessage;
            msg->Type = MC_CLIENT_TO_SERVER;
            msg->Message = MT_FORWARD_FOCUS;
            _PutEventInFront(msg);
            return true;
	    }
	else
	    {
	    XBeep();
	    XRefresh();
	    }
	}
      break;
    case KB_HOME:
      if(StringLength > 0 && !(Flags & FIELD_HIDDEN))
        {
        CP = 1;
        curX = 1;
	curY = 0;
	StringPos = 0;
        Draw();
        return true;
        }
      break;
    case KB_END:
	if(CP != StringLength + 1)
	    {
	    if(Flags & FIELD_HIDDEN)
		{
		CP = StringLength + 1;
		curX = StringLength + 1;
		}
	    else
		{
		CP = StringLength + 1;
		if(StringLength > LineLength)
		    if(StringLength < FieldLength)
			{
			curY = StringLength / LineLength;
			curX = StringLength - curY * LineLength + 1;
			}
		    else
        		{
        		curX = LineLength + 1;
			curY = LineCount - 1;
			StringPos = StringLength - FieldLength;
        		}
		else
        	    {
        	    curX = StringLength + 1;
		    curY = 0;
		    }
		}
	    Draw();
	    return true;
	    }
	break;
    case KB_BACK:
    case KB_BACKSPACE:
      if(!(Flags & FIELD_READONLY) && CP > 1)
        {
        for(i = CP - 1; i <= StringLength - 1; i++)
          CurrString[i - 1] = CurrString[i];
        CurrString[StringLength - 1] = '\0';
        if(curX > 1)
          curX--;
         else
          {
	  if(curY > 0)
            {
	    curX = LineLength;
	    curY--;
            }
	   else
	    if(StringPos > 0)
	      StringPos--;
	  }
	CP--;
        StringLength--;
        Draw();
        return true;
        }
       else
	{
	XBeep();
        XRefresh();
	}
      break;
    case KB_DC:
      if(!(Flags & FIELD_READONLY) && CP <= StringLength)
        {
        for(i = CP; i <= StringLength - 1; i++)
          CurrString[i - 1] = CurrString[i];
        CurrString[StringLength - 1] = '\0';
	StringLength--;
        Draw();
        return true;
	}
       else
	{
	XBeep();
        XRefresh();
	}
      break;
    case KB_ENTER:
    case KB_ESC:
      Draw();
      return true;
    default:
      key = __Event->KeyCode;
      if(key >= 0x20 && key <= 0xff &&
        (!oldtext ||
	  (!XGetInputState(TI_INSERT_MODE) && CP <= MaxLen) ||
	  (XGetInputState(TI_INSERT_MODE) && StringLength < MaxLen)) &&
	!(Flags & FIELD_READONLY))
        {
        if(!oldtext)
          {
          StringLength = 0;
	  bzero(CurrString, MaxLen);
          }
	if(Flags & FIELD_LOWER_CASE)
	  key = tolower(key);
	if(Flags & FIELD_UPPER_CASE)
	  key = toupper(key);
        if(CP <= StringLength)
          if(XGetInputState(TI_INSERT_MODE))
	    {
	    for(i = StringLength; i >= CP; i--)
	      CurrString[i] = CurrString[i - 1];
	    CurrString[CP - 1] = key;
	    CurrString[StringLength + 1] = '\0';
	    StringLength++;
            if(StringLength > FieldLength && (CP - StringPos) > FieldLength)
	      StringPos++;
            }
           else
	    CurrString[CP - 1] = key;
         else
	  {
	  CurrString[StringLength] = key;
	  CurrString[StringLength + 1] = '\0';
	  StringLength++;
          if(StringLength > FieldLength && (CP - StringPos) > FieldLength)
	    StringPos++;
	  }
	CP++;
	if(curX <= LineLength)
	  curX++;
	 else
	  if(LineCount > curY + 1)
	    {
	    curX = 2;
	    curY++;
	    }
        Draw();
        if(Flags & FIELD_AUTO_SKIP && StringLength == FieldLength)
    	    {
            XMessage *msg = new XMessage;
            msg->Type = MC_CLIENT_TO_SERVER;
            msg->Message = MT_FORWARD_FOCUS;
            _PutEventInFront(msg);
    	    }
	return true;
        }
      XBeep();
      XRefresh();
      return true;
    }
  return false;
}

bool XtInputStringField::ProcessMessage(XMessage *__Event)
{
  XMessage *msg;
  switch(__Event->Message)
    {
    case MT_RELEASE_FOCUS:
      CP = 1;
      curX = 1;
      curY = 0;
      StringPos = 0;
      if(!(Flags & FIELD_HIDDEN))
        strcpy(DefaultString, CurrString);
      XSetOutputState(TO_CURSOR_VISIBLE, GetState(SM_CURSOR_VISIBLE));
      SetState(SM_FOCUSED, false);
      return true;
    case MT_RESUME_FOCUS:
      if(Flags & FIELD_HIDDEN)
        {
        InputText = true;
	CP = curX = StringLength + 1;
	curY = 0;
	}
       else
        {
        InputText = (Flags & FIELD_NO_CLEAR);
        CP = 1;
        curX = 1;
        curY = 0;
	StringPos = 0;
	}
      XObject::SetState(SM_CURSOR_VISIBLE, XGetOutputState(TO_CURSOR_VISIBLE));
      if(XGetInputState(TI_INSERT_MODE))
        {
        XSetOutputState(TO_CURSOR_VISIBLE, true);
        XSetOutputState(TO_CURSOR_VERY_VISIBLE, false);
	}
       else
        {
        XSetOutputState(TO_CURSOR_VISIBLE, true);
        XSetOutputState(TO_CURSOR_VERY_VISIBLE, true);
	}
      SetState(SM_FOCUSED, true);
      return true;
    case MT_ANSWER_REQUEST:
      msg = new XMessage;
      msg->Type = MC_CLIENT_TO_SERVER;
      msg->Body = new char[strlen(CurrString) + 1];
      strcpy((char *)msg->Body, CurrString);
      msg->Size = StringLength;
      _PutEventInFront(msg);
      return true;
    }
  return false;
}

bool XtInputStringField::ProcessMouseEvent(XMouseEvent *__Event)
{
  XKeyboardEvent *ev;
  XMessage *msg;
  int just;
  if(__Event->Type == EV_MOUSEDOWN)
    if(Bounds.Contains(__Event->Position))
      {
      if(!GetState(SM_FOCUSED))
        {
        msg = new XMessage;
        msg->Type = MC_CLIENT_TO_SERVER;
        msg->Message = MT_RELEASE_FOCUS;
        if(Server)
  	  Server->ProcessMessage(msg);
        delete msg;
        msg = new XMessage;
        msg->Message = MT_RESUME_FOCUS;
        ProcessMessage(msg);
        delete msg;
	}
      if(Flags & FIELD_JUSTIFIED_RIGHT)
        {
        just = (FieldLength - StringLength);
        if(just < 0)
         just = 0;
        }
       else
        just = 0;
      CP = __Event->Position.x - Bounds.a.x + ((__Event->Position.y - Bounds.a.y) * LineLength) - just;
      CP += StringPos;
      if(CP > StringLength || CP < 1 || (Flags & FIELD_HIDDEN))
        {
	ev = new XKeyboardEvent;
        ev->KeyCode = KB_END;
	ProcessKeyboardEvent(ev);
	delete ev;
	}
       else
        {
	curX = __Event->Position.x - Bounds.a.x;
	curY = __Event->Position.y - Bounds.a.y;
	}
      InputText = true;
      Draw();
      return true;
      }
  return false;
}

void XtInputStringField::SetState(unsigned long __Mask, bool __BoolValue)
{
  XObject::SetState(__Mask, __BoolValue);
  if(GetState(SM_VISIBLE))
    Draw();
}

void XtInputStringField::SetValue(const char *__DefaultString)
{
    if(strlen(__DefaultString) > MaxLen)
	return;

  if(!GetState(SM_FOCUSED) && !(Flags & FIELD_HIDDEN))
    {
    strcpy(DefaultString, __DefaultString);
    strcpy(CurrString, DefaultString);
    StringLength = strlen(CurrString);
    if(GetState(SM_VISIBLE))
      Draw();
    }
}

XtInputNumericField::XtInputNumericField(XRect __r,
					 double __DefaultNumber,
					 unsigned char __IntegerDigits,
					 unsigned char __DecimalDigits,
					 unsigned long __Flags) :
  XtInputStringField(__r,
                     "",
		     __IntegerDigits + __DecimalDigits + (__DecimalDigits ? 1 : 0) + (__Flags & ALLOW_NEGATIVE_VALUES ? 1 : 0),
		     __Flags)
{
  IntegerDigits = __IntegerDigits;
  DecimalDigits = __DecimalDigits;
  SetValue(__DefaultNumber);
}

void XtInputNumericField::ClearValue()
{
    strcpy(DefaultString, "0");
    StringLength = strlen(DefaultString);
    strcpy(CurrString, DefaultString);
    if(GetState(SM_VISIBLE))
	Draw();
}    

double XtInputNumericField::GetNumericValue()
{
    return atof(CurrString);
}

bool XtInputNumericField::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
  int i, temp;
  char decsep;

  decsep = Region->GetDecimalPoint();
  switch(__Event->KeyCode)
    {
    case '-':
      if(!(Flags & ALLOW_NEGATIVE_VALUES) || (Flags & FIELD_READONLY) || CP > 1)
	{
	XBeep();
	XRefresh();
	return true;
	}
      break;
    case KB_IC:
    case KB_LEFT:
    case KB_RIGHT:
    case KB_HOME:
    case KB_END:
    case KB_ENTER:
    case KB_UP:
    case KB_DOWN:
    case KB_ESC:
      break;
    case KB_BACK:
    case KB_BACKSPACE:
      if(CP == 1 || (CurrString[CP - 2] == decsep && IntegerDigits + (CurrString[0] == '-' ? 1 : 0) < StringLength - 1))
        {
        XBeep();
        XRefresh();
	return true;
	}
      break;
    case KB_DC:
      if(CurrString[CP - 1] == decsep && IntegerDigits + (CurrString[0] == '-' ? 1 : 0) < StringLength - 1)
        {
        XBeep();
        XRefresh();
	return true;
	}
      break;
    default:
      if(__Event->KeyCode == decsep)
        {
        if(!DecimalDigits || (Flags & FIELD_READONLY))
          {
          XBeep();
          XRefresh();
          return true;
          }
        if((temp = strchr(CurrString, decsep) - CurrString + 1) > 0)
          {
          if(CP >= temp)
            {
            XBeep();
            XRefresh();
            return true;
            }
          temp -= CP;
          for(i = 0; i < temp; i++)
            {
            __Event->KeyCode = KB_RIGHT;
            XtInputStringField::ProcessKeyboardEvent(__Event);
            }
          return true;
          }
        if(CP > IntegerDigits + 1 + (CurrString[0] == '-' ? 1 : 0))
          {
          XBeep();
          XRefresh();
          return true;
          }
        break;
        }
      else
       {
       if(__Event->KeyCode >= '0' &&
         __Event->KeyCode <= '9' &&
	 (!InputText ||
	   (!XGetInputState(TI_INSERT_MODE) && CP <= MaxLen) ||
	   (XGetInputState(TI_INSERT_MODE) && StringLength < MaxLen)) &&
	 !(Flags & FIELD_READONLY))
         {
         temp = strchr(CurrString, decsep) - CurrString + 1;
         if(temp < 0)
           temp = 0;
         if(!XGetInputState(TI_INSERT_MODE))
           if(temp && CP == temp)
             {
             XBeep();
             XRefresh();
             return true;
             }
            else
             if(CP <= StringLength)
               break;
         if(!InputText || !(temp > 0 && ((CP > temp && StringLength - temp >= DecimalDigits) || (temp > 0 && CP <= temp && IntegerDigits + (CurrString[0] == '-' ? 1 : 0) <= temp - 1))) && !(temp == 0 && IntegerDigits + (CurrString[0] == '-' ? 1 : 0) <= StringLength))
           break;
         }
       XBeep();
       XRefresh();
       return true;
       }
    }
  return XtInputStringField::ProcessKeyboardEvent(__Event);
}

void XtInputNumericField::SetValue(const char *__DefaultString)
{
  SetValue(strtod(__DefaultString, 0));
}

void XtInputNumericField::SetValue(double __DefaultNumber)
{
  int id, dd;
  int decsep;
  char *ind;
  char f[IF_MAXSTRLEN], *s = new char[IF_MAXSTRLEN + 1];

  if(!GetState(SM_FOCUSED))
    {
    decsep = Region->GetDecimalPoint();
    if(__DefaultNumber < 0 && !(Flags & ALLOW_NEGATIVE_VALUES))
      __DefaultNumber = 0;
    sprintf(s, "%f", __DefaultNumber);
    dd = strchr(s, decsep) - s + 1;
    dd = strlen(s) - dd;
    id = strlen(s) - dd - 1;
    delete s;
    if(floor(__DefaultNumber) == __DefaultNumber)
      dd = 0;
    if(dd > DecimalDigits)
      dd = DecimalDigits;
    if(id > IntegerDigits)
      {
      id = 1;
      dd = 0;
      __DefaultNumber = 0;
      }
    sprintf(f, "%%%d.%df", id, dd);
    sprintf(DefaultString, f, __DefaultNumber);
    ind = rindex(DefaultString, '.');
    if(ind > 0)
      *ind = decsep;
    StringLength = strlen(DefaultString);
    strcpy(CurrString, DefaultString);
    if(GetState(SM_VISIBLE))
      Draw();
    }
}

XtInputDateField::XtInputDateField(XRect __r,
				   unsigned char __DefaultDay,
				   unsigned char __DefaultMonth,
				   unsigned short __DefaultYear,
				   unsigned long __Flags) :

    XtInputStringField(__r,
		       "",
		       10,
		       __Flags)
{
    strcpy(ObjClass, "XtInputDateField");
    CountrySetting = COUNTRY_GERMAN;
    SetValue(__DefaultDay, __DefaultMonth, __DefaultYear);
}

unsigned long XtInputDateField::GetTime()
{
    int Day, Month, Year;
    struct tm t;

    if(sscanf(CurrString, "%2d.%2d.%4d", &Day, &Month, &Year) == 3)
	{
	t.tm_year = Year - 1900;
	t.tm_mon = Month - 1;
	t.tm_mday = Day;
	t.tm_hour = 0;
	t.tm_min = 0;
	t.tm_sec = 0;
	t.tm_isdst = -1;
	return mktime(&t);
	}

  return 0;
}

bool XtInputDateField::ProcessKeyboardEvent(XKeyboardEvent *__Event)
{
    bool oldinput, oldtext = InputText;
    InputText = true;

    switch(__Event->KeyCode)
	{
	case KB_HOME:
	case KB_END:
	case KB_ENTER:
	case KB_ESC:
	    break;
	case KB_IC:
	    return true;
	case KB_LEFT:
	case KB_RIGHT:
	    if(XtInputStringField::ProcessKeyboardEvent(__Event) &&
		CurrString[CP - 1] == '.')
		XtInputStringField::ProcessKeyboardEvent(__Event);
	    return true;
	case KB_BACK:
	case KB_BACKSPACE:
	    if(!(Flags & FIELD_READONLY) && CP > 1)
		{
		if(CurrString[CP - 2] == '.')
		    {
		    __Event->KeyCode = KB_LEFT;
		    XtInputStringField::ProcessKeyboardEvent(__Event);
		    }
		if(CP - 1 > YearPos)
		    {
		    __Event->KeyCode = KB_BACKSPACE;
		    XtInputStringField::ProcessKeyboardEvent(__Event);
		    }
		else
		    {
		    __Event->KeyCode = KB_LEFT;
		    XtInputStringField::ProcessKeyboardEvent(__Event);
		    CurrString[CP - 1] = ' ';
		    Draw();
		    }
		}
	    return true;
	case KB_DC:
	    if(!(Flags & FIELD_READONLY) && CP <= StringLength)
		{
		CurrString[CP - 1] = ' ';
		Draw();
		}
	    return true;
	default:
	    if(__Event->KeyCode >= '0' && __Event->KeyCode <= '9' &&
		!(Flags & FIELD_READONLY))
		{
		if(!oldtext)
		SetValue(0, 0, 0);
		oldinput = XGetInputState(TI_INSERT_MODE);
		XSetInputState(TI_INSERT_MODE, false);
		if(XtInputStringField::ProcessKeyboardEvent(__Event) &&
		    CurrString[CP - 1] == '.')
		    {
		    __Event->KeyCode = KB_RIGHT;
		    XtInputStringField::ProcessKeyboardEvent(__Event);
		    }
		XSetInputState(TI_INSERT_MODE, oldinput);
		}
	    else
		{
		XBeep();
		XRefresh();
		}
	    return true;
	}

    return XtInputStringField::ProcessKeyboardEvent(__Event);
}

void XtInputDateField::SetValue(const char *__DefaultString)
{
    int Day, Month, Year;

    if(sscanf(__DefaultString, "%2d.%2d.%4d", &Day, &Month, &Year) == 3)
	SetValue(Day, Month, Year);
}

void XtInputDateField::SetValue(unsigned char __DefaultDay, unsigned char __DefaultMonth, unsigned short __DefaultYear)
{
    char Day[80], Month[80], Year[80];

    if(!GetState(SM_FOCUSED))
	{
	if(__DefaultDay < 1 || __DefaultDay > 31 ||
	    __DefaultMonth < 1 || __DefaultMonth > 12)
	    {
	    sprintf(Day, "  ");
	    sprintf(Month, "  ");
	    sprintf(Year, "    ");
	    }
	else
	    {
	    sprintf(Day, "%02d", __DefaultDay);
	    sprintf(Month, "%02d", __DefaultMonth);
	    sprintf(Year, "%04d", __DefaultYear);
	    }
	switch(CountrySetting)
	    {
	    case COUNTRY_GERMAN:
		DayPos = 0;
		MonthPos = 3;
		YearPos = 5;
		sprintf(DefaultString, "%s.%s.%s", Day, Month, Year);
		break;
	    }
	StringLength = strlen(DefaultString);
	strcpy(CurrString, DefaultString);
	if(GetState(SM_VISIBLE))
	    Draw();
	}
}
