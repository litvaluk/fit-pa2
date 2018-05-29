#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
using namespace std;
class InvalidDateException
{
};
#endif /* __PROGTEST__ */

// uncomment if your code implements the overloaded suffix operators
// #define TEST_LITERALS

struct TAddition {
  int m_days = 0;
  int m_months = 0;
  int m_years = 0;

  vector<TAddition> operator + (TAddition addition) const {
    vector<TAddition> vect;
    vect.push_back(*this);
    vect.push_back(addition);
    return vect;
  }

  vector<TAddition> operator - (TAddition addition) const {
    addition.m_days = -addition.m_days;
    addition.m_months = -addition.m_months;
    addition.m_years = -addition.m_years;
    vector<TAddition> vect;
    vect.push_back(*this);
    vect.push_back(addition);
    return vect;
  }

  TAddition operator - () const {
    TAddition tmp;
    tmp.m_years = -this->m_years;
    tmp.m_months = -this->m_months;
    tmp.m_days = -this->m_days;
    return tmp;
  }

};

TAddition Year(int x){
  TAddition tmp;
  tmp.m_years = x;
  return tmp;
}

TAddition Month(int x){
  TAddition tmp;
  tmp.m_months = x;
  return tmp;
}

TAddition Day(int x){
  TAddition tmp;
  tmp.m_days = x;
  return tmp;
}

class CDate {
  // -----------------------------------------------------------------------------------------------------
  // -------------------------------------------PUBLIC----------------------------------------------------
  // -----------------------------------------------------------------------------------------------------
  public:
    // constructor
    CDate(int year, int month, int day) {
      if(!isValid(year, month, day)){
        throw InvalidDateException();
      }
      m_year = year;
      m_month = month;
      m_day = day;
    }

    ~CDate() {};

    // operator +
    CDate operator + (const TAddition& addition) const {
      int y, m, d;
      y = m_year;
      m = m_month;
      d = m_day;

      if(addition.m_days < 0 || addition.m_months < 0 || addition.m_years < 0){
        CDate tmp(y, m, d);
        TAddition tmpAddition;
        tmpAddition.m_years = abs(addition.m_years);
        tmpAddition.m_months = abs(addition.m_months);
        tmpAddition.m_days = abs(addition.m_days);
        return tmp - tmpAddition;
      }

      y += addition.m_years;

      int mon = addition.m_months;
      while (mon > 12) {
        y++;
        mon -= 12;
      }
      while (mon > 0) {
        m++;
        if (m == 13) {
          y++;
          m = 1;
        }
        mon--;
      }

      int dy = addition.m_days;

      while (dy > daysInMonth(m, y)){
        dy -= daysInMonth(m, y);
        m++;
        if (m == 13) {
          m = 1;
          y++;
        }
      }
      while(dy > 0){
        dy--;
        d++;
        if (d == daysInMonth(m, y) + 1) {
          m++;
          d = 1;
          if (m == 13) {
            m = 1;
            y++;
          }
        }
      }

      CDate tmp(y, m, d);
      return tmp;
    }

    // operator -
    CDate operator - (const TAddition& subtraction) const {
      int y, m, d;
      y = m_year;
      m = m_month;
      d = m_day;

      if(subtraction.m_days < 0 || subtraction.m_months < 0 || subtraction.m_years < 0){
        CDate tmp(y, m, d);
        TAddition tmpAddition;
        tmpAddition.m_years = abs(subtraction.m_years);
        tmpAddition.m_months = abs(subtraction.m_months);
        tmpAddition.m_days = abs(subtraction.m_days);
        return tmp + tmpAddition;
      }

      y -= subtraction.m_years;

      int mon = subtraction.m_months;
      while (mon > 12) {
        y--;
        mon -= 12;
      }
      while (mon > 0) {
        m--;
        if (m == 0) {
          y--;
          m = 12;
        }
        mon--;
      }

      int dy = subtraction.m_days;
      while (dy > daysInMonth(m == 1 ? 12 : m-1, m == 1 ? y-1 : y)) {
        dy -= daysInMonth(m == 1 ? 12 : m-1, m == 1 ? y-1 : y);
        m--;
        if (m == 0) {
          m = 12;
          y--;
        }
      }
      while (dy > 0) {
        dy--;
        d--;
        if (d == 0) {
          d = daysInMonth(m == 1 ? 12 : m-1, m == 1 ? y-1 : y);
          m--;
          if (m == 0) {
            m = 12;
            y--;
          }
        }
      }

      CDate tmp(y, m, d);
      return tmp;
    }

    // operator - date x date
    int operator - (const CDate& rdate) const {

      int targetYear = 0;
      int targetMonth = 0;
      int targetDay = 0;
      int year = 0;
      int month = 0;
      int day = 0;

      int dayCount = 0;

      if (*this < rdate) {
        targetYear = rdate.m_year;
        targetMonth = rdate.m_month;
        targetDay = rdate.m_day;
        year = this->m_year;
        month = this->m_month;
        day = this->m_day;
      }
      else {
        targetYear = this->m_year;
        targetMonth = this->m_month;
        targetDay = this->m_day;
        year = rdate.m_year;
        month = rdate.m_month;
        day = rdate.m_day;
      }

      if(year != targetYear){
        int newYear = year + 1;
        while (year != newYear) {
          day++;
          dayCount++;
          if (day > daysInMonth(month, year)) {
            month++;
            day = 1;
          }
          if (month > 12) {
            year++;
            month = 1;
          }
        }
      }

      while (year != targetYear) {
        dayCount += isLeap(year) ? 366 : 365;
        year++;
      }

      while (day != targetDay || month != targetMonth) {
        day++;
        dayCount++;
        if (day > daysInMonth(month, year)) {
          month++;
          day = 1;
        }
      }

      return rdate < *this ? dayCount : -dayCount;
    }

    // operator ==
    bool operator == (const CDate& rdate) const {
      if (this->m_day != rdate.m_day || this->m_month != rdate.m_month || this->m_year != rdate.m_year) {
        return false;
      }
      return true;
    }

    // operator !=
    bool operator != (const CDate& rdate) const {
      if (this->m_day == rdate.m_day && this->m_month == rdate.m_month && this->m_year == rdate.m_year) {
        return false;
      }
      return true;
    }

    // operator <
    bool operator < (const CDate& rdate) const {
      if (this->m_year > rdate.m_year) {
        return false;
      }
      if (this->m_year < rdate.m_year) {
        return true;
      }
      if (this->m_year == rdate.m_year) {
        if (this->m_month > rdate.m_month) {
          return false;
        }
        if (this->m_month < rdate.m_month) {
          return true;
        }
        if (this->m_month == rdate.m_month) {
          if (this->m_day > rdate.m_day) {
            return false;
          }
          if (this->m_day < rdate.m_day) {
            return true;
          }
          if (this->m_day == rdate.m_day) {
            return false;
          }
        }
      }
      return false;
    }

    // operator +=
    CDate& operator += (const vector<TAddition>& vect) {
      for (size_t i = 0; i < vect.size(); i++) {
        *this = *this + vect[i];
      }
      return *this;
    }

    CDate& operator += (const TAddition& add) {
      //cout << *this + add << endl;
      *this = *this + add;
      return *this;
    }


    // operator <<
    friend ostream& operator << (ostream& os, const CDate& date) {
      return os << date.m_year << "-" << setfill('0') << setw(2) << date.m_month
                << "-" << setfill('0') << setw(2) << date.m_day;
    }

  // -----------------------------------------------------------------------------------------------------
  // ------------------------------------------PRIVATE----------------------------------------------------
  // -----------------------------------------------------------------------------------------------------
  private:

    int m_year, m_month, m_day;

    bool isLeap(int y) const {
    	if(y%4000 == 0)
    		return false;
    	else if(y%400 == 0)
    		return true;
    	else if(y%100 == 0)
    		return false;
    	else if(y%4 == 0)
    		return true;
    	else
    		return false;
    }

    int daysInMonth(int m, int y) const {
    	if(m == 2){
    		if(isLeap(y))
    			return 29;
    		else
    			return 28;
    	}
    	else if(m == 4 || m == 6 || m == 9 || m == 11)
    		return 30;
    	else
    		return 31;
    }

    bool isValid(int year, int month, int day) {
      if (year < 1600) {
        return false;
      }
      if (month < 1 || month > 12) {
        return false;
      }
      if (day < 1 || day > daysInMonth(month, year)) {
        return false;
      }
      return true;
    }

};

vector<TAddition> operator + (vector<TAddition> vect, TAddition add) {
  vect.push_back(add);
  return vect;
}

vector<TAddition> operator - (vector<TAddition> vect, TAddition add) {
  add.m_days = -add.m_days;
  add.m_months = -add.m_months;
  add.m_years = -add.m_years;
  vect.push_back(add);
  return vect;
}

#ifndef __PROGTEST__
string             toString                                ( const CDate     & x )
{
  ostringstream oss;
  oss << x;
  return oss . str ();
}

int                main                                    ( void )
{
  CDate tmp ( 2000, 1, 1 );

  assert ( toString ( CDate ( 2000, 1, 1 ) )  == "2000-01-01" );
  assert ( toString ( CDate ( 2500, 12, 21 ) )  == "2500-12-21" );
  assert ( toString ( CDate ( 1685, 7, 11 ) )  == "1685-07-11" );
  try
  {
    tmp = CDate ( 1900, 2, 29 );
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  assert ( toString ( CDate ( 2000, 2, 29 ) )  == "2000-02-29" );
  assert ( toString ( CDate ( 2004, 2, 29 ) )  == "2004-02-29" );
  try
  {
    tmp = CDate ( 4000, 2, 29 );
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  assert ( toString ( CDate ( 2018, 3, 15 ) + Year ( 1 ) )  == "2019-03-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + Year ( 7 ) )  == "2025-03-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + Year ( 73 ) )  == "2091-03-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + Month ( 3 ) )  == "2018-06-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + Month ( 1 ) )  == "2018-04-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + Month ( 285 ) )  == "2041-12-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + Day ( 1 ) )  == "2018-03-16" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + Day ( 12 ) )  == "2018-03-27" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + Day ( 1234567 ) )  == "5398-05-02" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + Day ( 3 ) + Year ( 2 ) + Month ( 3 ) + Day ( 5 ) + Month ( 11 ) )  == "2021-05-23" );
  try
  {
    tmp = CDate ( 2000, 2, 29 ) + Year ( 300 );
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  try
  {
    tmp = CDate ( 2000, 3, 30 ) + Month ( 11 );
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  assert ( toString ( CDate ( 2001, 2, 20 ) + Day ( 9 ) )  == "2001-03-01" );
  assert ( toString ( CDate ( 2000, 1, 1 ) + Day ( 28 ) + Month ( 1 ) )  == "2000-02-29" );
  assert ( toString ( CDate ( 1999, 1, 1 ) + Year ( 1 ) + Day ( 28 ) + Month ( 1 ) )  == "2000-02-29" );
  assert ( toString ( CDate ( 2001, 1, 1 ) + Day ( 1095 ) + Day ( 28 ) + Month ( 1 ) )  == "2004-02-29" );
  try
  {
    tmp = CDate ( 2001, 1, 1 ) + Day ( 28 ) + Month ( 1 ) + Day ( 1095 );
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  try
  {
    tmp = CDate ( 2000, 1, 1 ) + Day ( 28 ) + Month ( 1 ) + Year ( 1 );
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  tmp = CDate ( 2000, 1, 1 );
  assert ( toString ( tmp - Year ( 5 ) + Month ( 2 ) )  == "1995-03-01" );
  assert ( toString ( tmp ) == "2000-01-01" );
  assert ( toString ( CDate ( 2000, 1, 1 ) - Year ( 1 ) - Month ( 3 ) - Day ( 10 ) )  == "1998-09-21" );
  assert ( toString ( CDate ( 2000, 1, 1 ) - Year ( 2 ) - Month ( -3 ) + Day ( -10 ) )  == "1998-03-22" );
  assert ( toString ( CDate ( 2000, 1, 1 ) + Day ( 59 ) - Month ( 1 ) - Year ( 2 ) )  == "1998-01-29" );
  try
  {
    tmp = CDate ( 2000, 1, 1 ) + Day ( 59 ) - Year ( 2 ) - Month ( 1 );
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  tmp = CDate ( 2000, 1, 1 );
  tmp +=  - Year ( 2 ) - Month ( -3 ) + Day ( -10 );
  assert ( toString ( tmp ) == "1998-03-22" );
  tmp = CDate ( 2000, 1, 1 );
  tmp += Day ( 59 ) - Month ( 1 ) - Year ( 2 );
  assert ( toString ( tmp ) == "1998-01-29" );
  try
  {
    tmp = CDate ( 2000, 1, 1 );
    tmp += Day ( 59 ) - Year ( 2 ) - Month ( 1 );
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  tmp = CDate ( 2018, 3, 15 ) + Day ( -3 );
  assert ( toString ( tmp ) == "2018-03-12" );
  assert ( !( CDate ( 2018, 3, 15 ) == CDate ( 2000, 1, 1 ) ) );
  assert ( CDate ( 2018, 3, 15 ) != CDate ( 2000, 1, 1 ) );
  assert ( !( CDate ( 2018, 3, 15 ) < CDate ( 2000, 1, 1 ) ) );
  assert ( CDate ( 2018, 3, 15 ) == CDate ( 2018, 3, 15 ) );
  assert ( !( CDate ( 2018, 3, 15 ) != CDate ( 2018, 3, 15 ) ) );
  assert ( !( CDate ( 2018, 3, 15 ) < CDate ( 2018, 3, 15 ) ) );
  assert ( !( CDate ( 2018, 3, 15 ) + Day ( 1 ) == CDate ( 2018, 3, 15 ) ) );
  assert ( CDate ( 2018, 3, 15 ) + Day ( 1 ) != CDate ( 2018, 3, 15 ) );
  assert ( !( CDate ( 2018, 3, 15 ) + Day ( 1 ) < CDate ( 2018, 3, 15 ) ) );
  assert ( !( CDate ( 2018, 3, 15 ) + Day ( -1 ) == CDate ( 2018, 3, 15 ) ) );
  assert ( CDate ( 2018, 3, 15 ) + Day ( -1 ) != CDate ( 2018, 3, 15 ) );
  assert ( CDate ( 2018, 3, 15 ) + Day ( -1 ) < CDate ( 2018, 3, 15 ) );
  assert ( CDate ( 2018, 3, 15 ) - CDate ( 2000, 1, 1 ) == 6648 );
  assert ( CDate ( 2000, 1, 1 ) - CDate ( 2018, 3, 15 ) == -6648 );
  assert ( CDate ( 2018, 3, 15 ) + Year ( 3 ) + Month ( -18 ) - CDate ( 2000, 1, 1 ) == 7197 );
  assert ( CDate ( 5398, 5, 2 ) - CDate ( 2018, 3, 15 ) == 1234567 );
#ifdef TEST_LITERALS
  assert ( toString ( CDate ( 2000, 1, 1 ) )  == "2000-01-01" );
  assert ( toString ( CDate ( 2500, 12, 21 ) )  == "2500-12-21" );
  assert ( toString ( CDate ( 1685, 7, 11 ) )  == "1685-07-11" );
  try
  {
    tmp = CDate ( 1900, 2, 29 );
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  assert ( toString ( CDate ( 2000, 2, 29 ) )  == "2000-02-29" );
  assert ( toString ( CDate ( 2004, 2, 29 ) )  == "2004-02-29" );
  try
  {
    tmp = CDate ( 4000, 2, 29 );
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  assert ( toString ( CDate ( 2018, 3, 15 ) + 1_year )  == "2019-03-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + 7_years )  == "2025-03-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + 73_years )  == "2091-03-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + 3_months )  == "2018-06-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + 1_month )  == "2018-04-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + 285_months )  == "2041-12-15" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + 1_day )  == "2018-03-16" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + 12_days )  == "2018-03-27" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + 1234567_days )  == "5398-05-02" );
  assert ( toString ( CDate ( 2018, 3, 15 ) + 3_days + 2_years + 3_months + 5_days + 11_months )  == "2021-05-23" );
  try
  {
    tmp = CDate ( 2000, 2, 29 ) + 300_years;
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  try
  {
    tmp = CDate ( 2000, 3, 30 ) + 11_months;
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  assert ( toString ( CDate ( 2001, 2, 20 ) + 9_days )  == "2001-03-01" );
  assert ( toString ( CDate ( 2000, 1, 1 ) + 28_days + 1_month )  == "2000-02-29" );
  assert ( toString ( CDate ( 1999, 1, 1 ) + 1_year + 28_days + 1_month )  == "2000-02-29" );
  assert ( toString ( CDate ( 2001, 1, 1 ) + 1095_days + 28_days + 1_month )  == "2004-02-29" );
  try
  {
    tmp = CDate ( 2001, 1, 1 ) + 28_days + 1_month + 1095_days;
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  try
  {
    tmp = CDate ( 2000, 1, 1 ) + 28_days + 1_month + 1_year;
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  tmp = CDate ( 2000, 1, 1 );
  assert ( toString ( tmp - 5_years + 2_months )  == "1995-03-01" );
  assert ( toString ( tmp ) == "2000-01-01" );
  assert ( toString ( CDate ( 2000, 1, 1 ) - 1_year - 3_months - 10_days )  == "1998-09-21" );
  assert ( toString ( CDate ( 2000, 1, 1 ) - 2_years - ( -3_months ) + ( -10_days ) )  == "1998-03-22" );
  assert ( toString ( CDate ( 2000, 1, 1 ) + 59_days - 1_month - 2_years )  == "1998-01-29" );
  try
  {
    tmp = CDate ( 2000, 1, 1 ) + 59_days - 2_years - 1_month;
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  tmp = CDate ( 2000, 1, 1 );
  tmp +=  - 2_years - ( -3_months ) + ( -10_days );
  assert ( toString ( tmp ) == "1998-03-22" );
  tmp = CDate ( 2000, 1, 1 );
  tmp += 59_days - 1_month - 2_years;
  assert ( toString ( tmp ) == "1998-01-29" );
  try
  {
    tmp = CDate ( 2000, 1, 1 );
    tmp += 59_days - 2_years - 1_month;
    assert ( "Missing exception" == NULL );
  }
  catch ( const InvalidDateException & e )
  {
  }
  tmp = CDate ( 2018, 3, 15 ) + ( -3_days );
  assert ( toString ( tmp ) == "2018-03-12" );
  assert ( !( CDate ( 2018, 3, 15 ) == CDate ( 2000, 1, 1 ) ) );
  assert ( CDate ( 2018, 3, 15 ) != CDate ( 2000, 1, 1 ) );
  assert ( !( CDate ( 2018, 3, 15 ) < CDate ( 2000, 1, 1 ) ) );
  assert ( CDate ( 2018, 3, 15 ) == CDate ( 2018, 3, 15 ) );
  assert ( !( CDate ( 2018, 3, 15 ) != CDate ( 2018, 3, 15 ) ) );
  assert ( !( CDate ( 2018, 3, 15 ) < CDate ( 2018, 3, 15 ) ) );
  assert ( !( CDate ( 2018, 3, 15 ) + 1_day == CDate ( 2018, 3, 15 ) ) );
  assert ( CDate ( 2018, 3, 15 ) + 1_day != CDate ( 2018, 3, 15 ) );
  assert ( !( CDate ( 2018, 3, 15 ) + 1_day < CDate ( 2018, 3, 15 ) ) );
  assert ( !( CDate ( 2018, 3, 15 ) + ( -1_day ) == CDate ( 2018, 3, 15 ) ) );
  assert ( CDate ( 2018, 3, 15 ) + ( -1_day ) != CDate ( 2018, 3, 15 ) );
  assert ( CDate ( 2018, 3, 15 ) + ( -1_day ) < CDate ( 2018, 3, 15 ) );
  assert ( CDate ( 2018, 3, 15 ) - CDate ( 2000, 1, 1 ) == 6648 );
  assert ( CDate ( 2000, 1, 1 ) - CDate ( 2018, 3, 15 ) == -6648 );
  assert ( CDate ( 2018, 3, 15 ) + 3_years + ( -18_months ) - CDate ( 2000, 1, 1 ) == 7197 );
  assert ( CDate ( 5398, 5, 2 ) - CDate ( 2018, 3, 15 ) == 1234567 );
  ostringstream oss;
  oss << setfill ( 'x' ) << left << hex << CDate ( 2000, 1, 1 ) << ' ' << setw ( 10 ) << 65536;
  assert ( oss . str () == "2000-01-01 10000xxxxx" );
#endif /* TEST_LITERALS */
  return 0;
}
#endif /* __PROGTEST__ */
