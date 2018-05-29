#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CPerson{
// -----------------------------------------------------------------------------------------------------
// -------------------------------------------PUBLIC----------------------------------------------------
// -----------------------------------------------------------------------------------------------------
public:
CPerson(const string& name, const string& surname, const string& email, unsigned int salary){
  m_name = name;
  m_surname = surname;
  m_email = email;
  m_salary = salary;
}

string getName() const {
  return m_name;
}

void setName(const string& name) {
  m_name = name;
}

string getSurname() const {
  return m_surname;
}

void setSurname(const string& surname) {
  m_surname = surname;
}

string getEmail() const {
  return m_email;
}

void setEmail(const string& email) {
  m_email = email;
}

unsigned int getSalary() const{
  return m_salary;
}

void setSalary(unsigned int salary) {
  m_salary = salary;
}
// -----------------------------------------------------------------------------------------------------
// -------------------------------------------PRIVATE---------------------------------------------------
// -----------------------------------------------------------------------------------------------------
private:
  string m_name;
  string m_surname;
  string m_email;
  unsigned int m_salary;
};

class CPersonalAgenda {
// -----------------------------------------------------------------------------------------------------
// -------------------------------------------PUBLIC----------------------------------------------------
// -----------------------------------------------------------------------------------------------------
public:
  CPersonalAgenda (void) {};
  ~CPersonalAgenda (void) {};

  bool Add (const string& name, const string& surname, const string& email, unsigned int salary) {
    if (name == "" || surname == "" || email == "") {
      return false;
    }
    CPerson tmp(name, surname, email, salary);

    int nameIndex = binarySearchForAdding(name, surname);
    if (nameIndex == -1) {
      return false;
    }
    int emailIndex = binarySearchForAdding(email);
    if (emailIndex == -1) {
      return false;
    }
    if ((unsigned int)nameIndex == m_nameSorted.size()) {
      m_nameSorted.push_back(tmp);
    }
    else {
      m_nameSorted.insert(m_nameSorted.begin() + nameIndex, tmp);
    }
    if ((unsigned int)emailIndex == m_emailSorted.size()) {
      m_emailSorted.push_back(tmp);
    }
    else {
      m_emailSorted.insert(m_emailSorted.begin() + emailIndex, tmp);
    }
    return true;
  }

  bool Del (const string& name, const string& surname) {
    int nameIndex = binarySearch(name, surname);
    if (nameIndex == -1) {
      return false;
    }
    int emailIndex = binarySearch(m_nameSorted[nameIndex].getEmail());
    m_nameSorted.erase(m_nameSorted.begin() + nameIndex);
    m_emailSorted.erase(m_emailSorted.begin() + emailIndex);
    return true;
  }

  bool Del (const string& email) {
    int emailIndex = binarySearch(email);
    if (emailIndex == -1) {
      return false;
    }
    int nameIndex = binarySearch(m_emailSorted[emailIndex].getName(), m_emailSorted[emailIndex].getSurname());
    m_emailSorted.erase(m_emailSorted.begin() + emailIndex);
    m_nameSorted.erase(m_nameSorted.begin() + nameIndex);
    return true;
  }

  bool ChangeName (const string& email, const string& newName, const string& newSurname) {
    int emailIndex = binarySearch(email);
    if (emailIndex == -1 || binarySearch(newName, newSurname) >= 0) {
      return false;
    }
    int nameIndex = binarySearch(m_emailSorted[emailIndex].getName(), m_emailSorted[emailIndex].getSurname());
    m_emailSorted[emailIndex].setName(newName);
    m_emailSorted[emailIndex].setSurname(newSurname);
    CPerson tmp(newName, newSurname, email, m_nameSorted[nameIndex].getSalary());
    m_nameSorted.erase(m_nameSorted.begin() + nameIndex);
    nameIndex = binarySearchForAdding(newName, newSurname);
    if ((unsigned int)nameIndex == m_nameSorted.size()) {
      m_nameSorted.push_back(tmp);
    }
    else {
      m_nameSorted.insert(m_nameSorted.begin() + nameIndex, tmp);
    }
    return true;
  }

  bool ChangeEmail (const string& name, const string& surname, const string& newEmail) {
    int nameIndex = binarySearch(name, surname);
    if (nameIndex == -1 || binarySearch(newEmail) >= 0) {
      return false;
    }
    int emailIndex = binarySearch(m_nameSorted[nameIndex].getEmail());
    m_nameSorted[nameIndex].setEmail(newEmail);
    CPerson tmp(name, surname, newEmail, m_emailSorted[emailIndex].getSalary());
    m_emailSorted.erase(m_emailSorted.begin() + emailIndex);
    emailIndex = binarySearchForAdding(newEmail);
    if ((unsigned int)emailIndex == m_emailSorted.size()) {
      m_emailSorted.push_back(tmp);
    }
    else {
      m_emailSorted.insert(m_emailSorted.begin() + emailIndex, tmp);
    }
    return true;
  }

  bool SetSalary (const string& name, const string& surname, unsigned int salary){
    int nameIndex = binarySearch(name, surname);
    if (nameIndex < 0) {
      return false;
    }
    int emailIndex = binarySearch(m_nameSorted[nameIndex].getEmail());
    m_nameSorted[nameIndex].setSalary(salary);
    m_emailSorted[emailIndex].setSalary(salary);
    return true;
  }

  bool SetSalary (const string& email, unsigned int salary){
    int emailIndex = binarySearch(email);
    if (emailIndex < 0) {
      return false;
    }
    int nameIndex = binarySearch(m_emailSorted[emailIndex].getName(), m_emailSorted[emailIndex].getSurname());
    m_emailSorted[emailIndex].setSalary(salary);
    m_nameSorted[nameIndex].setSalary(salary);
    return true;
  }

  unsigned int GetSalary (const string& name, const string& surname) const{
    int index = binarySearch(name, surname);
    return (index < 0) ? 0 : m_nameSorted[index].getSalary();
  }
  unsigned int GetSalary (const string& email) const{
    int index = binarySearch(email);
    return (index < 0) ? 0 : m_emailSorted[index].getSalary();
  }

  bool GetRank (const string& name, const string& surname, int& rankMin, int& rankMax) const{
    rankMin = rankMax = 0;
    int index = binarySearch(name, surname);
    if(index == -1){
      return false;
    }
    unsigned int salary = m_nameSorted[index].getSalary();
    for (size_t i = 0; i < m_nameSorted.size(); i++) {
      if (&(m_nameSorted[i]) == &(m_nameSorted[index])) {
        continue;
      }
      if (m_nameSorted[i].getSalary() < salary ) {
        rankMin++;
      }
      if (m_nameSorted[i].getSalary() == salary ) {
        rankMax++;
      }
    }
    rankMax += rankMin;
    return true;
  }
  bool GetRank (const string& email, int& rankMin, int& rankMax) const{
    rankMin = rankMax = 0;
    int index = binarySearch(email);
    if(index == -1){
      return false;
    }
    unsigned int salary = m_emailSorted[index].getSalary();
    for (size_t i = 0; i < m_emailSorted.size(); i++) {
      if (&(m_emailSorted[i]) == &(m_emailSorted[index])) {
        continue;
      }
      if (m_emailSorted[i].getSalary() < salary ) {
        rankMin++;
      }
      if (m_emailSorted[i].getSalary() == salary ) {
        rankMax++;
      }
    }
    rankMax += rankMin;
    return true;
  }

  bool GetFirst (string& outName, string& outSurname) const{
    if (m_nameSorted.size() == 0){
      return false;
    }
    outName = m_nameSorted[0].getName();
    outSurname = m_nameSorted[0].getSurname();
    return true;
  }

  bool GetNext (const string& name, const string& surname, string& outName, string& outSurname) const{

    if (m_nameSorted[m_nameSorted.size() - 1].getName() == name && m_nameSorted[m_nameSorted.size() - 1].getSurname() == surname) {
      return false;
    }
    int index = binarySearch(name, surname);
    if (index == -1) {
      return false;
    }
    outName = m_nameSorted[index+1].getName();
    outSurname = m_nameSorted[index+1].getSurname();

    return true;
  }

  void Print () {
    for (unsigned int i = 0; i < m_nameSorted.size(); i++) {
      cout << m_nameSorted[i].getName() << " " << m_nameSorted[i].getSurname() << endl;
    }
    cout << "------------------------" << endl;
    for (unsigned int i = 0; i < m_emailSorted.size(); i++) {
      cout << m_emailSorted[i].getEmail() << endl;
    }
  }

// -----------------------------------------------------------------------------------------------------
// -------------------------------------------PRIVATE---------------------------------------------------
// -----------------------------------------------------------------------------------------------------
private:

  vector<CPerson> m_nameSorted;
  vector<CPerson> m_emailSorted;

  int binarySearch(const string& name, const string& surname) const {

    int low = 0;
    int high = m_nameSorted.size() - 1;
    int mid = 0;

    while (low <= high){
        mid = low + (high-low)/2;
        if (m_nameSorted[mid].getSurname() == surname && m_nameSorted[mid].getName() == name) {
          return mid;
        }
        else if (m_nameSorted[mid].getSurname() + m_nameSorted[mid].getName() < surname + name) {
          low = mid + 1;
        }
        else if ((m_nameSorted[mid].getSurname() + m_nameSorted[mid].getName()) == (surname + name)
            && surname > m_nameSorted[mid].getSurname()) {
          low = mid + 1;
        }
        else {
          high = mid - 1;
        }
    }

    return -1;
  }

  int binarySearchForAdding(const string& name, const string& surname) const {

    int low = 0;
    int high = m_nameSorted.size() - 1;
    int mid = 0;

    while (low <= high){
        mid = low + (high-low)/2;
        if (m_nameSorted[mid].getSurname() == surname && m_nameSorted[mid].getName() == name) {
          return -1;
        }
        else if (m_nameSorted[mid].getSurname() + m_nameSorted[mid].getName() < surname + name) {
          low = mid + 1;
        }
        else if ((m_nameSorted[mid].getSurname() + m_nameSorted[mid].getName()) == (surname + name)
            && surname > m_nameSorted[mid].getSurname()) {
          low = mid + 1;
        }
        else {
          high = mid - 1;
        }
    }

    return low;
  }

  int binarySearch(const string& email) const {

    int low = 0;
    int high = m_emailSorted.size() - 1;
    string x = email;

    while (low <= high){
        int mid = low + (high-low)/2;
        if (m_emailSorted[mid].getEmail() == x) {
          return mid;
        }
        if (m_emailSorted[mid].getEmail() < x) {
          low = mid + 1;
        }
        else {
          high = mid - 1;
        }
    }
    return -1;
  }

  int binarySearchForAdding(const string& email) const {

    int low = 0;
    int high = m_emailSorted.size() - 1;
    string x = email;

    while (low <= high){
        int mid = low + (high-low)/2;
        if (m_emailSorted[mid].getEmail() == x) {
          return -1;
        }
        if (m_emailSorted[mid].getEmail() < x) {
          low = mid + 1;
        }
        else {
          high = mid - 1;
        }
    }
    return low;
  }

};

#ifndef __PROGTEST__
int main ( void )
{
  string outName, outSurname;
  int lo, hi;

  CPersonalAgenda b1;
  assert ( b1 . Add ( "John", "Smith", "john", 30000 ) );
  assert ( b1 . Add ( "John", "Miller", "johnm", 35000 ) );
  assert ( b1 . Add ( "Peter", "Smith", "peter", 23000 ) );
  assert ( b1 . GetFirst ( outName, outSurname )
           && outName == "John"
           && outSurname == "Miller" );
  assert ( b1 . GetNext ( "John", "Miller", outName, outSurname )
           && outName == "John"
           && outSurname == "Smith" );
  assert ( b1 . GetNext ( "John", "Smith", outName, outSurname )
           && outName == "Peter"
           && outSurname == "Smith" );
  assert ( ! b1 . GetNext ( "Peter", "Smith", outName, outSurname ) );
  assert ( b1 . SetSalary ( "john", 32000 ) );
  assert ( b1 . GetSalary ( "john" ) ==  32000 );
  assert ( b1 . GetSalary ( "John", "Smith" ) ==  32000 );
  assert ( b1 . GetRank ( "John", "Smith", lo, hi )
           && lo == 1
           && hi == 1 );
  assert ( b1 . GetRank ( "john", lo, hi )
           && lo == 1
           && hi == 1 );
  assert ( b1 . GetRank ( "peter", lo, hi )
           && lo == 0
           && hi == 0 );
  assert ( b1 . GetRank ( "johnm", lo, hi )
           && lo == 2
           && hi == 2 );
  assert ( b1 . SetSalary ( "John", "Smith", 35000 ) );
  assert ( b1 . GetSalary ( "John", "Smith" ) ==  35000 );
  assert ( b1 . GetSalary ( "john" ) ==  35000 );
  assert ( b1 . GetRank ( "John", "Smith", lo, hi )
           && lo == 1
           && hi == 2 );
  assert ( b1 . GetRank ( "john", lo, hi )
           && lo == 1
           && hi == 2 );
  assert ( b1 . GetRank ( "peter", lo, hi )
           && lo == 0
           && hi == 0 );
  assert ( b1 . GetRank ( "johnm", lo, hi )
           && lo == 1
           && hi == 2 );
  assert ( b1 . ChangeName ( "peter", "James", "Bond" ) );
  assert ( b1 . GetSalary ( "peter" ) ==  23000 );
  assert ( b1 . GetSalary ( "James", "Bond" ) ==  23000 );
  assert ( b1 . GetSalary ( "Peter", "Smith" ) ==  0 );
  assert ( b1 . GetFirst ( outName, outSurname )
           && outName == "James"
           && outSurname == "Bond" );
  assert ( b1 . GetNext ( "James", "Bond", outName, outSurname )
           && outName == "John"
           && outSurname == "Miller" );
  assert ( b1 . GetNext ( "John", "Miller", outName, outSurname )
           && outName == "John"
           && outSurname == "Smith" );
  assert ( ! b1 . GetNext ( "John", "Smith", outName, outSurname ) );
  assert ( b1 . ChangeEmail ( "James", "Bond", "james" ) );
  assert ( b1 . GetSalary ( "James", "Bond" ) ==  23000 );
  assert ( b1 . GetSalary ( "james" ) ==  23000 );
  assert ( b1 . GetSalary ( "peter" ) ==  0 );
  assert ( b1 . Del ( "james" ) );
  assert ( b1 . GetRank ( "john", lo, hi )
           && lo == 0
           && hi == 1 );
  assert ( b1 . Del ( "John", "Miller" ) );
  assert ( b1 . GetRank ( "john", lo, hi )
           && lo == 0
           && hi == 0 );
  assert ( b1 . GetFirst ( outName, outSurname )
           && outName == "John"
           && outSurname == "Smith" );
  assert ( ! b1 . GetNext ( "John", "Smith", outName, outSurname ) );
  assert ( b1 . Del ( "john" ) );
  assert ( ! b1 . GetFirst ( outName, outSurname ) );
  assert ( b1 . Add ( "John", "Smith", "john", 31000 ) );
  assert ( b1 . Add ( "john", "Smith", "joHn", 31000 ) );
  assert ( b1 . Add ( "John", "smith", "jOhn", 31000 ) );

  CPersonalAgenda b2;
  assert ( ! b2 . GetFirst ( outName, outSurname ) );
  assert ( b2 . Add ( "James", "Bond", "james", 70000 ) );
  assert ( b2 . Add ( "James", "Smith", "james2", 30000 ) );
  assert ( b2 . Add ( "Peter", "Smith", "peter", 40000 ) );
  assert ( ! b2 . Add ( "James", "Bond", "james3", 60000 ) );
  assert ( ! b2 . Add ( "Peter", "Bond", "peter", 50000 ) );
  assert ( ! b2 . ChangeName ( "joe", "Joe", "Black" ) );
  assert ( ! b2 . ChangeEmail ( "Joe", "Black", "joe" ) );
  assert ( ! b2 . SetSalary ( "Joe", "Black", 90000 ) );
  assert ( ! b2 . SetSalary ( "joe", 90000 ) );
  assert ( b2 . GetSalary ( "Joe", "Black" ) ==  0 );
  assert ( b2 . GetSalary ( "joe" ) ==  0 );
  assert ( ! b2 . GetRank ( "Joe", "Black", lo, hi ) );
  assert ( ! b2 . GetRank ( "joe", lo, hi ) );
  assert ( ! b2 . ChangeName ( "joe", "Joe", "Black" ) );
  assert ( ! b2 . ChangeEmail ( "Joe", "Black", "joe" ) );
  assert ( ! b2 . Del ( "Joe", "Black" ) );
  assert ( ! b2 . Del ( "joe" ) );
  assert ( ! b2 . ChangeName ( "james2", "James", "Bond" ) );
  assert ( ! b2 . ChangeEmail ( "Peter", "Smith", "james" ) );
  assert ( ! b2 . ChangeName ( "peter", "Peter", "Smith" ) );
  assert ( ! b2 . ChangeEmail ( "Peter", "Smith", "peter" ) );
  assert ( b2 . Del ( "Peter", "Smith" ) );
  assert ( ! b2 . ChangeEmail ( "Peter", "Smith", "peter2" ) );
  assert ( ! b2 . SetSalary ( "Peter", "Smith", 35000 ) );
  assert ( b2 . GetSalary ( "Peter", "Smith" ) ==  0 );
  assert ( ! b2 . GetRank ( "Peter", "Smith", lo, hi ) );
  assert ( ! b2 . ChangeName ( "peter", "Peter", "Falcon" ) );
  assert ( ! b2 . SetSalary ( "peter", 37000 ) );
  assert ( b2 . GetSalary ( "peter" ) ==  0 );
  assert ( ! b2 . GetRank ( "peter", lo, hi ) );
  assert ( ! b2 . Del ( "Peter", "Smith" ) );
  assert ( ! b2 . Del ( "peter" ) );
  assert ( b2 . Add ( "Peter", "Smith", "peter", 40000 ) );
  assert ( b2 . GetSalary ( "peter" ) ==  40000 );

  return 0;
}
#endif /* __PROGTEST__ */
