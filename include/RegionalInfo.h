/*
  RegionalInfo.h
  Definitions for region specific definitions

  Copyright (c) 1999 Dragos Acostachioaie

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

#ifndef _REGIONAL_INFO_H
#define _REGIONAL_INFO_H

#include <locale.h>

/**
    @memo Class that abstractize the basic region specific definitions
*/  
class RegionalInfo
{
 public:
  /** Constructs a RegionalInfo object, using the locale specified by the
      `LANG' environment variable or the portable `C' locale is that is not set.
      @see system's locale(7) man page for further details
  */
  RegionalInfo();

  /** Constructs a RegionalInfo object, using the specified locale. If the
      specified locale cannot be set, the default constructor is used.
  */
  RegionalInfo(char *__Region);

  /// Returns the current currency symbol
  virtual char *GetCurrencySymbol();

  /// Returns the current decimal point character
  virtual char GetDecimalPoint();

  /// Returns the current fractional digits
  virtual char GetFracDigits();

  /// Returns the current numbers grouping method
  virtual char *GetGrouping();

  /// Returns the current currency symbol position for negative values
  virtual bool GetNCsPrecedes();

  /// Checks if it's a space between currency symbol and negative values
  virtual bool GetNSepBySpace();

  /// Returns the sign symbol position for negative values
  virtual char GetNSignPosn();

  /// Returns the symbol currently used for negative values
  virtual char GetNegativeSign();

  /// Returns the current currency symbol position for positive values
  virtual bool GetPCsPrecedes();

  /// Checks if it's a space between currency symbol and positive values
  virtual bool GetPSepBySpace();

  /// Returns the sign symbol position for positive values
  virtual char GetPSignPosn();

  /// Returns the symbol currently used for positive values
  virtual char GetPositiveSign();

  /// Returns the current region name
  virtual char *GetRegion();

  /// Returns the current thousands separator
  virtual char GetThousandsSep();

  /// Changes the current currency symbol
  virtual void SetCurrencySymbol(char *__s);

  /// Changes the current decimal point character
  virtual void SetDecimalPoint(char __c);

  /// Changes the current fractional digits
  virtual void SetFracDigits(char __c);

  /// Changes the current numbers grouping method
  virtual void SetGrouping(char *__s);

  /// Changes the currency symbol position for negative values
  virtual void SetNCsPrecedes(bool __b);

  /// Enables or disables the space between currency symbol and negative values
  virtual void SetNSepBySpace(bool __b);

  /// Changes the sign symbol position for negative values
  virtual void SetNSignPosn(char __c);

  /// Changes the symbol used for negative values
  virtual void SetNegativeSign(char __c);

  /// Changes the currency symbol position for positive values
  virtual void SetPCsPrecedes(bool __b);

  /// Enables or disables the space between currency symbol and positive values
  virtual void SetPSepBySpace(bool __b);

  /// Changes the sign symbol position for positive values
  virtual void SetPSignPosn(char __c);

  /// Changes the symbol used for positive values
  virtual void SetPositiveSign(char __c);

  /// Changes the region
  virtual bool SetRegion(char *__Region);

  /// Changes the thousands separator character
  virtual void SetThousandsSep(char __c);

 protected:

  /// Current region name
  char RegionName[80];

  /// Current locale
  lconv *locale;
};

/** Global RegionalInfo class instantiation.
    Initialized/destroyed normally by <i>XInit()</i> and <i>XDone()</i>
*/
extern RegionalInfo *Region;

#endif
