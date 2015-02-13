/*
  RegionalInfo.cc

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

#include <RegionalInfo.h>

#include <stdlib.h>
#include <string.h>

RegionalInfo *Region;

RegionalInfo::RegionalInfo()
{
  SetRegion("");
}

RegionalInfo::RegionalInfo(char *__Region)
{
  if(!SetRegion(__Region))
    SetRegion("");
}

char *RegionalInfo::GetCurrencySymbol()
{
  return locale->currency_symbol;
}

char RegionalInfo::GetDecimalPoint()
{
  return locale->decimal_point[0];
}

char RegionalInfo::GetFracDigits()
{
  return locale->frac_digits;
}

char *RegionalInfo::GetGrouping()
{
  return locale->grouping;
}

bool RegionalInfo::GetNCsPrecedes()
{
  return (locale->n_cs_precedes == 1);
}

bool RegionalInfo::GetNSepBySpace()
{
  return (locale->n_sep_by_space == 1);
}

char RegionalInfo::GetNSignPosn()
{
  return locale->n_sign_posn;
}

char RegionalInfo::GetNegativeSign()
{
  return locale->negative_sign[0];
}

bool RegionalInfo::GetPCsPrecedes()
{
  return (locale->p_cs_precedes == 1);
}

bool RegionalInfo::GetPSepBySpace()
{
  return (locale->p_sep_by_space == 1);
}

char RegionalInfo::GetPSignPosn()
{
  return locale->p_sign_posn;
}

char RegionalInfo::GetPositiveSign()
{
  return locale->positive_sign[0];
}

char *RegionalInfo::GetRegion()
{
  return RegionName;
}

char RegionalInfo::GetThousandsSep()
{
  return locale->thousands_sep[0];
}

void RegionalInfo::SetCurrencySymbol(char *__s)
{
  strcpy(locale->currency_symbol, __s);
}

void RegionalInfo::SetDecimalPoint(char __c)
{
  locale->decimal_point[0] = __c;
}

void RegionalInfo::SetFracDigits(char __c)
{
  locale->frac_digits = __c;
}

void RegionalInfo::SetGrouping(char *__s)
{
  strcpy(locale->grouping, __s);
}

void RegionalInfo::SetNCsPrecedes(bool __b)
{
  locale->n_cs_precedes = (__b ? 1 : 0);
}

void RegionalInfo::SetNSepBySpace(bool __b)
{
  locale->n_sep_by_space = (__b ? 1 : 0);
}

void RegionalInfo::SetNSignPosn(char __c)
{
  locale->n_sign_posn = __c;
}

void RegionalInfo::SetNegativeSign(char __c)
{
  locale->negative_sign[0] = __c;
}

void RegionalInfo::SetPCsPrecedes(bool __b)
{
  locale->p_cs_precedes = (__b ? 1 : 0);
}

void RegionalInfo::SetPSepBySpace(bool __b)
{
  locale->p_sep_by_space = (__b ? 1 : 0);
}

void RegionalInfo::SetPSignPosn(char __c)
{
  locale->p_sign_posn = __c;
}

void RegionalInfo::SetPositiveSign(char __c)
{
  locale->positive_sign[0] = __c;
}

bool RegionalInfo::SetRegion(char *__Region)
{
  if(setlocale(LC_ALL, __Region) == 0)
    return false;
  locale = localeconv();
  if(strlen(__Region) == 0)
    if(getenv("LANG") == 0)
      strcpy(RegionName, "C");
     else
      strcpy(RegionName, getenv("LANG"));
   else
    strcpy(RegionName, __Region);
  return true;
}

void RegionalInfo::SetThousandsSep(char __c)
{
  locale->thousands_sep[0] = __c;
}
