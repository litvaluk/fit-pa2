#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
using namespace std;

class CDate
{
  public:
                             CDate                         ( int               y,
                                                             int               m,
                                                             int               d )
                             : m_Y ( y ),
                               m_M ( m ),
                               m_D ( d )
    {
    }

    int                      CompareTo                     ( const CDate     & other ) const
    {
      if ( m_Y != other . m_Y )
        return ( other . m_Y < m_Y ) - ( m_Y < other . m_Y );
      if ( m_M != other . m_M )
        return ( other . m_M < m_M ) - ( m_M < other . m_M );
      return ( other . m_D < m_D ) - ( m_D < other . m_D );
    }

    friend ostream         & operator <<                   ( ostream         & os,
                                                             const CDate     & d )
    {
      return os << d . m_Y << '-' << d . m_M << '-' << d . m_D;
    }

  private:
    int                      m_Y;
    int                      m_M;
    int                      m_D;
};
enum class ESortKey
{
  NAME,
  BIRTH_DATE,
  ENROLL_YEAR
};
#endif /* __PROGTEST__ */

unsigned int cnt = 0;

void normalize (string& str) {
  transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void split (string text, set<string>& words) {

  int i = 0;
  char ch;
  normalize(text);
  string word;

  while((ch=text[i++])) {
    if (isspace(ch)) {
      if (!word.empty()) {
        words.insert(word);
      }
      word = "";
    }
    else {
      word += ch;
    }
  }
  if (!word.empty()) {
    words.insert(word);
  }
}

class CStudent {

  public:

    // constructor
    CStudent (const string& name, const CDate& born, int enrolled)
    : m_name(name), m_born(born), m_enrolled(enrolled) {

      m_id = cnt;
      cnt++;
      split(m_name, m_nameSet);

    }

    // operator ==
    bool operator == (const CStudent& other) const {
      return other.GetName() == m_name
          && other.GetEnrolled() == m_enrolled
          && other.GetBorn().CompareTo(m_born) == 0;
    }

    // operator !=
    bool operator != (const CStudent& other) const {
      return !(*this == other);
    }

    // name getter
    string GetName () const {
      return m_name;
    }

    set<string> GetNameSet () const {
      return m_nameSet;
    }

    // born (date) getter
    CDate GetBorn () const {
      return m_born;
    }

    // enrolled getter
    int GetEnrolled () const {
      return m_enrolled;
    }

    int GetId () const {
      return m_id;
    }

  private:

    int m_id;

    string m_name;
    set<string> m_nameSet;
    CDate m_born;
    int m_enrolled;

};

class CFilter {

  public:

    CFilter(void) {
      m_bAfterBool = false;
      m_bBeforeBool = false;
      m_eAfterBool = false;
      m_eBeforeBool = false;
    }

    ~CFilter(void) {}

    CFilter& Name(const string& name) {
      string tmpName = name;
      set<string> tmpSet;
      split(tmpName, tmpSet);
      if (m_nameSets.find(tmpSet) != m_nameSets.end()) {
        return *this;
      }
      m_nameSets.insert(tmpSet);
      return *this;
    }

    CFilter& BornBefore(const CDate& date) {
      if (!m_bBeforeBool) {
        m_born.second = date;
        m_bBeforeBool = true;
      }
      else if (date.CompareTo(m_born.second) == -1) {
        m_born.second = date;
      }
      return *this;
    }

    CFilter& BornAfter(const CDate& date) {
      if (!m_bAfterBool) {
        m_born.first = date;
        m_bAfterBool = true;
      }
      else if (date.CompareTo(m_born.first) == 1) {
        m_born.first = date;
      }
      return *this;
    }

    CFilter& EnrolledBefore(int year) {
      if (!m_eBeforeBool) {
        m_enrolled.second = year;
        m_eBeforeBool = true;
      }
      else if (year < m_enrolled.second) {
        m_enrolled.second = year;
      }
      return *this;
    }

    CFilter& EnrolledAfter(int year) {
      if (!m_eAfterBool) {
        m_enrolled.first = year;
        m_eAfterBool = true;
      }
      else if (year > m_enrolled.first) {
        m_enrolled.first = year;
      }
      return *this;
    }

    bool IsBAfter () const {
      return m_bAfterBool;
    }

    bool IsBBefore () const {
      return m_bBeforeBool;
    }

    bool IsEAfter () const {
      return m_eAfterBool;
    }

    bool IsEBefore () const {
      return m_eBeforeBool;
    }

    set<set<string>> m_nameSets;
    pair<int, int> m_enrolled = make_pair(0, 0);
    pair<CDate, CDate> m_born = make_pair(CDate(0, 0, 0), CDate(0, 0, 0));

  private:

    bool m_bAfterBool;
    bool m_bBeforeBool;
    bool m_eAfterBool;
    bool m_eBeforeBool;

};


struct TSortOpt {
  ESortKey key;
  bool ascending;
};

class CSort {

  public:

    CSort(void) {}

    CSort& AddKey(ESortKey key, bool ascending) {
      TSortOpt sortOpt;
      sortOpt.key = key;
      sortOpt.ascending = ascending;
      keys.push_back(sortOpt);
      return *this;
    }

    TSortOpt operator [] (int i) const {
      return keys[i];
    }

    int size() const {
      return keys.size();
    }

    vector<TSortOpt> keys;

  private:

};

class CompareDate {
  public:
    CompareDate () {}
    bool operator () (const CDate& a, const CDate& b) const {
      if (a.CompareTo(b) == -1) {
        return true;
      }
      else {
        return false;
      }
    }
};

class CompareStudents {

  public:

    CompareStudents (const CSort& sortOpt) : m_sortOpt(sortOpt) {}

    bool operator () (const CStudent& a, const CStudent& b) {
       for (auto& x: m_sortOpt.keys) {
           switch (x.key) {
               case ESortKey::NAME: {
                  string aUpper = a.GetName();
                  transform(aUpper.begin(), aUpper.end(), aUpper.begin(), ::toupper);
                  string bUpper = b.GetName();
                  transform(bUpper.begin(), bUpper.end(), bUpper.begin(), ::toupper);
                  if (x.ascending ? (aUpper < bUpper) : (bUpper < aUpper)) {
                      return true;
                  }
                  if (aUpper == bUpper) {
                      continue;
                  }
                  break;
                }
                case ESortKey::BIRTH_DATE:
                  if (x.ascending ? (a.GetBorn().CompareTo(b.GetBorn()) < 0) : (b.GetBorn().CompareTo(a.GetBorn()) < 0)) {
                      return true;
                  }
                  if (a.GetBorn().CompareTo(b.GetBorn()) == 0) {
                      continue;
                  }
                  break;

                case ESortKey::ENROLL_YEAR:
                  if (x.ascending ? (a.GetEnrolled() < b.GetEnrolled()) : (b.GetEnrolled() < a.GetEnrolled())) {
                      return true;
                  }
                  if (a.GetEnrolled() == b.GetEnrolled()) {
                      continue;
                  }
                  break;
               default:
                   break;
           }
           return false;
       }
       return a.GetId() < b.GetId();
   }

  private:

    CSort m_sortOpt;

};

class CStudyDept {

  public:

    CStudyDept (void) {}

    // adds student to database
    bool AddStudent (const CStudent& x) {

      // add to nameSorted
      auto search1 = m_nameSorted.find(x.GetNameSet());
      while (search1 != m_nameSorted.end() && search1->second.GetNameSet() == x.GetNameSet()) {
        if (search1->second.GetEnrolled() == x.GetEnrolled() && search1->second.GetBorn().CompareTo(x.GetBorn()) == 0 && search1->second.GetName() == x.GetName()) {
          return false;
        }
        ++search1;
      }
      m_nameSorted.insert(search1, make_pair(x.GetNameSet(), x));

      // add to yearSorted
      auto search2 = m_yearSorted.find(x.GetEnrolled());
      while (search2 != m_yearSorted.end() && search2->second.GetEnrolled() == x.GetEnrolled()) {
        if (search2->second.GetName() == x.GetName() && search2->second.GetBorn().CompareTo(x.GetBorn()) == 0) {
          return false;
        }
        ++search2;
      }
      m_yearSorted.insert(search2, make_pair(x.GetEnrolled(), x));

      // add to dateSorted
      auto search3 = m_dateSorted.find(x.GetBorn());
      while (search3 != m_dateSorted.end() && search3->second.GetBorn().CompareTo(x.GetBorn()) == 0) {
        if (search3->second.GetName() == x.GetName() && search3->second.GetEnrolled() == x.GetEnrolled()) {
          return false;
        }
        ++search3;
      }
      m_dateSorted.insert(search3, make_pair(x.GetBorn(), x));

      return true;
    }

    // removes student from database
    bool DelStudent (const CStudent& x) {

      bool found = false;

      auto search1 = m_nameSorted.find(x.GetNameSet());
      while (search1 != m_nameSorted.end() && search1->second.GetNameSet() == x.GetNameSet()) {
        if (search1->second.GetEnrolled() == x.GetEnrolled() && search1->second.GetBorn().CompareTo(x.GetBorn()) == 0 && search1->second.GetName() == x.GetName()) {
          m_nameSorted.erase(search1);
          found = true;
          break;
        }
        ++search1;
      }

      if (!found) {
        return false;
      }

      auto search2 = m_yearSorted.find(x.GetEnrolled());
      while (search2 != m_yearSorted.end() && search2->second.GetEnrolled() == x.GetEnrolled()) {
        if (search2->second.GetName() == x.GetName() && search2->second.GetBorn().CompareTo(x.GetBorn()) == 0) {
          m_yearSorted.erase(search2);
          break;
        }
        ++search2;
      }
      auto search3 = m_dateSorted.find(x.GetBorn());
      while (search3 != m_dateSorted.end() && search3->second.GetBorn().CompareTo(x.GetBorn()) == 0) {
        if (search3->second.GetName() == x.GetName() && search3->second.GetEnrolled() == x.GetEnrolled()) {
          m_dateSorted.erase(search3);
          break;
        }
        ++search3;
      }

      return true;
    }

    list<CStudent> Search (const CFilter& flt, const CSort& sortOpt) const {

      list<CStudent> x;

      applyFilters(x, flt);

      x.sort(CompareStudents(sortOpt));

      return x;
    }

      set<string> Suggest (const string& name) const {
        string tmpName = name;
        set<string> src;
        set<string> dest;
        normalize(tmpName);
        split(tmpName, src);
        for (auto it = m_nameSorted.begin(); it != m_nameSorted.end(); ++it) {
          if (includes(it->first.begin(), it->first.end(), src.begin(), src.end())) {
            dest.insert(it->second.GetName());
          }
        }
        return dest;
      }

  private:

    multimap<set<string>, CStudent> m_nameSorted;
    multimap<int, CStudent> m_yearSorted;
    multimap<CDate, CStudent, CompareDate> m_dateSorted;

    void applyFilters (list<CStudent>& x, const CFilter& flt) const {

      bool nameFilterApplied = !flt.m_nameSets.empty();
      bool bornFilterApplied = flt.IsBAfter() || flt.IsBBefore();
      bool enrolledFilterApplied = flt.IsEAfter() || flt.IsEBefore();

      bool withoutFilter = !nameFilterApplied && !bornFilterApplied && !enrolledFilterApplied;

      if (!withoutFilter) {
        if (nameFilterApplied && !bornFilterApplied && !enrolledFilterApplied) {
          // cout << "n" << endl;

          if (!flt.m_nameSets.empty()) {
            for (auto it = flt.m_nameSets.begin(); it != flt.m_nameSets.end(); ++it) {
              auto search1 = m_nameSorted.find(*it);
              while (search1 != m_nameSorted.end() && search1->second.GetNameSet() == *it) {
                x.push_back(search1->second);
                ++search1;
              }
            }
          }
        }
        //=================================================================================================
        else if (!nameFilterApplied && bornFilterApplied && !enrolledFilterApplied) {
          // cout << "b" << endl;
          if (flt.IsBBefore() || flt.IsBAfter()) {
            if (!flt.IsBBefore() && flt.IsBAfter()) {
              auto it = m_dateSorted.rbegin();
              while (it != m_dateSorted.rend() && it->second.GetBorn().CompareTo(flt.m_born.first) == 1) {
                x.push_back(it->second);
                ++it;
              }
            }
            if (flt.IsBBefore() && !flt.IsBAfter()) {
              auto it = m_dateSorted.begin();
              while (it != m_dateSorted.end() && it->second.GetBorn().CompareTo(flt.m_born.second) == -1) {
                x.push_back(it->second);
                ++it;
              }
            }
            else {
              auto it = m_dateSorted.upper_bound(flt.m_born.first);
              while (it != m_dateSorted.end() && it->second.GetBorn().CompareTo(flt.m_born.second) == -1) {
                x.push_back(it->second);
                ++it;
              }
            }
          }
        }
        //=================================================================================================
        else if (!nameFilterApplied && !bornFilterApplied && enrolledFilterApplied) {
          // cout << "e" << endl;
          if (flt.IsEBefore() || flt.IsEAfter()) {

            if (!flt.IsEBefore() && flt.IsEAfter()) {
              auto it = m_yearSorted.rbegin();
              while (it != m_yearSorted.rend() && it->second.GetEnrolled() > flt.m_enrolled.first) {
                x.push_back(it->second);
                ++it;
              }
            }
            if (flt.IsEBefore() && !flt.IsEAfter()) {
              auto it = m_yearSorted.begin();
              while (it != m_yearSorted.end() && it->second.GetEnrolled() < flt.m_enrolled.second) {
                x.push_back(it->second);
                ++it;
              }
            }
            else {
              auto it = m_yearSorted.upper_bound(flt.m_enrolled.first);
              while (it != m_yearSorted.end() && it->second.GetEnrolled() < flt.m_enrolled.second) {
                x.push_back(it->second);
                ++it;
              }
            }
          }
        }
        //=================================================================================================
        else if (nameFilterApplied && bornFilterApplied && !enrolledFilterApplied) {
          // cout << "n b" << endl;
          for (const auto& setik : flt.m_nameSets) {
            auto it = m_nameSorted.lower_bound(setik);
            while (it != m_nameSorted.end() && it->second.GetNameSet() == setik) {
              if (satisfiesBornFilter(flt, it->second) && satisfiesEnrolledFilter(flt, it->second)) {
                x.push_back(it->second);
              }
              ++it;
            }
          }
        }
        else if (!nameFilterApplied && bornFilterApplied && enrolledFilterApplied) {
          // cout << "b e" << endl;
            if (!flt.IsBBefore() && flt.IsBAfter()) {
              auto it = m_dateSorted.rbegin();
              while (it != m_dateSorted.rend() && it->second.GetBorn().CompareTo(flt.m_born.first) == 1) {
                if (satisfiesEnrolledFilter(flt, it->second)) {
                  x.push_back(it->second);
                }
                ++it;
              }
            }
            else if (flt.IsBBefore() && !flt.IsBAfter()) {
              auto it = m_dateSorted.begin();
              while (it != m_dateSorted.end() && it->second.GetBorn().CompareTo(flt.m_born.second) == -1) {
                if (satisfiesEnrolledFilter(flt, it->second)) {
                  x.push_back(it->second);
                }
                ++it;
              }
            }

            else {
              auto it = m_dateSorted.lower_bound(flt.m_born.first);
              while (it != m_dateSorted.end() && it->second.GetBorn().CompareTo(flt.m_born.second) == -1) {
                if (satisfiesEnrolledFilter(flt, it->second)) {
                  x.push_back(it->second);
                }
                ++it;
              }
            }
          }
        else if (nameFilterApplied && !bornFilterApplied && enrolledFilterApplied) {
          // cout << "n e" << endl;
          for (auto setik : flt.m_nameSets) {
            auto it = m_nameSorted.lower_bound(setik);
            while (it != m_nameSorted.end() && it->second.GetNameSet() == setik) {
              if (satisfiesBornFilter(flt, it->second) && satisfiesEnrolledFilter(flt, it->second)) {
                x.push_back(it->second);
              }
              ++it;
            }
          }
        }
        else {
          for (auto setik : flt.m_nameSets) {
            // cout << "n b e" << endl;
            auto it = m_nameSorted.lower_bound(setik);
            while (it != m_nameSorted.end() && it->second.GetNameSet() == setik) {
              if (satisfiesBornFilter(flt, it->second) && satisfiesEnrolledFilter(flt, it->second)) {
                x.push_back(it->second);
              }
              ++it;
            }
          }
        }
      }

      // add all to list if no filter applied
      if (withoutFilter) {
        for (auto it = m_nameSorted.begin(); it != m_nameSorted.end(); ++it) {
          x.push_back(it->second);
          //cout << "pushing back" << endl;
        }
      }
    }

    bool satisfiesBornFilter (const CFilter& flt, const CStudent& st) const {
      if (flt.IsBBefore() || flt.IsBAfter()) {
        if (!flt.IsBBefore() && flt.IsBAfter() && st.GetBorn().CompareTo(flt.m_born.first) == 1) {
            return true;
        }
        else if (flt.IsBBefore() && !flt.IsBAfter() && st.GetBorn().CompareTo(flt.m_born.second) == -1) {
            return true;
        }
        else if (st.GetBorn().CompareTo(flt.m_born.second) == -1 && st.GetBorn().CompareTo(flt.m_born.first) == 1) {
            return true;
        }
        else {
          return false;
        }
      }
      else {
        return true;
      }
    }

    bool satisfiesEnrolledFilter (const CFilter& flt, const CStudent& st) const {
      if (flt.IsEBefore() || flt.IsEAfter()) {
        if (!flt.IsEBefore() && flt.IsEAfter() && st.GetEnrolled() > flt.m_enrolled.first) {
            return true;
        }
        else if (flt.IsEBefore() && !flt.IsEAfter() && st.GetEnrolled() < flt.m_enrolled.second) {
            return true;
        }
        else if (st.GetEnrolled() > flt.m_enrolled.first && st.GetEnrolled() < flt.m_enrolled.second) {
            return true;
        }
        else {
          return false;
        }
      }
      else {
        return true;
      }
    }
};

#ifndef __PROGTEST__
int main ( void )
{
  CStudyDept x0;
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2010 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2010 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1997, 6, 17), 2010 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1997, 6, 17), 2010 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1980, 4, 11), 2016 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1980, 4, 11), 2016 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2016 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2016 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2010 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2010 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1997, 6, 17), 2016 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1997, 6, 17), 2016 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2016 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2016 ) ) );
  assert ( x0 . AddStudent ( CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ) ) );
  assert ( x0 . AddStudent ( CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ) ) );
  assert ( x0 . AddStudent ( CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ) ) );
  assert ( x0 . AddStudent ( CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ) ) );
  assert ( x0 . AddStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
  assert ( x0 . AddStudent ( CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ) ) );
  assert ( x0 . AddStudent ( CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ) ) );
  assert ( x0 . AddStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ) ) );
  assert ( x0 . AddStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ) ) );
  assert ( x0 . AddStudent ( CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ) ) );
  assert ( x0 . Search ( CFilter (), CSort () ) == (list<CStudent>
  {
    CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
    CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ),
    CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 )
  }) );
  assert ( x0 . Search ( CFilter (), CSort () . AddKey ( ESortKey::NAME, true ) ) == (list<CStudent>
  {
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
    CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
    CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 )
  }) );
  assert ( x0 . Search ( CFilter (), CSort () . AddKey ( ESortKey::NAME, false ) ) == (list<CStudent>
  {
    CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ),
    CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
    CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 )
  }) );
  assert ( x0 . Search ( CFilter (), CSort () . AddKey ( ESortKey::ENROLL_YEAR, false ) . AddKey ( ESortKey::BIRTH_DATE, false ) . AddKey ( ESortKey::NAME, true ) ) == (list<CStudent>
  {
    CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
    CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
    CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 )
  }) );
  assert ( x0 . Search ( CFilter () . Name ( "james bond" ), CSort () . AddKey ( ESortKey::ENROLL_YEAR, false ) . AddKey ( ESortKey::BIRTH_DATE, false ) . AddKey ( ESortKey::NAME, true ) ) == (list<CStudent>
  {
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 )
  }) );
  assert ( x0 . Search ( CFilter () . BornAfter ( CDate ( 1980, 4, 11) ) . BornBefore ( CDate ( 1983, 7, 13) ) . Name ( "John Taylor" ) . Name ( "james BOND" ), CSort () . AddKey ( ESortKey::ENROLL_YEAR, false ) . AddKey ( ESortKey::BIRTH_DATE, false ) . AddKey ( ESortKey::NAME, true ) ) == (list<CStudent>
  {
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 )
  }) );
  assert ( x0 . Search ( CFilter () . Name ( "james" ), CSort () . AddKey ( ESortKey::NAME, true ) ) == (list<CStudent>
  {
  }) );
  assert ( x0 . Suggest ( "peter" ) == (set<string>
  {
    "John Peter Taylor",
    "Peter John Taylor",
    "Peter Taylor"
  }) );
  assert ( x0 . Suggest ( "bond" ) == (set<string>
  {
    "Bond James",
    "James Bond"
  }) );
  assert ( x0 . Suggest ( "peter joHn" ) == (set<string>
  {
    "John Peter Taylor",
    "Peter John Taylor"
  }) );
  assert ( x0 . Suggest ( "peter joHn bond" ) == (set<string>
  {
  }) );
  assert ( x0 . Suggest ( "pete" ) == (set<string>
  {
  }) );
  assert ( x0 . Suggest ( "peter joHn PETER" ) == (set<string>
  {
    "John Peter Taylor",
    "Peter John Taylor"
  }) );
  assert ( ! x0 . AddStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
  assert ( x0 . DelStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
  assert ( x0 . Search ( CFilter () . BornAfter ( CDate ( 1980, 4, 11) ) . BornBefore ( CDate ( 1983, 7, 13) ) . Name ( "John Taylor" ) . Name ( "james BOND" ), CSort () . AddKey ( ESortKey::ENROLL_YEAR, false ) . AddKey ( ESortKey::BIRTH_DATE, false ) . AddKey ( ESortKey::NAME, true ) ) == (list<CStudent>
  {
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 )
  }) );
  assert ( ! x0 . DelStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
  return 0;
}
#endif /* __PROGTEST__ */
