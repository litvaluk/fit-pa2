#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;
#endif /* __PROGTEST__ */

// string comparison functions
bool smallerThan(const char* left, const char* right) {
  return strcmp(left, right) < 0;
}

bool biggerThan(const char* left, const char* right) {
  return strcmp(left, right) > 0;
}

bool areEqual(const char* left, const char* right) {
  return strcmp(left, right) == 0;
}


//--------------------------------------------------------------------------------------------------------
//---------------------------------------------ARRAY------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
template <class T>
class Array{
  //--------------------------------------------PUBLIC----------------------------------------------------
  public:

    Array(): m_size(0), m_capacity(3) {
      array = new T [m_capacity];
    }

    ~Array() {
      delete[] array;
    }

    T at (int pos) {
      return array[pos];
    }

    void pushBack(const T& t) {
      if (m_size == m_capacity) {
        resize();
      }
      array[m_size] = t;
      m_size++;
    }

    bool insert(int pos, const T& t) {
      if (pos > m_size || pos < 0) {
          return false;
      }
      if (m_size == m_capacity) {
        resize();
      }
      for (int i = m_size; i > pos; i--) {
        array[i] = array[i-1];
      }
      array[pos] = t;
      m_size++;
      return true;
    }

    bool erase(int pos) {
      if (pos > m_size || pos < 0) {
          return false;
      }
      for (int i = pos; i < m_size-1; i++) {
        array[i] = array[i+1];
      }
      m_size--;
      return true;
    }

    int size() const{
      return m_size;
    }

    // dangerous - only use this method if you know what you are doing
    void setSize(int size) {
      m_size = size;
    }

    int capacity() const {
      return m_capacity;
    }

    T operator [] (int index) const {
      return array[index];
    }

  //--------------------------------------------PRIVATE---------------------------------------------------
  private:

    T* array;
    int m_size;
    int m_capacity;

    void resize() {
      m_capacity *= 2;
      T* tmp = new T [m_capacity];
      for (int i = 0; i < m_size; i++) {
        tmp[i] = array[i];
      }
      delete[] array;
      array = tmp;
    }

};
//--------------------------------------------------------------------------------------------------------
//---------------------------------------------CMAIL------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
class CMail
{
  //--------------------------------------------PUBLIC----------------------------------------------------
  public:

    CMail(const char* from, const char* to, const char* body) {
      strncpy (m_from, from, sizeof (m_from));
      strncpy (m_to, to, sizeof (m_to));
      strncpy (m_body, body, sizeof (m_body));
    }

    bool operator == (const CMail& x) const {
      if(!areEqual(this->getFrom(), x.getFrom())) {
        return false;
      }
      if(!areEqual(this->getTo(), x.getTo())) {
        return false;
      }
      if(!areEqual(this->getBody(), x.getBody())) {
        return false;
      }
      return true;
    }

    const char* getFrom() const {
      return m_from;
    }

    const char* getTo() const {
      return m_to;
    }

    const char* getBody() const {
      return m_body;
    }
  //--------------------------------------------PRIVATE---------------------------------------------------
  private:

    char m_from[1000], m_to[1000], m_body[10000];

};

//--------------------------------------------------------------------------------------------------------
//------------------------------------------CMAILITERATOR-------------------------------------------------
//--------------------------------------------------------------------------------------------------------
class CMailIterator
{
  //--------------------------------------------PUBLIC----------------------------------------------------
  public:

    CMailIterator(const Array<CMail*>* inbox, int from, int to): m_pos(from), m_from(from), m_to(to){
      m_array = inbox;
    }

    operator bool (void) const {
      if (m_pos != m_to) {
        return true;
      }
      else {
        return false;
      }
    }

    bool operator ! (void) const {
      if (m_pos == m_to) {
        return true;
      }
      else {
        return false;
      }
    }

    const CMail& operator * (void) const {
      return *((*m_array)[m_pos]);
    }

    CMailIterator& operator ++ (void) {
      this->m_pos++;
      return *this;
    }

  //--------------------------------------------PRIVATE---------------------------------------------------
  private:

    int m_pos, m_from, m_to;
    const Array<CMail*>* m_array;

};

//--------------------------------------------------------------------------------------------------------
//-------------------------------------------CMAILSERVER--------------------------------------------------
//--------------------------------------------------------------------------------------------------------
class CMailServer
{
  //--------------------------------------------PUBLIC----------------------------------------------------
  public:

    CMailServer(void) {}

    CMailServer(const CMailServer & src) {
      for (int i = 0; i < src.fromSorted.size(); i++) {
        CMail* mailFrom = new CMail(src.fromSorted[i]->getFrom(), src.fromSorted[i]->getTo(), src.fromSorted[i]->getBody());
        CMail* mailTo = new CMail(src.toSorted[i]->getFrom(), src.toSorted[i]->getTo(), src.toSorted[i]->getBody());
        this->fromSorted.pushBack(mailFrom);
        this->toSorted.pushBack(mailTo);

      }
    }

    CMailServer& operator = (const CMailServer & src) {
      if (&src == this) {
        return *this;
      }
      for (int i = 0; i < fromSorted.size(); i++) {
        delete fromSorted[i];
      }
      for (int i = 0; i < toSorted.size(); i++) {
        delete toSorted[i];
      }
      fromSorted.setSize(0);
      toSorted.setSize(0);

      for (int i = 0; i < src.fromSorted.size(); i++) {
        CMail* mailFrom = new CMail(src.fromSorted[i]->getFrom(), src.fromSorted[i]->getTo(), src.fromSorted[i]->getBody());
        CMail* mailTo = new CMail(src.toSorted[i]->getFrom(), src.toSorted[i]->getTo(), src.toSorted[i]->getBody());
        this->fromSorted.pushBack(mailFrom);
        this->toSorted.pushBack(mailTo);
      }

      return *this;
    }

    ~CMailServer(void) {
      for (int i = 0; i < fromSorted.size(); i++) {
        delete fromSorted[i];
      }
      for (int i = 0; i < toSorted.size(); i++) {
        delete toSorted[i];
      }
    }

    void SendMail(const CMail& m) {
      CMail* mailFrom = new CMail(m.getFrom(), m.getTo(), m.getBody());
      CMail* mailTo = new CMail(m.getFrom(), m.getTo(), m.getBody());

      if (fromSorted.size() == 0) {
        fromSorted.pushBack(mailFrom);
        toSorted.pushBack(mailTo);
      }
      else {
        fromSorted.insert(upperBound(mailFrom->getFrom(), fromSorted), mailFrom);
        toSorted.insert(upperBound(mailTo->getTo(), toSorted), mailTo);
      }

    }

    CMailIterator Outbox(const char* email) const {
      int from = lowerBound(email, fromSorted);
      int to = upperBound(email, fromSorted);
      return CMailIterator(&fromSorted, from, to);
    }

    CMailIterator Inbox(const char* email) const {
      int from = lowerBound(email, toSorted);
      int to = upperBound(email, toSorted);
      return CMailIterator(&toSorted, from, to);
    }

  //--------------------------------------------PRIVATE---------------------------------------------------
  private:

    Array<CMail*> fromSorted;
    Array<CMail*> toSorted;

    int lowerBound(const char* email, const Array<CMail*>& array) const {

      int low = 0;
      int high = array.size() - 1;
      int mid = 0;
      bool from;

      if (&array == &fromSorted) {
        from = true;
      }
      else {
        from = false;
      }

      while (low <= high) {
        mid = (low+high)/2;
        if (areEqual(from ? array[mid]->getFrom() : array[mid]->getTo(), email)) {
          //res = mid;
          high = mid - 1;
        }
        else if(biggerThan(from ? array[mid]->getFrom() : array[mid]->getTo(), email)) {
          high = mid - 1;
        }
        else {
          low = mid + 1;
        }
      }
      if (biggerThan(email, from ? array[mid]->getFrom() : array[mid]->getTo())) {
        return mid + 1;
      }
      return mid;
    }

    int upperBound(const char* email, const Array<CMail*>& array) const {

      int low = 0;
      int high = array.size() - 1;
      int mid = 0;
      bool from;

      if (&array == &fromSorted) {
        from = true;
      }
      else {
        from = false;
      }

      while (low <= high) {
        mid = (low+high)/2;
        if (areEqual(from ? array[mid]->getFrom() : array[mid]->getTo(), email)) {
          //res = mid;
          low = mid + 1;
        }
        else if(biggerThan(from ? array[mid]->getFrom() : array[mid]->getTo(), email)) {
          high = mid - 1;
        }
        else {
          low = mid + 1;
        }
      }
      if (smallerThan(email, from ? array[mid]->getFrom() : array[mid]->getTo())) {
        return mid;
      }
      return mid + 1;
    }

};

#ifndef __PROGTEST__
int main ( void )
{
  char from[100], to[100], body[1024];

  assert ( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "john", "peter", "progtest deadline" ) );
  assert ( !( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "john", "progtest deadline", "peter" ) ) );
  assert ( !( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "peter", "john", "progtest deadline" ) ) );
  assert ( !( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "peter", "progtest deadline", "john" ) ) );
  assert ( !( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "progtest deadline", "john", "peter" ) ) );
  assert ( !( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "progtest deadline", "peter", "john" ) ) );
  CMailServer s0;
  s0 . SendMail ( CMail ( "john", "peter", "some important mail" ) );
  strncpy ( from, "john", sizeof ( from ) );
  strncpy ( to, "thomas", sizeof ( to ) );
  strncpy ( body, "another important mail", sizeof ( body ) );
  s0 . SendMail ( CMail ( from, to, body ) );
  strncpy ( from, "john", sizeof ( from ) );
  strncpy ( to, "alice", sizeof ( to ) );
  strncpy ( body, "deadline notice", sizeof ( body ) );
  s0 . SendMail ( CMail ( from, to, body ) );
  s0 . SendMail ( CMail ( "alice", "john", "deadline confirmation" ) );
  s0 . SendMail ( CMail ( "peter", "alice", "PR bullshit" ) );
  CMailIterator i0 = s0 . Inbox ( "alice" );
  assert ( i0 && *i0 == CMail ( "john", "alice", "deadline notice" ) );
  assert ( ++i0 && *i0 == CMail ( "peter", "alice", "PR bullshit" ) );
  assert ( ! ++i0 );

  CMailIterator i1 = s0 . Inbox ( "john" );
  assert ( i1 && *i1 == CMail ( "alice", "john", "deadline confirmation" ) );
  assert ( ! ++i1 );

  CMailIterator i2 = s0 . Outbox ( "john" );
  assert ( i2 && *i2 == CMail ( "john", "peter", "some important mail" ) );
  assert ( ++i2 && *i2 == CMail ( "john", "thomas", "another important mail" ) );
  assert ( ++i2 && *i2 == CMail ( "john", "alice", "deadline notice" ) );
  assert ( ! ++i2 );

  CMailIterator i3 = s0 . Outbox ( "thomas" );
  assert ( ! i3 );

  CMailIterator i4 = s0 . Outbox ( "steve" );
  assert ( ! i4 );

  CMailIterator i5 = s0 . Outbox ( "thomas" );
  s0 . SendMail ( CMail ( "thomas", "boss", "daily report" ) );
  assert ( ! i5 );

  CMailIterator i6 = s0 . Outbox ( "thomas" );
  assert ( i6 && *i6 == CMail ( "thomas", "boss", "daily report" ) );
  assert ( ! ++i6 );

  CMailIterator i7 = s0 . Inbox ( "alice" );
  s0 . SendMail ( CMail ( "thomas", "alice", "meeting details" ) );
  assert ( i7 && *i7 == CMail ( "john", "alice", "deadline notice" ) );
  assert ( ++i7 && *i7 == CMail ( "peter", "alice", "PR bullshit" ) );
  assert ( ! ++i7 );

  CMailIterator i8 = s0 . Inbox ( "alice" );
  assert ( i8 && *i8 == CMail ( "john", "alice", "deadline notice" ) );
  assert ( ++i8 && *i8 == CMail ( "peter", "alice", "PR bullshit" ) );
  assert ( ++i8 && *i8 == CMail ( "thomas", "alice", "meeting details" ) );
  assert ( ! ++i8 );

  CMailServer s1 ( s0 );
  s0 . SendMail ( CMail ( "joe", "alice", "delivery details" ) );
  s1 . SendMail ( CMail ( "sam", "alice", "order confirmation" ) );
  CMailIterator i9 = s0 . Inbox ( "alice" );
  assert ( i9 && *i9 == CMail ( "john", "alice", "deadline notice" ) );
  assert ( ++i9 && *i9 == CMail ( "peter", "alice", "PR bullshit" ) );
  assert ( ++i9 && *i9 == CMail ( "thomas", "alice", "meeting details" ) );
  assert ( ++i9 && *i9 == CMail ( "joe", "alice", "delivery details" ) );
  assert ( ! ++i9 );

  CMailIterator i10 = s1 . Inbox ( "alice" );
  assert ( i10 && *i10 == CMail ( "john", "alice", "deadline notice" ) );
  assert ( ++i10 && *i10 == CMail ( "peter", "alice", "PR bullshit" ) );
  assert ( ++i10 && *i10 == CMail ( "thomas", "alice", "meeting details" ) );
  assert ( ++i10 && *i10 == CMail ( "sam", "alice", "order confirmation" ) );
  assert ( ! ++i10 );

  CMailServer s2;
  s2 . SendMail ( CMail ( "alice", "alice", "mailbox test" ) );
  CMailIterator i11 = s2 . Inbox ( "alice" );
  assert ( i11 && *i11 == CMail ( "alice", "alice", "mailbox test" ) );
  assert ( ! ++i11 );

  s2 = s0;
  s0 . SendMail ( CMail ( "steve", "alice", "newsletter" ) );
  s2 . SendMail ( CMail ( "paul", "alice", "invalid invoice" ) );
  CMailIterator i12 = s0 . Inbox ( "alice" );
  assert ( i12 && *i12 == CMail ( "john", "alice", "deadline notice" ) );
  assert ( ++i12 && *i12 == CMail ( "peter", "alice", "PR bullshit" ) );
  assert ( ++i12 && *i12 == CMail ( "thomas", "alice", "meeting details" ) );
  assert ( ++i12 && *i12 == CMail ( "joe", "alice", "delivery details" ) );
  assert ( ++i12 && *i12 == CMail ( "steve", "alice", "newsletter" ) );
  assert ( ! ++i12 );

  CMailIterator i13 = s2 . Inbox ( "alice" );
  assert ( i13 && *i13 == CMail ( "john", "alice", "deadline notice" ) );
  assert ( ++i13 && *i13 == CMail ( "peter", "alice", "PR bullshit" ) );
  assert ( ++i13 && *i13 == CMail ( "thomas", "alice", "meeting details" ) );
  assert ( ++i13 && *i13 == CMail ( "joe", "alice", "delivery details" ) );
  assert ( ++i13 && *i13 == CMail ( "paul", "alice", "invalid invoice" ) );
  assert ( ! ++i13 );

  return 0;
}
#endif /* __PROGTEST__ */
